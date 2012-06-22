# parse js and find 
require "../rumino/rumino"


require "tokenizer"
require "parser"




path = "t.js"
path ="test/domain.js"
path ="test/http.js"
#path ="test/jquery.js"

src = readFile(path)
lines = src.split("\n")
lines.each do |line|
  if line.size > 1000 then
    raise "line too long\n"
  end
end

t = Narcissus::Tokenizer.new( src, path, 1 )



st = Time.now
while ! t.done
  tk = t.get
#  prt Narcissus::TOKENS[tk], "(",(et-st),")"
end
et = Time.now

p "done(#{et-st})"
