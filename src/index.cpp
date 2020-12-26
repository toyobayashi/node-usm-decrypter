#include <napi.h>
#include "clCRID.h"
#include "fopen.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#endif

static bool is_dir (const std::string& path) {
  int code = 0;
#ifdef _WIN32
  struct _stat info;
  std::wstring wpath = a2w(path, CODEPAGE_UTF8, LOCALE_UTF8);
  code = _wstat(wpath.c_str(), &info);
  if (code != 0) {
    return false;
  }
  return ((info.st_mode & S_IFMT) == S_IFDIR);
#else
  struct stat info;
  code = ::stat(path.c_str(), &info);
  if (code != 0) {
    return false;
  }
  return S_ISDIR(info.st_mode);
#endif
}

class USMDecrypter : public Napi::ObjectWrap<USMDecrypter> {
public:
  static Napi::Object init(Napi::Env env, Napi::Object exports);
  USMDecrypter(const Napi::CallbackInfo &info);
  virtual ~USMDecrypter();

  clCRID* crid();

private:
  static Napi::FunctionReference _constructor;
  clCRID* _crid;
  
  unsigned int ciphKey1;
  unsigned int ciphKey2;

  Napi::Value _demux(const Napi::CallbackInfo &info);
  Napi::Value _demuxSync(const Napi::CallbackInfo &info);
};

class USMAsyncWorker : public Napi::AsyncWorker {
public:
  USMAsyncWorker(clCRID*, const std::string&, const std::string&, bool, const Napi::Function&);
  void Execute();
  void OnOK();
private:
  clCRID _crid;
  std::string _usmFile;
  std::string _outdir;
  bool _adxDecode;
  bool _res;
  std::string _errmsg;
};

USMAsyncWorker::USMAsyncWorker(clCRID* crid, const std::string& usmFile, const std::string& outdir, bool adxDecode, const Napi::Function& callback): Napi::AsyncWorker(callback) {
  _crid = *crid;
  _usmFile = usmFile;
  _outdir = outdir;
  _adxDecode = adxDecode;
  _res = false;
  _errmsg = "";
}

void USMAsyncWorker::Execute() {
  if (!is_dir(_outdir)) {
    _errmsg = "USMDecrypter::demux(): fs.statSync(arguments[1]).isDirectory() !== true";
    _res = Napi::Boolean::New(Env(), false);
    return;
  }
  _res = _crid.Demux(_usmFile.c_str(), _outdir.c_str(), _adxDecode);
}

void USMAsyncWorker::OnOK() {
  Napi::Env env = Env();
  if (_res) {
    if (!Callback().IsEmpty()) {
      Callback().Call({ env.Null(), Napi::String::New(env, _outdir) });
    }
  } else {
    if (!Callback().IsEmpty()) {
      Callback().Call({ Napi::Error::New(env, _errmsg == "" ? (_usmFile + " decrypt failed.") : _errmsg).Value(), Napi::String::New(env, "") });
    }
  }
}

Napi::FunctionReference USMDecrypter::_constructor;

Napi::Object USMDecrypter::init(Napi::Env env, Napi::Object exports) {
  // This method is used to hook the accessor and method callbacks
  Napi::Function classConstructor = DefineClass(env, "USMDecrypter", {
    InstanceMethod("demux", &USMDecrypter::_demux),
    InstanceMethod("demuxSync", &USMDecrypter::_demuxSync)
  });

  _constructor = Napi::Persistent(classConstructor);

  _constructor.SuppressDestruct();
  exports.Set("USMDecrypter", classConstructor);
  return exports;
}

