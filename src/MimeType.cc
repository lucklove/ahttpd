#include "MimeType.hh"

namespace ahttpd {

namespace 
{
struct map_t 
{
	const char* extension;
	const char* mime_type;
} maps[] = {
	{ "323", "text/h323" },
	{ "aif", "audio/x-aiff" },
	{ "aifc", "audio/x-aiff" },
	{ "aiff", "audio/x-aiff" },
	{ "au", "audio/basic" },
	{ "avi", "video/x-msvideo" },
	{ "css", "text/css" },
	{ "flv", "video/x-flv" },
	{ "gif", "image/gif" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "jpg", "image/jpeg" },
	{ "js", "text/javascript" },
	{ "mp3", "audio/mp3" },
	{ "mp4", "video/mp4" },
	{ "oga", "audio/ogg" },
	{ "ogg", "audio/ogg" },
	{ "ogv", "video/ogg" },
	{ "png", "image/png" },
	{ "svg", "image/svg+xml" },
	{ "swf", "application/x-shockwave-flash" },
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

}	/**< namespace ahttpd */
