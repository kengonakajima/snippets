

File.open("conf.txt").read.split("\n").each{|i|
  command, param = i.split("=")
  print command , ":" , param , "\n"
}
