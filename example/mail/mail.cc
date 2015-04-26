#include "mail.hh"
#include "log.hh"

int
main(int argc, char *argv[])
{
	Mail m("sender@xxx.com", "senderpassword", "smtp.xxx.com");
	m.send("receiver@xxx.com", "subject", "body", [](bool good) {
		Log("DEBUG") << good;
	});
	m.apply();
}
