require "../rumino/rumino"

s=readFile("../rumino/rumino.rb")

curclass=nil

s.split("\n").each do |line|

  if line =~ /^require\s\"(.*)\"/ then
    p "REQUIRE:",$1
  end
  if line =~ /(^|\s)def\s+([a-zA-Z0-9_]+)/ then
    p "DEF:",$2
  end
  if line =~ /(^|\s)class\s+([a-zA-Z0-9_]+)/ then
    p "CLASS:",$2
    curclass = 
  end
#  if line =~ /\sclass/
#  print "orig:",line,"\n"
end
