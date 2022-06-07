// PWH. 2022.06.07

#include "dime/misc.h"
#include <windows.h>

std::wstring ToWString(std::string const& str) {
	return ToWString(std::string_view(str));
}

std::wstring ToWString(char const* sz) {
	return ToWString(std::string_view(sz, strlen(sz)));
}

std::wstring ToWString(std::string_view sv) {
	std::wstring str;
	if (sv.empty())
		return str;

	int len{};
	int code = CP_UTF8;
	if (len = ::MultiByteToWideChar(code, MB_ERR_INVALID_CHARS, sv.data(), (int)sv.size(), nullptr, 0); len <= 0) {	// is UTF8 ?
		// NOT utf8
		code = CP_ACP;
		len = ::MultiByteToWideChar(code, 0, sv.data(), (int)sv.size(), nullptr, 0);
	}

	if (len > 0) {
		str.assign((size_t)len, 0);
		::MultiByteToWideChar(code, 0, sv.data(), (int)sv.size(), str.data(), str.size());
	}

	return str;
}
