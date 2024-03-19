# frozen_string_literal: true

require_relative "test_helper"

module Onigmo
  class VisitorTest < Test::Unit::TestCase
    class StringVisitor < Visitor
      attr_reader :strings

      def initialize(strings)
        @strings = strings
      end

      def visit_string_node(node)
        strings << node.value
        super
      end
    end

    def test_visit
      source =
        "\\A\\z\\K^$\\G\\Z\\b\\B.\\1\\g<1>\\w\\W" \
        "[[:print:]][^[:print:]]" \
        "(a|b|c)(?~)(?(1))(?i)(?>)" \
        "(?=)(?!)(?<=)(?<!)*"

      strings = []
      Onigmo.parse(source).accept(StringVisitor.new(strings))

      assert_equal(3, strings.reject(&:empty?).length)
    end
  end
end
