def c(*args)
  args.each { |a|
    print "c:#{a.to_s}\n"
  }
end

def b(*args)
  args.each { |a|
    print "b:#{a.to_s}\n"
  }
end

def a(*args)
  args.each { |a|
    print "a:#{a.to_s}\n"
  }
end


a b c 10
