# frozen_string_literal: true

require_relative "test_helper"

module Onigmo
  class OnigmoTest < Test::Unit::TestCase
    def test_parse
      Onigmo.parse("abc")
    end
  end
end
