# frozen_string_literal: true

require "mkmf"

append_cflags("-Wno-missing-noreturn")

create_makefile("onigmo/onigmo")
