require "bindata"

data = File.open("r.0.0.mca").read

print "size:", data.bytes.size, "\n"

def findChunkData(data,chx,chz)
  index_in_loctable = (chz%32)*32+(chx%32)
  ofs = index_in_loctable*4

  b0 = data.bytes[ofs+0]
  b1 = data.bytes[ofs+1]
  b2 = data.bytes[ofs+2]
  b3 = data.bytes[ofs+3]
  unit = 4096
  byte_ofs = (b0 * 65536 + b1 * 256 + b2) * unit
  size_bytes = b3 * unit
  return byte_ofs, size_bytes
end

ofs,bsz = findChunkData(data,0,0)

print "Chunk 0,0 ofs: at #{ofs},  #{bsz} bytes\n"

