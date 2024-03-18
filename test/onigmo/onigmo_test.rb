# frozen_string_literal: true

require_relative "test_helper"

module Onigmo
  class OnigmoTest < Test::Unit::TestCase
    def test_compile
      refute_nil Onigmo.compile("abc")
    end

    def test_compile_failure
      assert_raise(ArgumentError) { Onigmo.compile("(?<>)") }
    end
  end
end
