require "prime"

nprime=0
1000000.times do |i|
  if i.prime? then nprime+=1 end
end

print nprime,"\n"