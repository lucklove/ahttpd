#include "ahttpd.hh"

using namespace ahttpd;

int
main(int argc, char *argv[])
{
	if(argc != 4) {
		std::cout << "USEAGE: ./mail dest@destdomain.com subject body" << std::endl;
		return -1;
	}
	Mail m("ahttpd@163.com", "qjhjhnevjghjdoml", "smtp.163.com");
	m.send(argv[1], 
		[=](MailPkgPtr pkg) {
			pkg->addHeader("Content-Type", "text/html");
			pkg->addHeader("subject", argv[2]);
			pkg->out() << argv[3];
		},
		[=](bool good) {
			if(!good)
				std::cout << "send mail to "<< argv[1] << " failed" << std::endl;
		}
	);
	m.apply();
}
