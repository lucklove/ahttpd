#include "base64.hh"

#define INV	    -1
#define PADDING	    '='

namespace {

const char base64_char[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/' 
};

int 
base256_char(char c)
{
	if(c >= 'A' && c <= 'Z') {
		return c - 'A';
	} else if (c >= 'a' && c <= 'z') {
		return c - 'a' + 26;
	} else if (c >= '0' && c <= '9') {
		return c - '0' + 52;
	} else if (c == '+') {
		return 62;
	} else if (c == '/') {
		return 63;
	} else {
		return INV;
	}
}

void 
base256to64(char c1, char c2, char c3, int padding, std::string& output)
{
	output.push_back(base64_char[c1>>2]);
	output.push_back(base64_char[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)]);
	switch(padding) {
		case 0:
			output.push_back(base64_char[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)]);
			output.push_back(base64_char[c3 & 0x3F]);
			break;
		case 1:
			output.push_back(base64_char[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)]);
			output.push_back(PADDING);
			break;
		case 2:
		default:
			output.push_back(PADDING);
			output.push_back(PADDING);
			break;
	}
}
}

std::string
Base64::encode(const std::string& str)
{
    std::string output;
    char c1, c2, c3;
    size_t i = 0;

    while (i < str.size()) {
	c1 = str[i++];
	if(i == str.size()) {
	    base256to64(c1, 0, 0, 2, output);
	    break;
	} else {
	    c2 = str[i++];
	    if (i == str.size()) {
		base256to64(c1, c2, 0, 1, output);
		break;
	    } else {
		c3 = str[i++];
		base256to64(c1, c2, c3, 0, output);
	    }
	}
    }
    return output;
}


std::string
Base64::decode(const std::string& str)
{
	std::string output;
	auto len = str.size();

	while (len && str[len-1] == '=')
		--len;

	for(size_t i = 0; i < len;) {
		/** Fill up c, silently ignoring invalid characters */
		size_t k;
		int c[4];
		for(k = 0; k < 4 && i < len; ++k) {
			do {
				c[k] = base256_char(str[i++]);
			} while (c[k] == INV && i < len);
		}

		if(k < 4) {
			if(k > 1) {
				output.push_back((c[0] << 2) | ((c[1] & 0x30) >> 4));
				if(k > 2)
					output.push_back(((c[1] & 0x0F) << 4) | ((c[2] & 0x3C) >> 2));
			}
			break;
		}
		output.push_back((c[0] << 2) | ((c[1] & 0x30) >> 4));
		output.push_back(((c[1] & 0x0F) << 4) | ((c[2] & 0x3C) >> 2));
		output.push_back(((c[2] & 0x03) << 6) | (c[3] & 0x3F));
	}
	return output;
}
