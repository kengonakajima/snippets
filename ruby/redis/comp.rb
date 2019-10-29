require "zlib"
require "base64"
require "base62"
ary=[]
100.times do 
  ary.push(rand(1000000000))
end
data = ary.join(",")
comp = Zlib::Deflate.deflate(data)

ary62 = ary.map do |x| Base62.encode(x) end
s= Base64.encode64 comp
p s,data, ary62.join(",")
p s.size, data.size, ary62.join(",").size
