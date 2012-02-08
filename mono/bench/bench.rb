class V
  @value = nil
  @is_a_prime = nil
  def initialize(v,p)
    @value = v
    @is_a_prime = p
  end
  def value
    return @value
  end
  def is_a_prime=(x)
    @is_a_prime = x
  end
  def is_a_prime
    return @is_a_prime
  end
end

def main  
  vals = Array.new

  #init
  n = ARGV[0].to_i
  for i in (0 ..n)
    vals[i] = V.new(i,true)
  end

  #check
  for i in (2 .. n)
    for j in ( 2 .. n)
      next if( i == j )
      if((vals[j].value % i) == 0 ) then
        vals[j].is_a_prime = false
      end
    end
  end

  #count
  cnt = 0
  for i in ( 2 .. n)
    if( vals[i].is_a_prime ) then
      cnt+=1
      print vals[i].value, " "
    end
  end
  print "done. cnt=#{cnt}\n"
end

main()
