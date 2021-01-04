require "wavefile"
include WaveFile

samples=[]
Reader.new("echoing_quicktime.wav").each_buffer do |buf|
  buf.samples.each do |sample|
    samples.push(sample)
  end
end

def dump(ary)
  ary.each do |s|
    print s, "\n"
  end
end

def neg(input)
  # 足して0になればいいんだから符号を全部逆にすればよい
  return input.map do |v| -v end
end

# aのofsの位置からbを足す
# [aaaaaaaaaaaa]
#    ofs[bbbbbbbbbb]
# [aaaaaaaaaaaaaaaaaaa]
#    ofs[bbbbbbbbbb]
# 長さは、
def add(a,b,ofs)
  output=[]
  end_ind=ofs+b.size-1
  for i in 0..end_ind
    aval=a[i]
    aval=0 if aval==nil
#    print i," ", aval, " ", aval.class, "\n"
    bval=b[i-ofs]
    bval=0 if bval==nil
    out=aval+bval
    out=-32767 if out<-32767
    out=32767 if out>32767
    output[i]=out
  end
  return output
end

def writewav(samples,path)
  buffer=Buffer.new(samples,Format.new(:mono,:pcm_16,48000))
  Writer.new(path,Format.new(:mono,:pcm_16,48000)) do |writer|
    writer.write(buffer)
  end
end


#

#dump(samples)
# add neg and get silence

twice=add(samples,samples,0)
writewav(twice,"twice.wav")

neged=neg(samples)
silent=add(samples,neged,0)
writewav(silent,"silent.wav")
d1=add(samples,neged,1)
writewav(d1,"d1.wav")
d2=add(samples,neged,2)
writewav(d2,"d2.wav")
d4=add(samples,neged,4)
writewav(d4,"d4.wav")
d10=add(samples,neged,10)
writewav(d10,"d10.wav")
d100=add(samples,neged,100)
writewav(d100,"d100.wav")
d1000=add(samples,neged,1000)
writewav(d1000,"d1000.wav")
d10000=add(samples,neged,10000)
writewav(d10000,"d10000.wav")




