#ifndef __FOPEN_H__
#define __FOPEN_H__

#include <cstdio>
#include <string>

#define CODEPAGE_SHIFT_JIS (932)
#define CODEPAGE_UTF8 (65001)
#define LOCALE_SHIFT_JIS ("ja_JP.sjis")
#define LOCALE_UTF8 ("en_US.utf8")

FILE* utf8_fopen (const char* path, const char* mode);
std::string shiftjis2utf8 (const std::string& str);

#endif
