# frozen_string_literal: true

require_relative "test_helper"

module Onigmo
  class ParseTest < Test::Unit::TestCase
    def test_alternation_node
      assert_parses(AlternationNode, "a|b")
    end

    def test_anchor_buffer_begin_node
      assert_parses(AnchorBufferBeginNode, "\\A")
    end

    def test_anchor_buffer_end_node
      assert_parses(AnchorBufferEndNode, "\\z")
    end

    def test_anchor_keep_node
      assert_parses(AnchorKeepNode, "\\K")
    end

    def test_anchor_line_begin_node
      assert_parses(AnchorLineBeginNode, "^")
    end

    def test_anchor_line_end_node
      assert_parses(AnchorLineEndNode, "$")
    end

    def test_anchor_position_begin_node
      assert_parses(AnchorPositionBeginNode, "\\G")
    end

    def test_anchor_semi_end_node
      assert_parses(AnchorSemiEndNode, "\\Z")
    end

    def test_anchor_word_boundary_node
      assert_parses(AnchorWordBoundaryNode, "\\b")
    end

    def test_anchor_word_boundary_invert_node
      assert_parses(AnchorWordBoundaryInvertNode, "\\B")
    end

    def test_any_node
      assert_parses(AnyNode, ".")
    end

    def test_backref_node
      assert_parses(BackrefNode, "\\1")
    end

    def test_call_node
      assert_parses(CallNode, "\\g<name>", &:node)
    end

    def test_cclass_node
      assert_parses(CClassNode, "[a-z]")
    end

    def test_cclass_invert_node
      assert_parses(CClassInvertNode, "[^a-z]")
    end

    def test_enclose_absent
      assert_parses(EncloseAbsentNode, "(?~a)")
    end

    def test_enclose_condition_node
      assert_parses(EncloseConditionNode, "(?(1)a)")
    end

    def test_enclose_memory_node
      assert_parses(EncloseMemoryNode, "(a)")
    end

    def test_enclose_options_node
      assert_parses(EncloseOptionsNode, "(?i)")
    end

    def test_enclose_stop_backtrack_node
      assert_parses(EncloseStopBacktrackNode, "(?>a)")
    end

    def test_list_node
      assert_parses(ListNode, "a.b")
    end

    def test_look_ahead_node
      assert_parses(LookAheadNode, "(?=a)")
    end

    def test_look_ahead_invert_node
      assert_parses(LookAheadInvertNode, "(?!a)")
    end

    def test_look_behind_node
      assert_parses(LookBehindNode, "(?<=a)")
    end

    def test_look_behind_invert_node
      assert_parses(LookBehindInvertNode, "(?<!a)")
    end

    def test_quantifier_node
      assert_parses(QuantifierNode, "a*")
    end

    def test_string_node
      assert_parses(StringNode, "abc")
    end

    def test_word_node
      assert_parses(WordNode, "\\w")
    end

    def test_word_invert_node
      assert_parses(WordInvertNode, "\\W")
    end

    def test_failure
      assert_raise(ArgumentError) { Onigmo.parse("(?<>)") }
    end

    def test_abstract
      assert_raise(NoMethodError) { Node.send(:new).accept(nil) }
    end

    private

    def assert_parses(kind, source)
      node = Onigmo.parse(source)
      node = yield node if block_given?
      assert_kind_of(kind, node)

      assert_kind_of(String, node.to_json)
      assert_kind_of(String, PP.pp(node, +""))
      assert_kind_of(Hash, node.deconstruct_keys(nil))
    end
  end
end
