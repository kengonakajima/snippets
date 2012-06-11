def arglog(b)
  b.eval( "local_variables.each do |name| print name, ':',  (eval name), '\t' end")
  print "\n"
end

def hoge(a,b,c)
  print local_variables.join(","),"\n"
  local_variables.each do |name|
    puts eval name
  end
end

def piyo(a,b,c)
  arglog(binding)
end


hoge(1,2,3)

piyo(4,5,6)
