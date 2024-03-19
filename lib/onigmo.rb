# frozen_string_literal: true

module Onigmo
  class AlternationNode
    attr_reader :nodes

    def initialize(nodes)
      @nodes = nodes
    end
  end

  class AnchorNode
    attr_reader :type

    def initialize(type)
      @type = type
    end
  end

  class AnyNode
  end

  class BackrefNode
    attr_reader :values

    def initialize(values)
      @values = values
    end
  end

  class CallNode
    attr_reader :name

    def initialize(name)
      @name = name
    end
  end

  class CClassNode
    attr_reader :inverted, :ranges

    def initialize(inverted, ranges)
      @inverted = inverted
      @ranges = ranges
    end
  end

  class CTypeNode
    attr_reader :type

    def initialize(type)
      @type = type
    end
  end

  class EncloseNode
    attr_reader :type, :node

    def initialize(type, node)
      @type = type
      @node = node
    end
  end

  class ListNode
    attr_reader :nodes

    def initialize(nodes)
      @nodes = nodes
    end
  end

  class QuantifierNode
    attr_reader :lower, :upper, :greedy, :node

    def initialize(lower, upper, greedy, node)
      @lower = lower
      @upper = upper
      @greedy = greedy
      @node = node
    end
  end

  class StringNode
    attr_reader :value

    def initialize(value)
      @value = value
    end
  end
end

require "onigmo/onigmo"
