

def crc2(msg)
  r = 0xFFFFFFFF
  msg.size.times do |i|  #	for (i = 0; i < n; i++) {
    r ^= msg[i]  # 		r ^= c[i];
    8.times do |i|  #		for (j = 0; j < CHAR_BIT; j++)
      if (r & 1)==1 then #			if (r & 1) r = (r >> 1) ^ CRCPOLY2;
        r = (r >> 1) ^ 0xEDB88320
      else  # else
        r >>= 1  #       r >>= 1;
      end
      r &= 0xffffffff
	end
  end
  return r ^ 0xFFFFFFFF
end
if ARGV.size != 1 then
  STDERR.print "need 1 arg\n"
  exit 1
end

print crc2(ARGV[0]),"\n"
