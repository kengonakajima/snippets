require "bindata"
require "nbt_utils"
require "zlib"

def getU32(data,ofs)
  b0 = data.bytes[ofs+0]
  b1 = data.bytes[ofs+1]
  b2 = data.bytes[ofs+2]
  b3 = data.bytes[ofs+3]
  return (b0*65536*256)+(b1*65536)+b2*256+b3
end


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

  real_size = getU32(data,byte_ofs)
  comp_type = data.bytes[byte_ofs+4]


  return byte_ofs + 5, size_bytes, real_size, comp_type
end



#
#
#

path = "r.0.0.mca"

data = File.open(path).read
#print "size:", data.bytes.size, " top:" + data.bytes[8192..8210].join(","), "\n"
ofs,bsz_unit,realsize,comp_type = findChunkData(data,0,0)
print "Chunk 0,0 ofs: at #{ofs},  disk:#{bsz_unit}bytes, datasize:#{realsize}, comptype:#{comp_type}\n"

f = File.open(path,"rb")
f.seek(ofs, IO::SEEK_SET)
chunkdata = f.read(realsize)
f.close

inflated = Zlib::Inflate.inflate(chunkdata)

sio = StringIO.new(inflated)

print sio, "\n"
last_byte = sio.read(1).bytes.first
klass = NBTUtils::Tag.tag_type_to_class(last_byte)
top = klass.new(sio,true)

#print top,"\n"

level = top.payload[0], "\n"
data_version = top.payload[1]

print "data_version:", data_version, "\n"

#print level[0].payload, "\n"
heightmap = level[0].payload[2].payload, "\n"
print heightmap, "\n"
print level[0], "\n"

