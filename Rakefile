# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/extensiontask"

Rake::ExtensionTask.new(:compile) do |ext|
  ext.name = "onigmo"
  ext.ext_dir = "ext/onigmo"
  ext.lib_dir = "lib/onigmo"
  ext.gem_spec = Gem::Specification.load("onigmo.gemspec")
end
