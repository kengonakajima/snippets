def fileLined(path)
  f = File.open(path,"r")
  lines = f.read.split("\n")
  f.close()
  ary = []
  lines.size.times do |i|
    ary.push( "#{(i+1)} : #{lines[i]}" )
  end  
  return ary.join("\n")
end

print fileLined("/etc/hosts"),"\n"

