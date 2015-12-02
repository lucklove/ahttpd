#include "parser.hh"
#include "server.hh"
#include "log.hh"
#include "utils.hh"
#include "connection.hh"
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>


#define STACK_BUFF_SIZE    (1024 * 1024)

namespace ahttpd 
{

namespace 
{
template<typename Pac_t, typename Handle_t>
void parse_headers(Pac_t pac, Handle_t handler)
{
    pac->connection()->asyncReadUntil("\n",
        [=](const ::boost::system::error_code& err, size_t) 
    {
        if(err) 
        {
            handler(nullptr);
            return;
        }
        static const boost::regex key_val_reg("([[:print:]]+): ([[:print:]]*)");
        boost::smatch results;
        std::istream in(&pac->connection()->readBuffer());
        std::string line;
        getline(in, line);
        if(line == "\r" || line == "")          /**< 头部最后的\r\n" */
        {    
            handler(pac);
            return;
        }
        if(boost::regex_search(line, results, key_val_reg))
            pac->addHeader(results.str(1), results.str(2));
        parse_headers(pac, handler);
    });                
}

template<typename Pac_t, typename Handle_t>
void read_chunked_body(Pac_t pac, Handle_t handler)
{
    auto conn = pac->connection();
    if(!conn) return;
    conn->asyncReadUntil("\n",
        [=](const ::boost::system::error_code& err, size_t) 
    {
        if(err) 
        {
            Log("DEBUG") << __FILE__ << ":" << __LINE__;
            Log("ERROR") << err.message();
            handler(nullptr);
            return;
        }
        if(!pac->connection())            /**< 判断在其他地方是否出现解析错误的情况 */
            return;
        std::istream in(&pac->connection()->readBuffer());
        size_t length = 0;
        in >> std::hex >> length;
        in.ignore(0x7FFFFFFF, '\n');

        size_t already_read = pac->connection()->readBuffer().in_avail();
        int need_read = length - already_read;
        if(length > STACK_BUFF_SIZE) 
        {
            Log("ERROR") << "BUFFER OVERFLOW";
            Log("NOTE") << "LENGTH: " << length;
            handler(nullptr);
            return;
        }
        if(need_read > 0) 
        {
            conn->asyncRead(::boost::asio::transfer_exactly(need_read),
                [=](const ::boost::system::error_code& err, size_t n) 
            {
                if(err || static_cast<int>(n) < need_read) 
                { 
                    if(err) 
                    {
                        Log("DEBUG") << __FILE__ << ":" << __LINE__;
                        Log("ERROR") << err.message();
                    }
                    if(static_cast<int>(n) != need_read)
                        Log("ERROR") << "BUFFER HUNGERY";
                    handler(nullptr);
                    return;
                }
                if(!pac->connection())
                    return;

                char buf[STACK_BUFF_SIZE];
                std::istream in(&pac->connection()->readBuffer());
                in.read(buf, length);
                pac->out().write(buf, length);
            });
        } 
        else if(length > 0) 
        {
            char buf[STACK_BUFF_SIZE];
            in.read(buf, length);
            pac->out().write(buf, length);    
        }

        conn->asyncReadUntil("\n",
            [=](const ::boost::system::error_code& err, size_t n) 
        {
            if(err) 
            {
                Log("DEBUG") << __FILE__ << ":" << __LINE__;
                Log("ERROR") << err.message();
                if(length != 0)
                    handler(nullptr);
                return;
            }
            if(!pac->connection())
                return;
            std::istream in(&conn->readBuffer());
            in.ignore(0x7FFFFFFF, '\n');
            if(length == 0) 
            {
                handler(pac);
                return;
            }
        });
        if(length)
            read_chunked_body(pac, handler);
    });    
}

template<typename Pac_t, typename Handle_t>
void parse_body(Pac_t pac, Handle_t handler)
{
    auto h = pac->getHeader("Content-Length");
    if(h == nullptr) 
    {
        auto h = pac->getHeader("Transfer-Encoding");
        if(h != nullptr && strcasecmp(h->c_str(), "chunked")  == 0) 
        {
            read_chunked_body(pac, handler);
        } 
        else 
        {
            handler(pac);
        }
    } 
    else 
    {
        size_t length = 0;
        try 
        {
            length = ::boost::lexical_cast<size_t>(*h);
        } 
        catch(::boost::bad_lexical_cast &e) 
        {
            Log("DEBUG") << __FILE__ << ":" << __LINE__;
            Log("ERROR") << e.what();
            handler(nullptr);
            return;
        }
        if(length == 0) 
        {
            handler(pac);
            return;
        }

        size_t already_read = pac->connection()->readBuffer().in_avail();
        int need_read = length - already_read;

        if(need_read < 0) 
        {
            handler(nullptr);
            return;
        }

        if(already_read)
            pac->out() << &pac->connection()->readBuffer();

        if(need_read == 0) 
        {
            handler(pac);
            return;
        }

        pac->connection()->asyncRead(::boost::asio::transfer_exactly(need_read),
            [=](const ::boost::system::error_code& err, size_t n) 
        {
            if(err || static_cast<int>(n) != need_read)         /**< 避免警告 */
            {    
                handler(nullptr);
                return;
            }
            pac->out() << &pac->connection()->readBuffer();
            handler(pac);
        });
    }
}

void parse_request_first_line(RequestPtr req, std::function<void(RequestPtr)> handler)
{
    req->connection()->asyncReadUntil("\n", 
        [req, handler](const ::boost::system::error_code& err, size_t n) 
    {
        if(err) 
        {
            handler(nullptr);
            return;
        }
        std::istream in(&req->connection()->readBuffer());
        std::string line;
        getline(in, line);
        StringTokenizer first_line_st(line);
        std::string url, path;
        StringTokenizer url_st;

        if(!first_line_st.hasMoreTokens())
            goto bad_request;
        req->setMethod(first_line_st.nextToken());

        if(!first_line_st.hasMoreTokens())
            goto bad_request;
        url = first_line_st.nextToken();
        url_st = StringTokenizer(url, '?');
        path = url_st.nextToken();
        if(!urlDecode(path))
            goto bad_request;
        req->setPath(path);
        if(url_st.hasMoreTokens())
            req->setQueryString(url_st.nextToken());
        if(!first_line_st.hasMoreTokens())
            goto bad_request;
        req->setVersion(first_line_st.nextToken());

        handler(req);
        return;

        bad_request:
            Log("WARNING") << "Bad request";
            handler(nullptr);
    });
}

void parse_response_first_line(ResponsePtr res, std::function<void(ResponsePtr)> handler)
{
    res->connection()->asyncReadUntil("\n", 
        [=](const ::boost::system::error_code& err, size_t n) 
    {
        if(err) 
        {
            Log("DEBUG") << __FILE__ << ":" << __LINE__;
            Log("ERROR") << err.message();
            handler(nullptr);
            return;
        }
        boost::smatch results;
        static const boost::regex first_line_reg("([[:print:]]*) ([0-9]*) (((?!\r)[[:print:] ])*)");
        std::istream in(&res->connection()->readBuffer());
        std::string line;
        getline(in, line);
        if(boost::regex_search(line, results, first_line_reg)) 
        {
            res->setVersion(results.str(1));
            try 
            {
                res->setStatus(::boost::lexical_cast<Response::status_t>(results.str(2)));
            } 
            catch(::boost::bad_lexical_cast &e) 
            {
                handler(nullptr);
                return;
            }
            res->setMessage(results.str(3));
            handler(res);
        } 
        else 
        {
            handler(nullptr);
        }
    });
}

}

#define RETURN_IF_ERROR(result, pack, handler)                          \
do                                                                      \
{                                                                       \
    if(!result)                                                         \
    {                                                                   \
        pack->discardConnection();                                      \
        handler(nullptr);                                               \
        return;                                                         \
    }                                                                   \
}                                                                       \
while(0)

#define PARSE(package, pack, handler)                                   \
do                                                                      \
{                                                                       \
    parse_##package##_first_line(pack, [=](decltype(pack) pac)          \
    {                                                                   \
        RETURN_IF_ERROR(pac, pack, handler);                            \
        parse_headers(pac, [=](decltype(pac) pac)                       \
        {                                                               \
            RETURN_IF_ERROR(pac, pack, handler);                        \
            pac->parseCookie();                                         \
            parse_body(pac, [=](decltype(pac) pac)                      \
            {                                                           \
                RETURN_IF_ERROR(pac, pack, handler);                    \
                handler(pac);                                           \
            });                                                         \
        });                                                             \
    });                                                                 \
}                                                                       \
while(0)
        
void parseRequest(ConnectionPtr conn, std::function<void(RequestPtr)> handler)
{
    RequestPtr req = std::make_shared<Request>(conn);    
    PARSE(request, req, handler);
}

void parseResponse(ConnectionPtr conn, std::function<void(ResponsePtr)> handler)
{
    ResponsePtr res = std::make_shared<Response>(conn);    
    PARSE(response, res, handler);
}

}    /**< namespace ahttpd */
