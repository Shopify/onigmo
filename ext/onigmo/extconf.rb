# frozen_string_literal: true

require "mkmf"

check_sizeof("long long")
check_sizeof("long")
check_sizeof("void*")
check_sizeof("int")

append_cflags("-Wno-deprecated-non-prototype")
append_cflags("-Wno-shorten-64-to-32")

$INCFLAGS << " -I$(srcdir)/enc"
$INCFLAGS << " -I$(srcdir)/enc/unicode"

create_makefile("onigmo/onigmo")
