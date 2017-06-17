require "bindata"



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

def getU32(data,ofs)
  b0 = data.bytes[ofs+0]
  b1 = data.bytes[ofs+1]
  b2 = data.bytes[ofs+2]
  b3 = data.bytes[ofs+3]
  return (b0*65536*256)+(b1*65536)+b2*256+b3
end

def readChunk(data,ofs,bsz)
  # 4+1bytes header
  chunksz = getU32(data,ofs)
  comptype = data.bytes[ofs+4]

  print "readChunk: chunksz: #{chunksz} comp:#{comptype}\n"
end


#
#
#


data = File.open("r.0.0.mca").read
print "size:", data.bytes.size, "\n"
ofs,bsz = findChunkData(data,0,0)
print "Chunk 0,0 ofs: at #{ofs},  #{bsz} bytes\n"



readChunk(data,ofs,bsz)


