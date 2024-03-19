# frozen_string_literal: true

module Onigmo
  # The base class for all nodes in the tree.
  class Node
    def accept(visitor)
      raise NoMethodError, __method__
    end

    def deconstruct_keys(keys)
      accept(DeconstructVisitor.new)
    end

    def pretty_print(q)
      accept(PrettyPrintVisitor.new(q))
      q.flush
    end

    def as_json
      accept(JSONVisitor.new)
    end

    def to_json(*opts)
      as_json.to_json(*opts)
    end

    private_class_method :new
  end

  # foo|bar
  # ^^^^^^^
  class AlternationNode < Node
    attr_reader :nodes

    def initialize(nodes)
      @nodes = nodes
    end
  end

  # \A
  # ^^
  class AnchorBufferBeginNode < Node
  end

  # \z
  # ^^
  class AnchorBufferEndNode < Node
  end

  # \K
  # ^^
  class AnchorKeepNode < Node
  end

  # ^
  # ^
  class AnchorLineBeginNode < Node
  end

  # $
  # ^
  class AnchorLineEndNode < Node
  end

  # \G
  # ^^
  class AnchorPositionBeginNode < Node
  end

  # \Z
  # ^^
  class AnchorSemiEndNode < Node
  end

  # \b
  # ^^
  class AnchorWordBoundaryNode < Node
  end

  # \B
  # ^^
  class AnchorWordBoundaryInvertNode < Node
  end

  # .
  # ^
  class AnyNode < Node
  end

  # \k<name>
  # ^^^^^^^^
  class BackrefNode < Node
    attr_reader :values

    def initialize(values)
      @values = values
    end
  end

  # \g<name>
  # ^^^^^^^^
  class CallNode < Node
    attr_reader :number, :name

    def initialize(number, name)
      @number = number
      @name = name
    end
  end

  # [a-z]
  # ^^^^^
  class CClassNode < Node
    attr_reader :values

    def initialize(values)
      @values = values
    end
  end

  # [^a-z]
  # ^^^^^^
  class CClassInvertNode < Node
    attr_reader :values

    def initialize(values)
      @values = values
    end
  end

  # (?~subexp)
  # ^^^^^^^^^^
  class EncloseAbsentNode < Node
    attr_reader :node

    def initialize(node)
      @node = node
    end
  end

  # (?(cond)subexp)
  # ^^^^^^^^^^^^^^^
  class EncloseConditionNode < Node
    attr_reader :number, :node

    def initialize(number, node)
      @number = number
      @node = node
    end
  end

  # ()
  # ^^
  class EncloseMemoryNode < Node
    attr_reader :number, :node

    def initialize(number, node)
      @number = number
      @node = node
    end
  end

  # (?options:subexp)
  # ^^^^^^^^^^^^^^^^^
  class EncloseOptionsNode < Node
    attr_reader :options, :node

    def initialize(options, node)
      @options = options
      @node = node
    end
  end

  # (?>subexp)
  # ^^^^^^^^^^
  class EncloseStopBacktrackNode < Node
    attr_reader :node

    def initialize(node)
      @node = node
    end
  end

  # a.b
  # ^^^
  class ListNode < Node
    attr_reader :nodes

    def initialize(nodes)
      @nodes = nodes
    end
  end

  # (?=subexp)
  # ^^^^^^^^^^
  class LookAheadNode < Node
    attr_reader :node

    def initialize(node)
      @node = node
    end
  end

  # (?!subexp)
  # ^^^^^^^^^^
  class LookAheadInvertNode < Node
    attr_reader :node

    def initialize(node)
      @node = node
    end
  end

  # (?<=subexp)
  # ^^^^^^^^^^
  class LookBehindNode < Node
    attr_reader :node

    def initialize(node)
      @node = node
    end
  end

  # (?<!subexp)
  # ^^^^^^^^^^^
  class LookBehindInvertNode < Node
    attr_reader :node

    def initialize(node)
      @node = node
    end
  end

  # a{1,2}
  #  ^^^^^
  class QuantifierNode < Node
    attr_reader :lower, :upper, :greedy, :node

    def initialize(lower, upper, greedy, node)
      @lower = lower
      @upper = upper
      @greedy = greedy
      @node = node
    end
  end

  # abc
  # ^^^
  class StringNode < Node
    attr_reader :value

    def initialize(value)
      @value = value
    end
  end

  # \w
  # ^^
  class WordNode < Node
  end

  # \W
  # ^^
  class WordInvertNode < Node
  end
end
