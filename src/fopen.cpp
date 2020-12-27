#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#include <iconv.h>
#endif

#include <clocale>
#include <cstdlib>
#include <cstring>
#include <cstddef>

#include "fopen.h"

std::wstring a2w(const std::string& str, unsigned int windowscp, const char* localestr) {
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
  iconv_t cd = iconv_open("UNICODE", localestr);
  if (!cd) return L"";
  char* inbuf = (char*)malloc(str.length() + 1);
  strcpy(inbuf, &str[0]);
  inbuf[str.length()] = '\0';
  char* inbuforigin = inbuf;
  size_t inbuflen = str.length() + 1;
  size_t outbuflen = 2 * inbuflen;
  char* outbuf = (char*)malloc(outbuflen);
  memset(outbuf, 0, outbuflen);
  char* outbuforigin = outbuf;
  iconv(cd, &inbuf, &inbuflen, &outbuf, &outbuflen);
  free(inbuforigin);
  iconv_close(cd);
  std::wstring res((wchar_t*)outbuforigin);
  free(outbuforigin);
  return res;
#endif
}

std::string w2a(const std::wstring& wstr, unsigned int windowscp, const char* localestr) {
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
  iconv_t cd = iconv_open(localestr, "UNICODE");
  if (!cd) return "";
  wchar_t* inbuf = (wchar_t*)malloc(wstr.length() + 1);
  wcscpy(inbuf, &wstr[0]);
  inbuf[wstr.length()] = L'\0';
  wchar_t* inbuforigin = inbuf;
  size_t inbuflen = (wstr.length() + 1) * 2;
  size_t outbuflen = 2 * inbuflen;
  char* outbuf = (char*)malloc(outbuflen);
  memset(outbuf, 0, outbuflen);
  char* outbuforigin = outbuf;
  iconv(cd, (char**)(&inbuf), &inbuflen, &outbuf, &outbuflen);
  free(inbuforigin);
  iconv_close(cd);
  std::string res(outbuforigin);
  free(outbuforigin);
  return res;
#endif
}

std::string shiftjis2utf8 (const std::string& str, unsigned int encoding) {
  if (encoding == CODEPAGE_UTF8) {
    return str;
  }
#ifdef _WIN32
  return w2a(a2w(str, CODEPAGE_SHIFT_JIS, LOCALE_SHIFT_JIS), CODEPAGE_UTF8, LOCALE_UTF8);
#else
  iconv_t cd = iconv_open("UTF-8", "CP932");
  if (!cd) return "";
  char* inbuf = (char*)malloc(str.length() + 1);
  strcpy(inbuf, &str[0]);
  inbuf[str.length()] = '\0';
  char* inbuforigin = inbuf;
  size_t inbuflen = str.length() + 1;
  size_t outbuflen = 2 * inbuflen;
  char* outbuf = (char*)malloc(outbuflen);
  memset(outbuf, 0, outbuflen);
  char* outbuforigin = outbuf;
  iconv(cd, &inbuf, &inbuflen, &outbuf, &outbuflen);
  free(inbuforigin);
  iconv_close(cd);
  std::string res(outbuforigin);
  free(outbuforigin);
  return res;
#endif
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
