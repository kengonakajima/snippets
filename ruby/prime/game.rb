require "prime"
require "pp"

counts={}
maxn=100

10000.times do 
  i=(maxn*rand()).to_i
  next if i<=1
  primes = i.prime_division
  primes.each do |pair|
    prime = pair[0]
    num = pair[1]
    if counts[prime]==nil then 
      counts[prime]=0
    end
    counts[prime]+=num
  end
end
ary = counts.keys.sort 

print "prime num:", ary.size, "\n"
ary.each do |k|
  print k, " ", counts[k], "\n"
end

