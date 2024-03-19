# frozen_string_literal: true

module Onigmo
  # foo|bar
  # ^^^^^^^
  class AlternationNode
    attr_reader :nodes

    def initialize(nodes)
      @nodes = nodes
    end
  end

  # \A
  # ^^
  class AnchorNode
    attr_reader :type

    def initialize(type)
      @type = type
    end
  end

  # .
  # ^
  class AnyNode
  end

  # \k<name>
  # ^^^^^^^^
  class BackrefNode
    attr_reader :values

    def initialize(values)
      @values = values
    end
  end

  # \g<name>
  # ^^^^^^^^
  class CallNode
    attr_reader :number, :name

    def initialize(number, name)
      @number = number
      @name = name
    end
  end

  # [[a-z]]
  # ^^^^^^^
  class CClassNode
    attr_reader :inverted, :ranges

    def initialize(inverted, ranges)
      @inverted = inverted
      @ranges = ranges
    end
  end

  # \w
  # ^^
  class CTypeNode
    attr_reader :type

    def initialize(type)
      @type = type
    end
  end

  # ()
  # ^^
  class EncloseNode
    attr_reader :type, :node

    def initialize(type, node)
      @type = type
      @node = node
    end
  end

  # a.b
  # ^^^
  class ListNode
    attr_reader :nodes

    def initialize(nodes)
      @nodes = nodes
    end
  end

  # a{1,2}
  #  ^^^^^
  class QuantifierNode
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
  class StringNode
    attr_reader :value

    def initialize(value)
      @value = value
    end
  end
end

require "onigmo/onigmo"
