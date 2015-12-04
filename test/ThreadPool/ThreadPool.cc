#include "UnitTest.hh"
#include "ThreadPool.hh"
#include "log.hh"

using namespace ahttpd;

TEST_CASE(thread_pool_test)
{
    ThreadPool pool(1, []
    { 
        Log("NOTE") << "task start(thread pool)";
    }, []{
        Log("NOTE") << "task end(thread pool)";
    });
    bool flag = false;
    pool.enqueue([&]
    { 
        Log("NOTE") << "do task(thread pool)";
        flag = true;    
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    });
    TEST_CHECK(pool.size());
    Log("NOTE") << "pool.size() = " << pool.size();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    TEST_CHECK(flag);
}
