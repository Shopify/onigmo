# frozen_string_literal: true

require "pp"

module Onigmo
  class PrettyPrintVisitor < Visitor
    attr_reader :q

    def initialize(q)
      @q = q
    end

    def visit_alternation_node(node)
      q.group do
        q.text("alternation(")
        q.nest(2) do
          q.breakable("")
          q.seplist(node.nodes) do |child_node|
            visit(child_node)
          end
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_anchor_buffer_begin_node(node)
      q.text("anchorBufferBegin()")
    end

    def visit_anchor_buffer_end_node(node)
      q.text("anchorBufferEnd()")
    end

    def visit_anchor_keep_node(node)
      q.text("anchorKeep()")
    end

    def visit_anchor_line_begin_node(node)
      q.text("anchorLineBegin()")
    end

    def visit_anchor_line_end_node(node)
      q.text("anchorLineEnd()")
    end

    def visit_anchor_position_begin_node(node)
      q.text("anchorPositionBegin()")
    end

    def visit_anchor_semi_end_node(node)
      q.text("anchorSemiEnd()")
    end

    def visit_anchor_word_boundary_node(node)
      q.text("anchorWordBoundary()")
    end

    def visit_anchor_word_boundary_invert_node(node)
      q.text("anchorWordBoundaryInvert()")
    end

    def visit_any_node(node)
      q.text("any")
    end

    def visit_backref_node(node)
      q.group do
        q.text("backref(")
        q.nest(2) do
          q.breakable("")
          q.seplist(node.values) do |value|
            q.pp(value)
          end
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_call_node(node)
      q.group do
        q.text("call(")
        q.nest(2) do
          q.breakable("")
          q.seplist([node.number, node.name]) do |value|
            q.pp(value)
          end
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_cclass_node(node)
      q.group do
        q.text("cclass(")
        q.nest(2) do
          q.breakable("")
          q.pp(node.ranges)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_cclass_invert_node(node)
      q.group do
        q.text("cclassInvert(")
        q.nest(2) do
          q.breakable("")
          q.pp(node.ranges)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_enclose_absent_node(node)
      q.group do
        q.text("encloseAbsent(")
        q.nest(2) do
          q.breakable("")
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_enclose_condition_node(node)
      q.group do
        q.text("encloseCondition(")
        q.nest(2) do
          q.breakable("")
          q.pp(node.number)
          q.comma_breakable
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_enclose_memory_node(node)
      q.group do
        q.text("encloseMemory(")
        q.nest(2) do
          q.breakable("")
          q.pp(node.number)
          q.comma_breakable
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_enclose_options_node(node)
      q.group do
        q.text("encloseOptions(")
        q.nest(2) do
          q.breakable("")
          q.pp(node.options)
          q.comma_breakable
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_enclose_stop_backtrack_node(node)
      q.group do
        q.text("encloseStopBacktrack(")
        q.nest(2) do
          q.breakable("")
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_list_node(node)
      q.group do
        q.text("list(")
        q.nest(2) do
          q.breakable("")
          q.seplist(node.nodes) do |child_node|
            visit(child_node)
          end
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_look_ahead_node(node)
      q.group do
        q.text("lookAhead(")
        q.nest(2) do
          q.breakable("")
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_look_ahead_invert_node(node)
      q.group do
        q.text("lookAheadInvert(")
        q.nest(2) do
          q.breakable("")
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_look_behind_node(node)
      q.group do
        q.text("lookBehind(")
        q.nest(2) do
          q.breakable("")
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_look_behind_invert_node(node)
      q.group do
        q.text("lookBehindInvert(")
        q.nest(2) do
          q.breakable("")
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_quantifier_node(node)
      q.group do
        q.text("quantifier(")
        q.nest(2) do
          q.breakable("")
          q.seplist({ lower: node.lower, upper: node.upper, greedy: node.greedy }) do |key, value|
            q.text("#{key}: ")
            q.pp(value)
          end
          q.comma_breakable
          visit(node.node)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_string_node(node)
      q.group do
        q.text("string(")
        q.nest(2) do
          q.breakable("")
          q.pp(node.value)
        end
        q.breakable("")
        q.text(")")
      end
    end

    def visit_word_node(node)
      q.text("word()")
    end

    def visit_word_invert_node(node)
      q.text("wordInvert()")
    end
  end
end
