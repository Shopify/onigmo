# frozen_string_literal: true

Gem::Specification.new do |spec|
  spec.name = "onigmo"
  spec.version = "0.1.0"
  spec.authors = ["Kevin Newton"]
  spec.email = ["kddnewton@gmail.com"]

  spec.summary = "Onigmo::parse"
  spec.homepage = "https://github.com/kddnewton/onigmo"
  spec.license = "MIT"

  spec.files = `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  spec.require_paths = ["lib"]
  spec.extensions = ["ext/onigmo/extconf.rb"]
end
