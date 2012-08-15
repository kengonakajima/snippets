--

path = "dir/dir2/sol.png"

b = MOAIDataBuffer.new()
b:load(path)

print("png len:", b:getSize() )

sha1 = crypto.evp.new("sha1")

cs = sha1:digest( b:getString() )
print("sha1:", cs )
assert( cs == "5503856fe2d30d95332a7e4be12ad53d07865fa4" ) -- shasum in osx 

print("test ok")
