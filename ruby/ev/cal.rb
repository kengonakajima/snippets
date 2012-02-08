lines = File.open( "/Users/ringo/Documents/eval.csv").read.split("\r")
ppl = lines.shift.split(",")[4..-1]

pplh = {}
pplah = {} # age
lines.each { |line|
	tk = line.split(",")
    next if tk[0] == nil or tk[0].empty?
	revenue = tk[2].to_i
	totalpercent = tk[3].to_i
	ppl.size.times { |x|
		pplh[ppl[x]]=0 if pplh[ppl[x]]==nil
        if tk[0] == "age" then
            pplah[ppl[x]] = tk[4+x].to_i
        else
            percent = tk[4+x].to_i 
            percent *= 100.0 / totalpercent
            pplh[ppl[x]] += revenue * percent / 100.0
        end
	}
}
total = 0
ary = []
pplh.keys.each { |x| 	ary.push( [x,pplh[x]*0.40+pplah[x]] ) if( x and pplh[x] )  }
ary.sort! {|a,b|	a[1] <=> b[1]  }
ary.each { |x| 	total += x[1]  } 
ary.each { |x|	print x[0]," : ", x[1], "\n"  }
print "Total: ", total , "\n"
