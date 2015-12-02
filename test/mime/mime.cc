#include "UnitTest.hh"
#include "MimeType.hh"

using namespace ahttpd;

TEST_CASE(mime_test)
{
    TEST_CHECK(guessMimeType("no_dot") == "text/plain");
    TEST_CHECK(guessMimeType("something.htm") == "text/html");
    TEST_CHECK(guessMimeType("something.txt") == "text/plain");
    TEST_CHECK(guessMimeType("something.323") == "text/h323");
}
