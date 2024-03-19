# Onigmo

A Ruby wrapper around the onigmo regular expression library.

> [!WARNING]  
> This library purposefully accesses internal headers of the onigmo library, and as such no compatibility is guaranteed. This library should be used only for debugging, testing, and educational purposes.

## Installation

Install the gem and add to the application's Gemfile by executing:

    $ bundle add onigmo

If bundler is not being used to manage dependencies, install the gem by executing:

    $ gem install onigmo

## Usage

The `Onigmo` module provides two methods: `parse` and `compile`.

### parse

`Onigmo.parse(source)` gives you back the abstract syntax tree (AST) of the regular expression.

```
irb(main):001> Onigmo.parse("aaa|bbb*")
=> 
alternation(
  string("aaa"),
  list(
    string("bb"),
    quantifier(
      lower: 0,
      upper: nil,
      greedy: true,
      string("b")
    )
  )
)
```

These nodes each have their own APIs for their respective fields. They also share the following common APIs:

* `deconstruct_keys(keys)` - an implementation of pattern matching
* `pretty_print(q)` - an implementation of pretty printing
* `as_json` - returns a hash suitable for serialization
* `to_json` - returns a JSON string suitable for serialization

### compile

`Onigmo.compile(source)` gives you back the list of bytecode instructions that onigmo will use to execute the regular expression.

```
irb(main):001> Onigmo.compile("aaa|bbb*")
# =>
[[:push, 9],
 [:exact3, "aaa"],
 [:jump, 15],
 [:exact2, "bb"],
 [:push, 7],
 [:exact1, "b"],
 [:jump, -12],
 [:end]]
```

Every instruction in the list will be an array. The operands to the instructions will be simple values (i.e., strings, symbols, integers, or arrays).

### visit

With the abstract syntax tree, the `Onigmo` module also provides some ability to visit the nodes in the tree. This is done through visitors. A visitor is an object that responds to one or more `visit_*` methods corresponding to the names of the nodes in the tree. For example, if you wanted to visit only the strings in the tree, you could write:

```ruby
class StringVisitor < Visitor
  attr_reader :strings

  def initialize(strings)
    @strings = strings
  end

  def visit_string_node(node)
    strings << node.value
  end
end
```

By implementing more `visit_*` methods, you can have access to more nodes in the tree. The default behavior is to walk every node in the tree. You can modify this behavior depending on how you implement the visitor node. If you want to stop visiting (like in the example above) you do nothing. If you want to visit specific child nodes, you would call `visit(child_node)`. If you want to visit all nodes (the default behavior) then you can simply call `super`.

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake test-unit` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/Shopify/onigmo.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
