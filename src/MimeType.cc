#include "MimeType.hh"

namespace {
struct map_t {
	const char* extension;
	const char* mime_type;
} maps[] = {
	{ "gif", "image/gif" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "jpg", "image/jpeg" },
	{ "png", "image/png" }
};
}

std::string
guessMimeType(const std::string& path)
{
	auto pos = path.rfind('.');
	if(pos == path.npos)
		return "text/plain";
	
	auto extension = path.substr(pos + 1, path.size());
	
	for(auto m : maps) {
		if(m.extension == extension)
			return m.mime_type;
	}
	
	return "text/plain";
}
