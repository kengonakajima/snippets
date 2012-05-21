require 'rubygems'
require 'httpclient'
require 'kconv'
hc = HTTPClient.new


res = hc.get("https://api.github.com/users/kengonakajima/gists" )



print res.header["X-RateLimit-Remaining"],"\n"

#print res.content,"\n"


res = hc.get("https://api.github.com/users/kengonakajima/gists?access_token=00a2972d66a500700612f65c0fc593b2261f03" ) # corrupt, renew it

print res.header["X-RateLimit-Remaining"],"\n"
