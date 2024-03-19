# frozen_string_literal: true

require_relative "test_helper"

module Onigmo
  class CompileTest < Test::Unit::TestCase
    def test_success
      refute_nil Onigmo.compile("abc")
    end

    def test_failure
      assert_raise(ArgumentError) { Onigmo.compile("(?<>)") }
    end
  end
end
