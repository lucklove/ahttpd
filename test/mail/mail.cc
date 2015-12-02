#include "UnitTest.hh"
#include "mail.hh"
#include "utils.hh"

using namespace ahttpd;

TEST_CASE(mail_test)
{
    Mail m("ahttpd@163.com", "smtp.163.com", "465", true);
    m.pass("jybhsrapkkjeeddu").send("1041324091@qq.com", 
        [=](MailPkgPtr pkg) {
            pkg->addHeader("subject", "TEST REPORT");
            pkg->out() << "A new test passed at " + gmtTime(time(nullptr)) + " GMT";
        },
        [=](bool good) {
            Log("NOTE") << "check mail result";
            TEST_CHECK(good);
        }
    ).apply();
}
