#include <napi.h>
#include "clCRID.h"

namespace usm {

static Napi::Object _init(Napi::Env env, Napi::Object exports) {
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, _init)

}
