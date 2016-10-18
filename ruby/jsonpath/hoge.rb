require "rubygems"
require "jsonpath"

j = File.open("aho.json").read

path = JsonPath.new( "$..hat_id")

print path.on(j)

