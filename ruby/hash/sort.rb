h = { "a" => 20, "b" => 30, "c" => 10  }
h.sort {|a,b| a[1]<=>b[1]}.each do |k,v|
  print k,",",v,"\n"
end
   #=> [["c", 10], ["a", 20], ["b", 30]]
