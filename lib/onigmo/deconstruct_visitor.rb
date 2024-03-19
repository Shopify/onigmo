# frozen_string_literal: true

module Onigmo
  class DeconstructVisitor < Visitor
    def visit_alternation_node(node)
      { nodes: visit_all(node.nodes) }
    end

    def visit_anchor_buffer_begin_node(node)
      {}
    end

    def visit_anchor_buffer_end_node(node)
      {}
    end

    def visit_anchor_keep_node(node)
      {}
    end

    def visit_anchor_line_begin_node(node)
      {}
    end

    def visit_anchor_line_end_node(node)
      {}
    end

    def visit_anchor_position_begin_node(node)
      {}
    end

    def visit_anchor_semi_end_node(node)
      {}
    end

    def visit_anchor_word_boundary_node(node)
      {}
    end

    def visit_anchor_word_boundary_invert_node(node)
      {}
    end

    def visit_any_node(node)
      {}
    end

    def visit_backref_node(node)
      { values: node.values }
    end

    def visit_call_node(node)
      { number: node.number, name: node.name }
    end

    def visit_cclass_node(node)
      { values: node.values }
    end

    def visit_cclass_invert_node(node)
      { values: node.values }
    end

    def visit_enclose_absent_node(node)
      { node: visit(node.node) }
    end

    def visit_enclose_condition_node(node)
      { number: node.number, node: visit(node.node) }
    end

    def visit_enclose_memory_node(node)
      { number: node.number, node: visit(node.node) }
    end

    def visit_enclose_options_node(node)
      { options: node.options, node: visit(node.node) }
    end

    def visit_enclose_stop_backtrack_node(node)
      { node: visit(node.node) }
    end

    def visit_list_node(node)
      { nodes: visit_all(node.nodes) }
    end

    def visit_look_ahead_invert_node(node)
      { node: visit(node.node) }
    end

    def visit_look_ahead_node(node)
      { node: visit(node.node) }
    end

    def visit_look_behind_invert_node(node)
      { node: visit(node.node) }
    end

    def visit_look_behind_node(node)
      { node: visit(node.node) }
    end

    def visit_quantifier_node(node)
      { lower: node.lower, upper: node.upper, greedy: node.greedy, node: visit(node.node) }
    end

    def visit_string_node(node)
      { value: node.value }
    end

    def visit_word_node(node)
      {}
    end

    def visit_word_invert_node(node)
      {}
    end
  end
end
