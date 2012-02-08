#
# recent entry checker
# v2
# * save "title" to file
#
# $B:9J,$O(BNew$B%U%!%$%k$K$?$a$F$*$/!#(B
# 1$B8DI=<($9$k$?$S$K(B1$B9T$E$D>C$7$F$$$/!#(B
# 
# 1$BJ,$K(B1$B2sAv$i$;!"(B


showline = ARGV[0]
showline = 11 if(showline==nil)

file = "/tmp/wikipedia_recent_log"

ary = []

begin
	f = File.open( file )
	f.read.split("\n").each { |line|
		ary.push(line)
	}
    f.close
rescue
    print "file error\n"
end


out = `curl -s http://en.wikipedia.org/w/index.php?title=Special:Recentchanges&limit=250 | grep span | grep newpage`
File.open("/tmp/wikipedia_ck","w").write(out)

out.split("\n").each { |line|
	line.split(";").each { |line|
		next if( ! ( line =~ /newpage/ ) )

		if( line =~ /title=\"([a-zA-Z0-9_\-.,: ]+)"/ ) then
	        k = $1
	        found = false
	        ary.each { |x|
				if( x == k ) then
	                found = true
	            end
	        }
	        if( found ) then
	            next
	        else
	            f = File.open( file, "a+" )
	            kk = k.gsub(" ", "_" )
	            kk.gsub!(",","%2c")
	            str = "<a href=\"javascript:openLinkInBrowser('http://en.wikipedia.org/wiki/#{kk}')\">+ #{k}</a><BR>"
	            f.print( str,"\n")
	            f.close
	        end
	    end
    }
}


print `/usr/bin/tail -#{showline} #{file}`
