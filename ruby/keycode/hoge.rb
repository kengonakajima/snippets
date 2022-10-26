h={}
File.open("x").read.split("\n").each do |line|
  kc = line.split(".")[0].to_i
  print kc, "\n"
  print "conflict: '#{kc}' against: '#{h[kc]}'\n" if(h[kc]!=nil) 
  h[kc]=kc
  
end
