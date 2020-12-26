#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include <clocale>
#include <cstdlib>
#include <cstring>
#include <cstddef>

#include "fopen.h"

std::wstring a2w(const std::string& str, unsigned int windowscp, const char* locale) {
#ifdef _WIN32
  int len = MultiByteToWideChar(windowscp, 0, str.c_str(), -1, nullptr, 0);
  if (len == 0) return L"";
  wchar_t* buf = new wchar_t[len];
  memset(buf, 0, len * sizeof(wchar_t));
  MultiByteToWideChar(windowscp, 0, str.c_str(), -1, buf, len);
  std::wstring res(buf);
  delete[] buf;
  return res;
#else
  std::string target_locale = locale;
  const char* c_locale = std::setlocale(LC_CTYPE, nullptr);
  std::string locale(c_locale ? c_locale : "");

  if (locale != target_locale) {
    std::setlocale(LC_CTYPE, target_locale.c_str());
  }

  size_t len = std::mbstowcs(nullptr, str.c_str(), 0);
  wchar_t* buf = new wchar_t[len + 1];
  memset(buf, 0, (len + 1) * sizeof(wchar_t));
  std::mbstowcs(buf, str.c_str(), len + 1);
  std::wstring res(buf);
  delete[] buf;

  if (locale != "") {
    std::setlocale(LC_CTYPE, locale.c_str());
  }
  return res;
#endif
}

std::string w2a(const std::wstring& wstr, unsigned int windowscp, const char* locale) {
#ifdef _WIN32
  int len = WideCharToMultiByte(windowscp, 0, wstr.c_str(), -1, nullptr, 0, NULL, NULL);
  if (len == 0) return "";
  char* buf = new char[len];
  memset(buf, 0, len * sizeof(char));
  WideCharToMultiByte(windowscp, 0, wstr.c_str(), -1, buf, len, NULL, NULL);
  std::string res(buf);
  delete[] buf;
  return res;
#else
  std::string target_locale = locale;
  const char* c_locale = std::setlocale(LC_CTYPE, nullptr);
  std::string locale(c_locale ? c_locale : "");

  if (locale != target_locale) {
    std::setlocale(LC_CTYPE, target_locale.c_str());
  }

  size_t len = std::wcstombs(nullptr, wstr.c_str(), 0);
  char* buf = new char[len + 1];
  memset(buf, 0, (len + 1) * sizeof(char));
  std::wcstombs(buf, wstr.c_str(), len + 1);
  std::string res(buf);
  delete[] buf;

  if (locale != "") {
    std::setlocale(LC_CTYPE, locale.c_str());
  }
  return res;
#endif
}

std::string shiftjis2utf8 (const std::string& str) {
  return w2a(a2w(str, CODEPAGE_SHIFT_JIS, LOCALE_SHIFT_JIS), CODEPAGE_UTF8, LOCALE_UTF8);
}


FILE* utf8_fopen (const char* path, const char* mode) {
#ifdef _WIN32
  std::wstring wpath = a2w(path, CODEPAGE_UTF8, LOCALE_UTF8);
  std::wstring wmode = a2w(mode, CODEPAGE_UTF8, LOCALE_UTF8);
  return _wfopen(wpath.c_str(), wmode.c_str());
#else
  return fopen(path, mode);
#endif
}
