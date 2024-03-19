# frozen_string_literal: true

require_relative "test_helper"

module Onigmo
  class ParseTest < Test::Unit::TestCase
    def test_success
      refute_nil Onigmo.parse("abc")
    end

    def test_failure
      assert_raise(ArgumentError) { Onigmo.parse("(?<>)") }
    end
  end
end
