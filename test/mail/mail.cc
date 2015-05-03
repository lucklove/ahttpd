#include <boost/test/unit_test.hpp>
#include "mail.hh"
#include "utils.hh"

BOOST_AUTO_TEST_CASE(mail_test)
{
	Mail m("ahttpd@163.com", "qjhjhnevjghjdoml", "smtp.163.com", "465", true);
	m.send("1041324091@qq.com", "TEST REPORT", 
		"A new test passed at " + gmtTime(time(nullptr)) + " GMT", 
		[=](bool good) {
			BOOST_CHECK(good);
		}
	);
	m.apply();
}
