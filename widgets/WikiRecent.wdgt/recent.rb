#
# recent entry checker
# v2
# * save "title" to file
#
# $B:9J,$O(BNew$B%U%!%$%k$K$?$a$F$*$/!#(B
# 1$B8DI=<($9$k$?$S$K(B1$B9T$E$D>C$7$F$$$/!#(B
# $B2a5n(B10000$B8DJ,$r(Bold$B%U%!%$%k$KJ]B8$7$F$*$/!#(B

# * 1$BJ,$K(B1$B2s5/F0(B
# * N $B$N%(%s%H%j$r$5$,$7!"2a5n(B10000$B8D$K4^$^$l$J$$J,$r(Bjavascript$B$K$+$($9!#(B
# * javascript$B$O!"$?$H$($P(B6$B9T$,JV$C$F$-$?$i!"<!$N(B1$BJ,$G!"(B10$BIC$K(B1$B8DN.$7$F$$$/!#(B
# * $B%F%-%9%H$O$f$C$/$j>e$K%9%/%m!<%k$9$k!#(B


oldfile = "/tmp/wikirecent_old"

ary = []

begin
	f = File.open( oldfile )
	f.read.split("\n").each { |line|
		ary.push(line)
	}
    f.close
rescue
    print "cannot read oldfile '#{oldfile}\n"
end

if( ary.length > 100 ) then   
	ary = ary[ (ary.length - 100 ) .. -1 ]
end


out = `curl -s http://en.wikipedia.org/wiki/Special:Newpages | grep "<li>" `
File.open("/tmp/wikirecent_ck","w").write(out)

out.split("\n").each { |line|
	line.split(";").each { |line|
		if( line =~ /title=\"([a-zA-Z0-9_\-.,: ]+)"/ ) then
	        k = $1
	        found = false
	        ary.each { |x|
				if( x == k ) then
	                found = true
	            end
	        }
	        next if( found ) 
            f = File.open( oldfile, "a+" )
            f.print( k,"\n")
            print(k,"\n")
			ary.push(k)
            f.close
	    end
    }
}

# overwrite old titles

f = File.open( oldfile,"w")
ary.each { |title|
	f.print( title,"\n")
}
f.close
