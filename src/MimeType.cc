#include "MimeType.hh"

namespace {
struct map_t {
	const char* extension;
	const char* mime_type;
} maps[] = {
	{ "css", "text/css" },
	{ "gif", "image/gif" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "jpg", "image/jpeg" },
	{ "png", "image/png" },
	{ "js", "text/javascript" },
	{ "oga", "audio/ogg" },
	{ "ogg", "audio/ogg" },
	{ "ogv", "video/ogg" },
	{ "swf", "application/x-shockwave-flash" },
	{ "mp4", "video/mp4" },
	{ "flv", "video/x-flv" },
	{ "mp3", "audio/mp3" },
	{ "svg", "image/svg+xml" },
	{ "webm", "video/webm" },
	{ "xml", "application/xml" }
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
