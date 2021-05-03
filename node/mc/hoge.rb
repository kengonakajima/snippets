require "json"

def to_s16(ary,ofs)
  return ary[ofs]*256+ary[ofs+1]
end
# https://wiki.vg/Protocol#VarInt_and_VarLong
# TODO: support negative value
def to_vui(ary,ofs)
  b0=ary[ofs]
  return [b0,1] if b0 <128
  b0&=0x7f
  b1=ary[ofs+1]
  return [b0+(b1<<7),2] if b1<128
  b1&=0x7f
  b2=ary[ofs+2]
  return [b0+(b1<<7)+(b2<<14),3] if b2<128
  b2&=0x7f  
  b3=ary[ofs+3]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21),4] if b3<128
  b3&=0x7f  
  b4=ary[ofs+4]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28),5]
  throw "error"
end
def to_vul(ary,ofs)
  b0=ary[ofs]
  return [b0,1] if b0 <128
  b0&=0x7f
  b1=ary[ofs+1]
  return [b0+(b1<<7),2] if b1<128
  b1&=0x7f
  b2=ary[ofs+2]
  return [b0+(b1<<7)+(b2<<14),3] if b2<128
  b2&=0x7f  
  b3=ary[ofs+3]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21),4] if b3<128
  b3&=0x7f  
  b4=ary[ofs+4]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28),5]
  b4&=0x7f
  b5=ary[ofs+5]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28)+(b5<<35),6]
  b5&=0x7f
  b6=ary[ofs+6]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28)+(b5<<35)+(b6<<42),7]
  b6&=0x7f
  b7=ary[ofs+7]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28)+(b5<<35)+(b6<<42)+(b7<<49),8]
  b7&=0x7f
  b8=ary[ofs+8]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28)+(b5<<35)+(b6<<42)+(b7<<49)+(b8<<56),9]
  b8&=0x7f
  b9=ary[ofs+9]
  return [b0+(b1<<7)+(b2<<14)+(b3<<21)+(b4<<28)+(b5<<35)+(b6<<42)+(b7<<49)+(b8<<56)+(b9<<63),10]
  throw "error"
end

def bm2ary(bm)
  out=[0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0]
  out[0]=1 if bm & 1 == 1
  out[1]=1 if bm & 2 == 2
  out[2]=1 if bm & 4 == 4
  out[3]=1 if bm & 8 == 8
  out[4]=1 if bm & 16 == 16
  out[5]=1 if bm & 32 == 32
  out[6]=1 if bm & 64 == 64
  out[7]=1 if bm & 128 == 128
  out[8]=1 if bm & 256 == 256
  out[9]=1 if bm & 512 == 512
  out[10]=1 if bm & 1024 == 1024
  out[11]=1 if bm & 2048 == 2048
  out[12]=1 if bm & 4096 == 4096
  out[13]=1 if bm & 8192 == 8192
  out[14]=1 if bm & 16384 == 16384
  out[15]=1 if bm & 32768 == 32768
  return out
end

#
#
#

a=JSON.parse(File.open("pkt.txt").read())

print "x:", a["x"], " z:", a["z"], " gu:", a["groundUp"], "\n"
bitmap = a["bitMap"]
bit_ary = bm2ary(bitmap)
print "bit_ary:", bit_ary.join(","), "\n"
data=a["chunkData"]["data"]
print "ary size:",data.size,"\n"

# [5, 234,   4,  14,  [ 0,  33,  1, 2, vi(174, 30), 6, vi(154, 26), 70, vi(186, 61), 14, vi(243, 15), vi(255, 15), vi(226, 65), vi(157, 65), vi(128, 2), 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 1

# java -cp server_1.16.5.jar net.minecraft.data.Main --all

blocks=JSON.parse(File.open("blocks_1.16.5.json").read())
$id2key={}
blocks.keys.each do |key|
  blocks[key]["states"].each do |state|
    $id2key[state["id"]]=key
  end
end

def readChunkArray(data)
  ofs=0
  block_num=to_s16(data,ofs)
  print "#{ofs} block num:",block_num,"\n"
  ofs+=2

  bit_depth=data[ofs]
  print "#{ofs} bit_depth:",bit_depth,"\n"
  ofs+=1
  
  palette_len,inc = to_vui(data,ofs)
  print "#{ofs} palette_len:",palette_len,", inc:",inc,"\n"
  ofs+=inc

  palette_len.times do |i|
    ui,inc=to_vui(data,ofs)
    print "#{ofs} palette[#{i}]:",ui," key:",$id2key[ui], ", inc:",inc,"\n"
    ofs+=inc
  end

  data_array_len,inc = to_vui(data,ofs)
  print "#{ofs} data_array_len:",data_array_len,", inc:",inc,"\n"
  ofs+=inc
  
  data_array_len.times do |i|
    ul,inc=to_vul(data,ofs)
    print "#{ofs} ul:",ul,"\n"
    ofs+=inc
  end

  print "ofs:",ofs, " ", data[ofs],"\n"
end

readChunkArray(data)
  
#print data


