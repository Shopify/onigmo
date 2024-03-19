# frozen_string_literal: true

Gem::Specification.new do |spec|
  spec.name = "onigmo"
  spec.version = "0.1.0"
  spec.authors = ["Shopify"]
  spec.email = ["ruby@shopify.com"]

  spec.summary = "A Ruby wrapper around the onigmo regular expression library"
  spec.homepage = "https://github.com/Shopify/onigmo"
  spec.license = "MIT"

  spec.files = `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  spec.require_paths = ["lib"]
  spec.extensions = ["ext/onigmo/extconf.rb"]
end
