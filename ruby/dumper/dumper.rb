#
# Convert binary data to a C source with arrays named by file names
#

src = "/* generated source code by dumper.rb */\n"


def to_array(name,data)
  ary = data.unpack("C*")
  l = ary.size
  out = "const char #{name}[#{l}] = {\n"
  l.times do |i|
    out += sprintf( "0x%02x",ary[i] )+ ","
    out += "  " if (i%16) == 7
    out += "\n" if (i%16) == 15 
  end
  out += "};\n"
end


ARGV.each do |path|
  data = File.open(path,"rb").read()
  nm = "g_data_" + File::basename(path).gsub(".","_")
  src += "\n"
  fileinfo = `file #{path}`.strip
  src += "/* file info: #{fileinfo} */\n"
  src += to_array(nm, data)
end

print src, "\n"
