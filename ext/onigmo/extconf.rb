# frozen_string_literal: true

require "mkmf"
create_makefile("onigmo/onigmo")
find_header("onigmo.h", File.join(RbConfig::CONFIG["rubyhdrdir"], "ruby"))
