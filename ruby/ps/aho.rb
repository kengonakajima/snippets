

def splitringo(x,y)
    x.split.each { |i|
        y.call i
    }
end

hoge = Proc.new { |aho|
        print "hoge#{aho}\n"
}

while line = gets

    splitringo(line,hoge)

end
