require 'rubygems'
require 'httpclient'
require 'kconv'
hc = HTTPClient.new


data = {"page"=>0, "per_page"=>10 }
res = hc.post("http://scored.io/post", data )


print res.content, "\n"

print res.header,"\n"
