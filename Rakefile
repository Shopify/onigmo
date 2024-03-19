# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/extensiontask"
require "rake/testtask"

desc("Create tmp directory")
directory("tmp")

desc("Clone onigmo repository")
directory("tmp/onigmo" => "tmp") do
  sh("git clone --depth 1 https://github.com/k-takata/Onigmo.git tmp/onigmo")
end

onigmo_headers = ["regenc.h", "regint.h", "regparse.h"]
onigmo_headers.each do |filepath|
  desc("Download #{filepath} from onigmo repository")
  file("tmp/onigmo/#{filepath}" => "tmp/onigmo")

  desc("Copy #{filepath} from onigmo repository")
  file("ext/onigmo/#{filepath}" => "tmp/onigmo/#{filepath}") do
    cp("tmp/onigmo/#{filepath}", "ext/onigmo/#{filepath}")
  end
end

Rake::ExtensionTask.new(:compile) do |ext|
  ext.name = "onigmo"
  ext.ext_dir = "ext/onigmo"
  ext.lib_dir = "lib/onigmo"
  ext.gem_spec = Gem::Specification.load("onigmo.gemspec")
end

Rake::Task[:compile].prerequisites.prepend(
  *onigmo_headers.map { |filepath| "ext/onigmo/#{filepath}" }
)

Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.pattern = "test/**/*_test.rb"
  t.verbose = true
end

Rake::Task[:test].enhance([:compile])

task default: :test
