require "json"

def to_s16(ary,ofs)
  return ary[ofs]*256+ary[ofs+1]
end
# buf to varint
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
# buf to varlong
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

def to_l(ary,ofs)
  return ary[ofs+7]+(ary[ofs+6]<<8)+(ary[ofs+5]<<16)+(ary[ofs+4]<<24)+(ary[ofs+3]<<32)+(ary[ofs+2]<<40)+(ary[ofs+1]<<48)+(ary[ofs]<<56)
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


# long 
def i32_to_bitary(v)
  out=[]
  32.times do |i|
    out[31-i]=v&1
    v=v>>1
  end
  return out
end

def bitary_to_i(ba)
  out=0
  v=1
  ba.size.times do |i|
    out+=v if ba[ba.size-1-i]==1 
    v=v<<1
  end
  return out
end

# [25215072,806882310],[25215072,806882310],[25215072,806882310], ...
#
# 9 bits * 256 = 2304 bits,  36 longs
# irb(main):002:0> printf("%b",25215072)
# 1100000001100000001100000
# 110000000 110000000 1100000
# 000011000 000011000 000011000

# [[25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215328,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215073,-2145907706],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [25215072,806882310],[25215072,806882310],[25215072,806882310],[25215072,806882310],
#  [0,806882310]]

#
# 19200, 11488
# {"x":1200,"z":718,"groundUp":true,"bitMap":15,"heightmaps":{"type":"compound","name":"","value":{"MOTION_BLOCKING":{"type":"longArray","value":[[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[1,-117670337]]},"WORLD_SURFACE":{"type":"longArray","value":[[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[264758257,-117670337],[1,-117670337]]}}}

# MSB - LSB
# 0 aaaaaaaaa bbbbbbbbb ccccccccc ddddddddd eeeeeeeee fffffffff ggggggggg hhhhhhhhh iiiiiiiii
# 
def la2hm(la)
  outhm=[]
  ba=[]
  la.each do |pair|
    ba0=i32_to_bitary(pair[0])
    ba1=i32_to_bitary(pair[1])
    ba63=ba0[1..-1]+ba1 # 9*7
    slices=[]
    ba63.each_slice(9) do |slice|
      slices.push(slice)
    end
    slices.reverse.each do |slice|
      outhm.push(bitary_to_i(slice))
    end
  end
  return outhm
end

# ul:uint64_t
def ul2blocks(ul,bit_depth)
  
  if bit_depth==4 then
    # 1ULに16個入ってる
    s0=(ul>>48)&0xffff
    s1=(ul>>32)&0xffff
    s2=(ul>>16)&0xffff
    s3=ul&0xffff
    return [
      (s0>>12)&0xf,(s0>>8)&0xf,(s0>>4)&0xf,s0&0xf,
      (s1>>12)&0xf,(s1>>8)&0xf,(s1>>4)&0xf,s1&0xf,
      (s2>>12)&0xf,(s2>>8)&0xf,(s2>>4)&0xf,s2&0xf,
      (s3>>12)&0xf,(s3>>8)&0xf,(s3>>4)&0xf,s3&0xf
    ]
  elsif bit_depth==5 then
    # 1ULに12個はいっててMSBが0000
    return [
      (ul>>55)&0x1f,(ul>>50)&0x1f,(ul>>45)&0x1f,(ul>>40)&0x1f,(ul>>35)&0x1f,
      (ul>>30)&0x1f,(ul>>25)&0x1f,(ul>>20)&0x1f,(ul>>15)&0x1f,(ul>>10)&0x1f,
      (ul>>5)&0x1f,(ul&0x1f)
    ]
  elsif bit_depth==6 then
    # 1ULに10個 MSBは0000
    return [
      (ul>>54)&0x3f,(ul>>48)&0x3f,(ul>>42)&0x3f,(ul>>36)&0x3f,(ul>>30)&0x3f,
      (ul>>24)&0x3f,(ul>>18)&0x3f,(ul>>12)&0x3f,(ul>>6)&0x3f,ul&0x3f
    ]
  elsif bit_depth==7 then
    # 1ULに9個 MSBは0
    return [
      (ul>>56)&0x7f,(ul>>49)&0x7f,(ul>>42)&0x7f,(ul>>35)&0x7f,(ul>>28)&0x7f,
      (ul>>21)&0x7f,(ul>>14)&0x7f,(ul>>7)&0x7f,ul&0x7f
    ]
  elsif bit_depth==8 then
    # 1ULに8個
    return [
      (ul>>56)&0xff,(ul>>48)&0xff,(ul>>40)&0xff,(ul>>32)&0xff,(ul>>24)&0xff,
      (ul>>16)&0xff,(ul>>8)&0xff,ul&0xff
    ]
  elsif bit_depth==9 then
    # 1ULに7個
    return [
      (ul>>54)&0x1ff,(ul>>45)&0x1ff,(ul>>36)&0x1ff,(ul>>27)&0x1ff,(ul>>18)&0x1ff,
      (ul>>9)&0x1ff,ul&0x1ff
    ]
  elsif bit_depth==10 then
    # 1ULに6個, MSBは0000
    return [
      (ul>>50)&0x3ff,(ul>>40)&0x3ff,(ul>>30)&0x3ff,(ul>>20)&0x3ff,(ul>>10)&0x3ff,
      ul&0x3ff
    ]
  else
    throw "bit depth not supported"
  end

end

#
#
#

a=JSON.parse(File.open("pkt6.txt").read())

print "x:", a["x"], " z:", a["z"], " gu:", a["groundUp"], "\n"
bitmap = a["bitMap"]
bit_ary = bm2ary(bitmap)
section_num = bit_ary.count(1)
print "bit_ary:", bit_ary.join(",")," section_num:",section_num,  "\n"
outhm=la2hm(a["heightmaps"]["value"]["MOTION_BLOCKING"]["value"])
outhm.each_slice(16) do |row| print "hm:", row, "\n" end

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

def readChunkArray(data,secnum)
  ofs=0

  secnum.times do |si|
    print "reading section #{si}\n"

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

    # 4bitのときは256 (64x256/4=4096)
    # 5bitのときは342 (64x342/4=5472)だいぶオーバーしてる。 https://wiki.vg/Protocolによると頭の4ビットは0だと。
    # 60ビットだけが使われるので、  long1個あたりでは 12ブロック。よって、342*12=4104.
    # 6bitのときは410. long1個あたりでは10ブロック。よって4100
    # ルールは一応わかったかな。
    data_array_len_long,inc = to_vui(data,ofs) 
    print "#{ofs} data_array_len_long:",data_array_len_long,", inc:",inc,"\n"
    ofs+=inc

    blocks=[]
    data_array_len_long.times do |i|
      ul=to_l(data,ofs)  # sprintf("%016x",ul)
      blocks+=ul2blocks(ul,bit_depth)
      ofs+=8
    end
    blocks=blocks.slice(0,4096)
    print "#{ofs} blocks:", blocks.size, " ", blocks, "\n"
  end
  print "final ofs:",ofs, "\n"
  throw "bug!!" if ofs != data.size
end

readChunkArray(data,section_num)
  
#print data


