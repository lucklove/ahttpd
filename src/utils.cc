#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include "utils.hh"
#include "log.hh"

namespace ahttpd 
{

BufferOverflow::~BufferOverflow()
{
}

TokenError::~TokenError()
{
}

namespace {

struct map_t 
{
    const char* mchar;
    const short mint;
} maps[] = {
    { "Jan", 1 },
    { "Feb", 2 },
    { "Mar", 3 },
    { "Apr", 4 },
    { "May", 5 },
    { "Jun", 6 },
    { "Jul", 7 },
    { "Aug", 8 },
    { "Sep", 9 },
    { "Oct", 10 },
    { "Nov", 11 },
    { "Dec", 12 }
};

short get_int_month(const std::string& char_month)
{
    for(const map_t& m : maps) 
    {
        if(strcasecmp(m.mchar, char_month.c_str()) == 0)
            return m.mint;
    }
    return 0;
}

void to_localtime(struct tm* utc_tm)                    /**< 将utc的tm结构转换为local时间的tm结构 */
{
    struct tm gmt, local;
    time_t t = 0;
    boost::date_time::c_time::localtime(&t, &local);
    boost::date_time::c_time::gmtime(&t, &gmt);
    utc_tm->tm_hour += local.tm_hour - gmt.tm_hour;
    utc_tm->tm_min += local.tm_min - gmt.tm_min;
    utc_tm->tm_sec += local.tm_sec - gmt.tm_sec;
}

}

time_t gmtToTime(const std::string& gmt_time)
{
    try 
    {
        struct tm timestamp;
        StringTokenizer st(gmt_time, ' ', ',');
        if(!st.hasMoreTokens())
            return 0;
        st.nextToken();            /**< 丢弃星期 */
        StringTokenizer data_st(st.nextToken(), '-');
        timestamp.tm_mday = boost::lexical_cast<unsigned short>(data_st.nextToken());
        short month = get_int_month(data_st.nextToken());
        if(!month)
            return 0;
        timestamp.tm_mon = month - 1;
        timestamp.tm_year = boost::lexical_cast<unsigned short>(data_st.nextToken()) - 1900;
        if(timestamp.tm_year < 0)                /**< DD-MM-YY */
            timestamp.tm_year += 2000;        
        StringTokenizer time_st(st.nextToken(), ':');
        timestamp.tm_hour = boost::lexical_cast<unsigned short>(time_st.nextToken());
        timestamp.tm_min = boost::lexical_cast<unsigned short>(time_st.nextToken());
        timestamp.tm_sec = boost::lexical_cast<unsigned short>(time_st.nextToken());
        /** 由于mktime参数为local时间而非UTC时间, 因此需要先转换为本地时间 */
        to_localtime(&timestamp);
        return mktime(&timestamp);
    } 
    catch(boost::bad_lexical_cast &e) 
    {
        Log("DEBUG") << __FILE__ << ":" << __LINE__;
        Log("ERROR") << e.what();
        return 0;
    } 
    catch(TokenError& e) 
    {
        Log("DEBUG") << __FILE__ << ":" << __LINE__;
        Log("ERROR") << e;
        return 0;    
    }
}

namespace 
{
struct BadUrl : Exception 
{
    using Exception::Exception;
};

unsigned char to_hex(unsigned char x)   
{   
    return  x > 9 ? x + 55 : x + 48;   
}  
  
unsigned char from_hex(unsigned char x)   
{   
    unsigned char y;  
    if(x >= 'A' && x <= 'F') 
    {
        y = x - 'A' + 10;  
    } 
    else if(x >= 'a' && x <= 'f') 
    {
        y = x - 'a' + 10;
    } 
    else if(x >= '0' && x <= '9') 
    {
        y = x - '0';
    } 
    else 
    {
        DEBUG_THROW(BadUrl, "Decode url from hex failed");
    }
    return y;  
}  
}
  
std::string urlEncode(const std::string& str)  
{  
    std::string str_temp = "";  
    size_t length = str.length();  
    for(size_t i = 0; i < length; i++) 
    {  
        if(std::isalnum(static_cast<unsigned char>(str[i])) ||   
            (str[i] == '-') ||  
            (str[i] == '_') ||   
            (str[i] == '.') ||   
            (str[i] == '~')) 
        {
            str_temp += str[i];
        } 
        else if(str[i] == ' ') 
        { 
             str_temp += "+";  
        } 
        else 
        {  
            str_temp += '%';  
            str_temp += to_hex(static_cast<unsigned char>(str[i]) >> 4);  
            str_temp += to_hex(static_cast<unsigned char>(str[i]) % 16);  
        }  
    }
    return str_temp; 
}  
  
bool urlDecode(std::string& str)  
{  
    std::string str_temp = "";  
    size_t length = str.length();  
    for(size_t i = 0; i < length; i++) 
    {  
        if(str[i] == '+') 
        {
            str_temp += ' ';
        } 
        else if(str[i] == '%') 
        {  
            if(i + 2 >= length)
                return false;
            unsigned char high = 0;
            unsigned char low = 0;
            try 
            {
                high = from_hex(static_cast<unsigned char>(str[++i]));  
                low = from_hex(static_cast<unsigned char>(str[++i]));
            } 
            catch(BadUrl&) 
            {
                return false;
            }
            str_temp += high * 16 + low;
        } 
        else 
        {
            str_temp += str[i];  
        }
    }  
    str = str_temp;
    return true;
} 

bool isValidIPAddress(const std::string& addr)
{
    if(addr.find(':') != addr.npos)     /**< ipv6 */
    {
        const static std::string valid_ipv6_character("0123456789abcdefABCDEF");
        size_t s_count = 0;            /**< 每个段中的字符数     */
        size_t seg_count = 0;            /**< 段的个数-1        */
        bool compress_flag = false;        /**< 是否存在0压缩        */
        for(auto c : addr) 
        {
            if(c == ':') 
            {
                if(s_count == 0) 
                {
                    if(compress_flag)
                        return false;
                    compress_flag = true;    
                }
                s_count = 0; 
                ++seg_count;
                continue;
            }
            if(valid_ipv6_character.find(c) == valid_ipv6_character.npos)
                return false;
            ++s_count;
            if(s_count > 4)
                return false;
        }
        if(s_count == 0 && compress_flag)
            return false;
        if(seg_count < 8)
            return true;
    } 
    else if(addr.find('.') != addr.npos)    /**< ipv4 */
    {    
        const static std::string number("0123456789");
        size_t s_count = 0;
        size_t dot_count = 0;
        std::string ip_seg;
        for(auto c : addr) 
        {
            if(c == '.') 
            {
                if(s_count == 0)
                    return false;
                if(boost::lexical_cast<size_t>(ip_seg) > 255)
                    return false;
                ip_seg = {};
                s_count = 0;
                ++dot_count;
                continue;
            }
            if(number.find(c) == number.npos)
                return false;
            ip_seg += c;
            ++s_count;
            if(s_count > 3)
                return false;
        }
        if(dot_count == 3 && s_count)
            return true;
    }
    return false;
}

bool isDomainMatch(const std::string& url, std::string base)
{
    if(base[0] == '.')
        base = base.substr(1, base.size());

    if(base.size() > url.size()) 
        return false;

    if(strcasecmp(base.c_str(), url.substr(url.size() - base.size(), url.size()).c_str()))
        return false;

    if(base.size() == url.size())
        return true;

    if(isValidIPAddress(url))                /**< 如果是IP则必须完全匹配 */
        return false;

    if(url[url.size() - base.size() - 1] != '.')
        return false;

    return true;
}

bool isPathMatch(std::string path, std::string base)
{
    if(base == "")
        return true;

    if(base == "/" && path[0] == '/')
        return true;

    if(base.size() > 1 && base[base.size() - 1] == '/')
        base = base.substr(0, base.size() - 1);

    if(path != "/" && path[path.size() - 1] == '/')
        path = path.substr(0, path.size() - 1);

    if(base.size() > path.size())
        return false;
    
    if(base != path.substr(0, base.size()))
        return false;
    
    if(base.size() == path.size())
        return true;

    if(path[base.size()] == '/')
        return true;

    return false;
}

}    /**< namespace ahttpd */
