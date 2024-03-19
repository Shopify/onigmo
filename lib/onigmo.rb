# frozen_string_literal: true

module Onigmo
  require "onigmo/node"
  require "onigmo/onigmo"

  autoload :Visitor, "onigmo/visitor"
  autoload :DeconstructVisitor, "onigmo/deconstruct_visitor"
  autoload :JSONVisitor, "onigmo/json_visitor"
  autoload :PrettyPrintVisitor, "onigmo/pretty_print_visitor"
end