USMDecrypter::USMDecrypter(const Napi::CallbackInfo &info) : Napi::ObjectWrap<USMDecrypter>(info) {
  _crid = nullptr;
  // Napi::Env env = info.Env();
  ciphKey1 = 0xF27E3B22;
  ciphKey2 = 0x00003657;
  size_t argc = info.Length();

  if (argc < 1) {
    _crid = new clCRID(ciphKey1, ciphKey2);
  } else if (argc < 2) {
    if (info[0].IsNumber()) {
      ciphKey1 = info[0].As<Napi::Number>().Uint32Value();
    }
    _crid = new clCRID(ciphKey1, ciphKey2);
  } else {
    if (info[0].IsNumber()) {
      ciphKey1 = info[0].As<Napi::Number>().Uint32Value();
    }

    if (info[1].IsNumber()) {
      ciphKey2 = info[1].As<Napi::Number>().Uint32Value();
    }
    _crid = new clCRID(ciphKey1, ciphKey2);
  }
}

USMDecrypter::~USMDecrypter() {
  if (_crid) {
    delete _crid;
    _crid = nullptr;
  }
}

clCRID* USMDecrypter::crid() {
  return _crid;
}

static Napi::Value noop(const Napi::CallbackInfo &info) { return info.Env().Undefined(); }

Napi::Value USMDecrypter::_demux(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();
  size_t argc = info.Length();
  if (argc < 1) {
    Napi::Error::New(env, "USMDecrypter::demux(): arguments.length < 1").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  if (!info[0].IsString()) {
    Napi::Error::New(env, "USMDecrypter::demux(): typeof arguments[0] !== 'string'").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  std::string usm = "";
  std::string outdir = "";
  bool adxDecode = true;
  Napi::Function callback;

  size_t i = 0;

  for (i = 0; i < argc; i++) {
    if (info[i].IsFunction()) {
      callback = info[i].As<Napi::Function>();
      break;
    } else {
      switch (i) {
        case 0:
          usm = info[i].As<Napi::String>().Utf8Value();
          outdir = usm.find_last_of(".") == std::string::npos ? (usm + ".demux") : (usm.substr(0, usm.find_last_of(".")) + ".demux");
          break;
        case 1:
          if (info[i].IsString()) outdir = info[i].As<Napi::String>().Utf8Value();
          break;
        case 2:
          if (info[i].IsBoolean()) adxDecode = info[i].As<Napi::Boolean>().Value();
          break;
        default:
          break;
      }
    }
  }
  if (callback.IsEmpty()) {
    callback = Napi::Function::New(env, noop);
  }
  USMAsyncWorker* worker = new USMAsyncWorker(_crid, usm, outdir, adxDecode, callback);
  worker->Queue();

  return env.Undefined();
}

Napi::Value USMDecrypter::_demuxSync(const Napi::CallbackInfo &info){
  Napi::Env env = info.Env();
  size_t argc = info.Length();
  if (argc < 1) {
    Napi::Error::New(env, "USMDecrypter::demuxSync(): arguments.length < 1").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  if (!info[0].IsString()) {
    Napi::Error::New(env, "USMDecrypter::demuxSync(): typeof arguments[0] !== 'string'").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  std::string usm = "";
  std::string outdir = "";
  bool adxDecode = true;

  size_t i = 0;

  for (i = 0; i < argc; i++) {
    switch (i) {
      case 0:
        usm = info[i].As<Napi::String>().Utf8Value();
        outdir = usm.find_last_of(".") == std::string::npos ? (usm + ".demux") : (usm.substr(0, usm.find_last_of(".")) + ".demux");
        break;
      case 1:
        if (info[i].IsString()) outdir = info[i].As<Napi::String>().Utf8Value();
        break;
      case 2:
        if (info[i].IsBoolean()) adxDecode = info[i].As<Napi::Boolean>().Value();
        break;
      default:
        break;
    }
  }

  if (!is_dir(outdir)) {
    Napi::Error::New(env, "USMDecrypter::demuxSync(): fs.statSync(arguments[1]).isDirectory() !== true").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }

  bool res = _crid->Demux(usm.c_str(), outdir.c_str(), adxDecode);
  if (!res) {
    Napi::Error::New(env, usm + " decrypt failed.").ThrowAsJavaScriptException();
    return Napi::String::New(env, "");
  }
  return Napi::String::New(env, outdir);
}

static Napi::Object _init(Napi::Env env, Napi::Object exports) {
  USMDecrypter::init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, _init)
