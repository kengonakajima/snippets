

callback = Proc.new { |data|
    print "data=", data, "\n"
}

callback = Proc.new { |data|
    print "hoge=", data, "\n"
}




File.open("aho.rb").read.split("\n").each {  |line|
    callback.call(line)
}

    
