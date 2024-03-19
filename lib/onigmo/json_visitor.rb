# frozen_string_literal: true

require "json"

module Onigmo
  class JSONVisitor < DeconstructVisitor
    def visit_alternation_node(node)
      super.merge!(type: "alternation")
    end

    def visit_anchor_buffer_begin_node(node)
      super.merge!(type: "anchorBufferBegin")
    end

    def visit_anchor_buffer_end_node(node)
      super.merge!(type: "anchorBufferEnd")
    end

    def visit_anchor_keep_node(node)
      super.merge!(type: "anchorKeep")
    end

    def visit_anchor_line_begin_node(node)
      super.merge!(type: "anchorLineBegin")
    end

    def visit_anchor_line_end_node(node)
      super.merge!(type: "anchorLineEnd")
    end

    def visit_anchor_position_begin_node(node)
      super.merge!(type: "anchorPositionBegin")
    end

    def visit_anchor_semi_end_node(node)
      super.merge!(type: "anchorSemiEnd")
    end

    def visit_anchor_word_boundary_node(node)
      super.merge!(type: "anchorWordBoundary")
    end

    def visit_anchor_word_boundary_invert_node(node)
      super.merge!(type: "anchorWordBoundaryInvert")
    end

    def visit_any_node(node)
      super.merge!(type: "any")
    end

    def visit_backref_node(node)
      super.merge!(type: "backref")
    end

    def visit_call_node(node)
      super.merge!(type: "call")
    end

    def visit_cclass_node(node)
      super.merge!(type: "cclass")
    end

    def visit_cclass_invert_node(node)
      super.merge!(type: "cclassInvert")
    end

    def visit_enclose_absent_node(node)
      super.merge!(type: "encloseAbsent")
    end

    def visit_enclose_condition_node(node)
      super.merge!(type: "encloseCondition")
    end

    def visit_enclose_memory_node(node)
      super.merge!(type: "encloseMemory")
    end

    def visit_enclose_options_node(node)
      super.merge!(type: "encloseOptions")
    end

    def visit_enclose_stop_backtrack_node(node)
      super.merge!(type: "encloseStopBacktrack")
    end

    def visit_list_node(node)
      super.merge!(type: "list")
    end

    def visit_look_ahead_invert_node(node)
      super.merge!(type: "lookAheadInvert")
    end

    def visit_look_ahead_node(node)
      super.merge!(type: "lookAhead")
    end

    def visit_look_behind_invert_node(node)
      super.merge!(type: "lookBehindInvert")
    end

    def visit_look_behind_node(node)
      super.merge!(type: "lookBehind")
    end

    def visit_quantifier_node(node)
      super.merge!(type: "quantifier")
    end

    def visit_string_node(node)
      super.merge!(type: "string")
    end

    def visit_word_node(node)
      super.merge!(type: "word")
    end

    def visit_word_invert_node(node)
      super.merge!(type: "wordInvert")
    end
  end
end
