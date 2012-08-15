--

require( "lumino" )

print("start test")

l = 2500
t = {}
for i=0,l do
  insert(t,i%256)
end
s = string.char( unpack(t) )

d = MOAIDataBuffer.new()
print(d)

d:setString(s)
assert( d:getSize() == (l+1) )

gs = d:getString()
print("s-len:",#gs)
assert(#gs==(l+1))
assert(gs==s)


d:base64Encode()
gs = d:getString()
print("base64:", gs )

d:base64Decode()
gs = d:getString()
assert(#gs==(l+1))
assert(gs==s)


d:deflate()
print( "deflen:", d:getSize())

d:inflate()
gs = d:getString()
assert( d:getSize() == (l+1))
assert( #gs == (l+1))
assert( gs==s)

d:load( "/Users/ringo/bin/moai" )
print("moailen:", d:getSize() )
d:deflate()
print("moaideflate:", d:getSize() )
d:inflate()
print("moaiinflate:", d:getSize() )