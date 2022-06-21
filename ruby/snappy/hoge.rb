require "snappy"
data=File.open("text","r").read()
compressed=Snappy.deflate(data)
File.open("deflated","w").write(compressed)
decompressed=Snappy.inflate(compressed)
File.open("inflated","w").write(decompressed)

