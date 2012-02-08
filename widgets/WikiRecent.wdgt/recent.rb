#
# recent entry checker
# v2
# * save "title" to file
#
# 差分はNewファイルにためておく。
# 1個表示するたびに1行づつ消していく。
# 過去10000個分をoldファイルに保存しておく。

# * 1分に1回起動
# * N のエントリをさがし、過去10000個に含まれない分をjavascriptにかえす。
# * javascriptは、たとえば6行が返ってきたら、次の1分で、10秒に1個流していく。
# * テキストはゆっくり上にスクロールする。


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
