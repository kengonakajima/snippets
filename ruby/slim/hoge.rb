require "rubygems"
require "slim"

class Env
  attr_accessor :name
end


env = Env.new

env.name = "hogehogehogheoge layout"

tsrc = File.open( "t.slim", "rb" ).read

csrc = File.open( "contents.slim", "rb" ).read

l = Slim::Template.new { tsrc }

c = Slim::Template.new { csrc }.render(env)

puts l.render{c}
