#include <v8.h>
#include <node.h>
using namespace v8;
extern "C" {
    static void init(Handle<Object> target) {}
    NODE_MODULE(module_name, init)
}
 
