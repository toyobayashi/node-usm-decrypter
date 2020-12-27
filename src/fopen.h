#ifndef __FOPEN_H__
#define __FOPEN_H__

#include <cstdio>
#include <string>

#define CODEPAGE_SHIFT_JIS (932U)
#define CODEPAGE_UTF8 (65001U)
#define LOCALE_SHIFT_JIS ("CP932")
#define LOCALE_UTF8 ("UTF-8")

FILE* utf8_fopen (const char* path, const char* mode);
std::string shiftjis2utf8 (const std::string& str, unsigned int encoding);
std::wstring a2w(const std::string& str, unsigned int windowscp, const char* locale);

#endif
