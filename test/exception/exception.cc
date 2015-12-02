#include "UnitTest.hh"
#include "exception.hh"
#include "log.hh"

using namespace ahttpd;

struct TestException : Exception {
    using Exception::Exception;
};

TEST_CASE(exception_test)
{
    try {
        DEBUG_THROW(TestException, "Just test Exception, don't worry, nothing bad");
    } catch(Exception& e) {
        Log("NOTE") << e;
    }
}
