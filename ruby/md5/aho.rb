require "digest/md5";

f = "akajksdlfkasldkjfalskdjflakjsdlfjkasldkjflakjsdlfkjasdfdsjdhfaksjdf"

d = Digest::MD5.digest(f)

d.length.times { |i|
    print d[i],"\n"
}
