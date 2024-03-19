# frozen_string_literal: true

module Onigmo
  class AlternationNode
    def accept(visitor)
      visitor.visit_alternation_node(self)
    end

    def child_nodes
      [*nodes]
    end
  end

  class AnchorBufferBeginNode
    def accept(visitor)
      visitor.visit_anchor_buffer_begin_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorBufferEndNode
    def accept(visitor)
      visitor.visit_anchor_buffer_end_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorKeepNode
    def accept(visitor)
      visitor.visit_anchor_keep_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorLineBeginNode
    def accept(visitor)
      visitor.visit_anchor_line_begin_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorLineEndNode
    def accept(visitor)
      visitor.visit_anchor_line_end_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorPositionBeginNode
    def accept(visitor)
      visitor.visit_anchor_position_begin_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorSemiEndNode
    def accept(visitor)
      visitor.visit_anchor_semi_end_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorWordBoundaryNode
    def accept(visitor)
      visitor.visit_anchor_word_boundary_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnchorWordBoundaryInvertNode
    def accept(visitor)
      visitor.visit_anchor_word_boundary_invert_node(self)
    end

    def child_nodes
      []
    end
  end

  class AnyNode
    def accept(visitor)
      visitor.visit_any_node(self)
    end

    def child_nodes
      []
    end
  end

  class BackrefNode
    def accept(visitor)
      visitor.visit_backref_node(self)
    end

    def child_nodes
      []
    end
  end

  class CallNode
    def accept(visitor)
      visitor.visit_call_node(self)
    end

    def child_nodes
      []
    end
  end

  class CClassNode
    def accept(visitor)
      visitor.visit_cclass_node(self)
    end

    def child_nodes
      []
    end
  end

  class CClassInvertNode
    def accept(visitor)
      visitor.visit_cclass_invert_node(self)
    end

    def child_nodes
      []
    end
  end

  class EncloseAbsentNode
    def accept(visitor)
      visitor.visit_enclose_absent_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class EncloseConditionNode
    def accept(visitor)
      visitor.visit_enclose_condition_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class EncloseMemoryNode
    def accept(visitor)
      visitor.visit_enclose_memory_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class EncloseOptionsNode
    def accept(visitor)
      visitor.visit_enclose_options_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class EncloseStopBacktrackNode
    def accept(visitor)
      visitor.visit_enclose_stop_backtrack_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class ListNode
    def accept(visitor)
      visitor.visit_list_node(self)
    end

    def child_nodes
      [*nodes]
    end
  end

  class LookAheadNode
    def accept(visitor)
      visitor.visit_look_ahead_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class LookAheadInvertNode
    def accept(visitor)
      visitor.visit_look_ahead_invert_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class LookBehindNode
    def accept(visitor)
      visitor.visit_look_behind_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class LookBehindInvertNode
    def accept(visitor)
      visitor.visit_look_behind_invert_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class QuantifierNode
    def accept(visitor)
      visitor.visit_quantifier_node(self)
    end

    def child_nodes
      [node]
    end
  end

  class StringNode
    def accept(visitor)
      visitor.visit_string_node(self)
    end

    def child_nodes
      []
    end
  end

  class WordNode
    def accept(visitor)
      visitor.visit_word_node(self)
    end

    def child_nodes
      []
    end
  end

  class WordInvertNode
    def accept(visitor)
      visitor.visit_word_invert_node(self)
    end

    def child_nodes
      []
    end
  end

  class Visitor
    def visit_child_nodes(node)
      node.child_nodes.each { |child_node| child_node.accept(self) }
    end

    alias visit_alternation_node visit_child_nodes
    alias visit_anchor_buffer_begin_node visit_child_nodes
    alias visit_anchor_buffer_end_node visit_child_nodes
    alias visit_anchor_keep_node visit_child_nodes
    alias visit_anchor_line_begin_node visit_child_nodes
    alias visit_anchor_line_end_node visit_child_nodes
    alias visit_anchor_position_begin_node visit_child_nodes
    alias visit_anchor_semi_end_node visit_child_nodes
    alias visit_anchor_word_boundary_node visit_child_nodes
    alias visit_anchor_word_boundary_invert_node visit_child_nodes
    alias visit_any_node visit_child_nodes
    alias visit_backref_node visit_child_nodes
    alias visit_call_node visit_child_nodes
    alias visit_cclass_node visit_child_nodes
    alias visit_cclass_invert_node visit_child_nodes
    alias visit_enclose_absent_node visit_child_nodes
    alias visit_enclose_condition_node visit_child_nodes
    alias visit_enclose_memory_node visit_child_nodes
    alias visit_enclose_options_node visit_child_nodes
    alias visit_enclose_stop_backtrack_node visit_child_nodes
    alias visit_list_node visit_child_nodes
    alias visit_look_ahead_node visit_child_nodes
    alias visit_look_ahead_invert_node visit_child_nodes
    alias visit_look_behind_node visit_child_nodes
    alias visit_look_behind_invert_node visit_child_nodes
    alias visit_quantifier_node visit_child_nodes
    alias visit_string_node visit_child_nodes
    alias visit_word_node visit_child_nodes
    alias visit_word_invert_node visit_child_nodes

    private

    def visit(node)
      node.accept(self)
    end

    def visit_all(nodes)
      nodes.map { |node| visit(node) }
    end
  end
end
