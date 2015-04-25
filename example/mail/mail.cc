#include "mail.hh"
#include "log.hh"

int
main(int argc, char *argv[])
{
	Mail m("MDD_smart@163.com", "1245678900..", "smtp.163.com");
	m.send("1041324091@qq.com", "测试邮件", "请勿回复", [](bool good) {
		Log("DEBUG") << good;
	});
	m.apply();
}
