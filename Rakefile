# frozen_string_literal: true

require "bundler/gem_tasks"
require "rake/extensiontask"
require "rake/testtask"

onigmo_headers = [
  "onigmo.h", # Onigmo API header file (public)
  "onigmognu.h", # GNU regex API header file (public)
  "onigmoposix.h", # POSIX API header file (public)
  "regenc.h", # character encodings framework header file
  "regint.h", # internal definitions
  "regparse.h", # internal definitions for regparse.c and regcomp.c
  "st.h", # hash table functions header file
  "enc/iso_8859.h", # common definition of ISO-8859 encoding
  "enc/jis/props.h", # JIS properties data
  "enc/shift_jis.h", # Common part of Shift_JIS and Windows-31J encoding
  "enc/unicode/casefold.h", # Unicode case folding data
  "enc/unicode/name2ctype.h" # Unicode properties data
]

onigmo_sources = [
  "regcomp.c", # compiling and optimization functions
  "regenc.c", # character encodings framework
  "regerror.c", # error message function
  "regexec.c", # search and match functions
  "regext.c", # extended API functions (deluxe version API)
  "reggnu.c", # GNU regex API functions
  "regparse.c", # parsing functions.
  "regposerr.c", # POSIX error message function
  "regposix.c", # POSIX API functions
  "regsyntax.c", # pattern syntax functions and built-in syntax definition
  "regtrav.c", # capture history tree data traverse functions
  "regversion.c", # version info function
  "st.c", # hash table functions
  "enc/ascii.c", # ASCII-8BIT encoding
  "enc/big5.c", # Big5 encoding
  "enc/cp949.c", # CP949 encoding (only used in Ruby)
  "enc/emacs_mule.c", # Emacs internal encoding (only used in Ruby)
  "enc/euc_jp.c", # EUC-JP encoding
  "enc/euc_kr.c", # EUC-KR, EUC-CN encoding
  "enc/euc_tw.c", # EUC-TW encoding
  "enc/gb18030.c", # GB18030 encoding
  # "enc/gb2312.c", # GB2312 encoding (only used in Ruby)
  "enc/gbk.c", # GBK encoding
  "enc/iso_8859_1.c", # ISO-8859-1 (Latin-1)
  "enc/iso_8859_10.c", # ISO-8859-10 (Latin-6 or Nordic)
  "enc/iso_8859_11.c", # ISO-8859-11 (Thai)
  "enc/iso_8859_13.c", # ISO-8859-13 (Latin-7 or Baltic Rim)
  "enc/iso_8859_14.c", # ISO-8859-14 (Latin-8 or Celtic)
  "enc/iso_8859_15.c", # ISO-8859-15 (Latin-9 or West European with Euro)
  "enc/iso_8859_16.c", # ISO-8859-16 (Latin-10)
  "enc/iso_8859_2.c", # ISO-8859-2 (Latin-2)
  "enc/iso_8859_3.c", # ISO-8859-3 (Latin-3)
  "enc/iso_8859_4.c", # ISO-8859-4 (Latin-4)
  "enc/iso_8859_5.c", # ISO-8859-5 (Cyrillic)
  "enc/iso_8859_6.c", # ISO-8859-6 (Arabic)
  "enc/iso_8859_7.c", # ISO-8859-7 (Greek)
  "enc/iso_8859_8.c", # ISO-8859-8 (Hebrew)
  "enc/iso_8859_9.c", # ISO-8859-9 (Latin-5 or Turkish)
  "enc/koi8_r.c", # KOI8-R encoding
  "enc/koi8_u.c", # KOI8-U encoding
  # "enc/mktable.c", # character type table generator
  "enc/shift_jis.c", # Shift_JIS encoding
  "enc/unicode.c", # common codes of Unicode encoding
  "enc/us_ascii.c", # US-ASCII encoding (only used in Ruby)
  "enc/utf_16be.c", # UTF-16BE encoding
  "enc/utf_16le.c", # UTF-16LE encoding
  "enc/utf_32be.c", # UTF-32BE encoding
  "enc/utf_32le.c", # UTF-32LE encoding
  "enc/utf_8.c", # UTF-8 encoding
  "enc/windows_1250.c", # Windows-1250 (CP1250) encoding (Central/Eastern Europe)
  "enc/windows_1251.c", # Windows-1251 (CP1251) encoding (Cyrillic)
  "enc/windows_1252.c", # Windows-1252 (CP1252) encoding (Latin)
  "enc/windows_1253.c", # Windows-1253 (CP1253) encoding (Greek)
  "enc/windows_1254.c", # Windows-1254 (CP1254) encoding (Turkish)
  "enc/windows_1257.c", # Windows-1257 (CP1257) encoding (Baltic Rim)
  "enc/windows_31j.c" # Windows-31J (CP932) encoding
]

directory("tmp")
directory("tmp/onigmo" => "tmp") do
  sh("git clone --depth 1 https://github.com/k-takata/Onigmo.git tmp/onigmo")
end

directory("ext/onigmo/enc" => "tmp/onigmo") { mkdir_p("ext/onigmo/enc") }
directory("ext/onigmo/enc/jis" => "tmp/onigmo") { mkdir_p("ext/onigmo/enc/jis") }
directory("ext/onigmo/enc/unicode" => "tmp/onigmo") { mkdir_p("ext/onigmo/enc/unicode") }

onigmo_headers.each do |filepath|
  file("tmp/onigmo/#{filepath}" => "tmp/onigmo")

  dependencies = ["tmp/onigmo/#{filepath}"]

  dirpath = File.dirname(filepath)
  dependencies << "ext/onigmo/#{dirpath}" unless dirpath == "."

  file("ext/onigmo/#{filepath}" => dependencies) do
    cp("tmp/onigmo/#{filepath}", "ext/onigmo/#{filepath}")
  end
end

onigmo_sources.each do |filepath|
  destination = filepath.tr("/", "_")

  file("tmp/onigmo/#{filepath}" => "tmp/onigmo")
  file("ext/onigmo/#{destination}" => "tmp/onigmo/#{filepath}") do
    cp("tmp/onigmo/#{filepath}", "ext/onigmo/#{destination}")
  end
end

Rake::ExtensionTask.new(:compile) do |ext|
  ext.name = "onigmo"
  ext.ext_dir = "ext/onigmo"
  ext.lib_dir = "lib/onigmo"
  ext.gem_spec = Gem::Specification.load("onigmo.gemspec")
end

Rake::Task[:compile].prerequisites.prepend(
  *onigmo_headers.map { |filepath| "ext/onigmo/#{filepath}" },
  *onigmo_sources.map { |filepath| "ext/onigmo/#{filepath.tr("/", "_")}" }
)

Rake::TestTask.new(:test) do |t|
  t.libs << "test"
  t.pattern = "test/**/*_test.rb"
  t.verbose = true
end

task default: :test
