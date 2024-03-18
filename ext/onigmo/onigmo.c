#include "onigmo.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ruby.h>

static VALUE
parse(VALUE self, VALUE string) {
    return Qtrue;
}

void
Init_onigmo(void) {
    VALUE rb_cOnigmo = rb_define_module("Onigmo");
    rb_define_singleton_method(rb_cOnigmo, "parse", parse, 1);
}
