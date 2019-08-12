#include <ruby.h>
int main() {
    ruby_init();
    VALUE result = rb_eval_string_protect("(123456789123456 * 12345678912345678).to_s", NULL);
    printf("result: %s\n", rb_string_value_cstr(&result));
    return ruby_cleanup(0);
}
