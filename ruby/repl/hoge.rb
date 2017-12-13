while line=gets
  line.strip!
  line.gsub!(/\s+/,",")
  print line,"\n"
end