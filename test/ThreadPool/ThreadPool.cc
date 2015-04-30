#include <boost/test/unit_test.hpp>
#include "ThreadPool.hh"
#include "log.hh"

BOOST_AUTO_TEST_CASE(thread_pool_test)
{
	ThreadPool pool(1, []{ 
		Log("thread_pool_test") << "task start";
	}, []{
		Log("thread_pool_test") << "task end";
	});
	bool flag = false;
	pool.enqueue([&]{ 
		Log("thread_pool_test") << "do task";
		std::this_thread::sleep_for(std::chrono::seconds(1));
		flag = true;	
	});
	BOOST_CHECK(pool.size());
	Log("NOTE") << "pool.size() = " << pool.size();
	sleep(2);
	BOOST_CHECK(flag);
}
