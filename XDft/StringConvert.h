#pragma once

inline std::wstring MultiByteToWide(const std::string &s)
{
	if (s.empty())
		return std::wstring();
	int numChars = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), static_cast<int>(s.length()), 0, 0);
	std::vector<wchar_t> wbuf(numChars);
	::MultiByteToWideChar(CP_ACP, 0, s.c_str(), static_cast<int>(s.length()), &wbuf[0], numChars);
	return std::wstring(wbuf.begin(), wbuf.end());
}


inline std::string WideToMultiByte(const std::wstring &s)
{
	if (s.empty())
		return std::string();
	int numChars = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), static_cast<int>(s.length()), 0, 0, 0, 0);
	std::vector<char> buf(numChars);
	::WideCharToMultiByte(CP_ACP, 0, s.c_str(), static_cast<int>(s.length()), &buf[0], numChars, 0, 0);
	return std::string(buf.begin(), buf.end());
}