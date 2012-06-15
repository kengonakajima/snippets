require "rubygems"
require 'ruby_parser'
require "../rumino/rumino"

@processor = RubyParser.new

s=readFile("./corrupt.rb")

print @processor.parse( s)
