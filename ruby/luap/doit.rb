# parse lua and print summaries
require "../rumino/rumino"
require "pp"

$KCODE='u'

f=ARGV[0]
cmd( "ulimit -s unlimited")
ary = eval( cmd( "ruby lua-parser/lua2sexp -a #{f}" ) )
comments = eval( cmd( "ruby lua-parser/lua2sexp -c #{f}" ) )

$calls=Hash.new(0)

$outary=[]

def logDefn(up,cur,sz,md )
  if up then
    upary = up[1][1..-1]
    upary.push(up[2]) if up[2]
  else
    upary = nil
  end
  
  curary = cur[1][1..-1]
  curary.push(cur[2]) if cur[2]

  curary.shift if curary[0] == :_G
  upary.shift if upary and upary[0] == :_G
  curary.shift if upary  # omit local var name typically

  if upary then
    $outary.push( {:action=>"funcdef", :up=>upary[0], :name=>curary[0], :size=>sz, :sha1=>md } )
  else
    $outary.push( {:action=>"gfuncdef", :up=>nil, :name=>curary[0],:size=>sz, :sha1=>md } )
  end
end


$uppername = nil

def scan(d,ary)
  return if ary==nil
  t = ary[0]
  sp = " " * d
#  print "d:#{d} #{sp} sz:#{ary.size} #{t}\n"

  case t
  when :chunk 
    scan(d+1,ary[1])
    scan(d+1,ary[2])
  when :statlist
    ary[1..-1].each do |s| scan(d+1,s) end
  when :function 
    fname,fb = ary[1],ary[2]
    if fname then
      if $uppername then 
        origun = $uppername.dup
      else
        origun = nil
      end
      logDefn($uppername,fname, deepcount(fb), sha1(fb.to_s) )
      $uppername = fname
    end
    scan(d+1,fb)
    $uppername = origun
  when :funcbody
    pl,blk = ary[1],ary[2]
    scan(d+1,blk)
  when :block
    chk=ary[1]
    scan(d+1,chk)
  when :deflocal
    nm,explist = ary[1],ary[2]
    scan(d+1,explist)
  when :explist
    ary[1..-1].each do |e| scan(d+1,e) end
  when :exp
    ary[1..-1].each do |e| scan(d+1,e) end
  when :prefixexp
    ary[1..-1].each do |e| scan(d+1,e) end
  when :call
    pf,meth,args = ary[1],ary[2],ary[3]
#    pp "PPPPPP", pf, meth, args
    if pf[0]==:prefixexp and pf[1][0] == :var and pf[1][1][0]==:name then
      if pf[1][1][1] == :require then
        if args[0]==:args and args[1][0] == :explist and args[1][1][0] == :exp and args[1][1][1][0] ==:str then
          modname = args[1][1][1][1]
          print "REQUIRE: #{modname}\n"
        end
      else
        lastname = nil
        if meth and meth[0]==:name then 
          lastname = meth[1].to_s
        else
          lastname = pf[1][1][-1].to_s
        end
#        print "CALL-LASTNAME:", lastname, "\n"
        $calls[lastname]+=1
      end
    end

    explist = args[1]
    if explist then
      explist[1..-1].each do |e| scan(d+1,e) end
    end
  when :str
    s=ary[1]
#    if s =~ /\%/ or s =~ /\*/ or s =~ /\$$/ or s =~ /^\^/ then
      $outary.push( { :action=>"strlit", :val=>s} )
#    end
  else
    ary[1..-1].each do |e|
      if typeof(e)==Array then
        scan(d+1,e)
      end
    end
  end
end


def deepcount(ary)
  cnt=1
  if typeof(ary) == Array then
    ary.each do |e|
      cnt += deepcount(e)
    end
  end
  return cnt
end

scan(0,ary)

$outary.push( { :action=>"total", :size=> deepcount(ary) } )



#
#
#


$calls.valsort.reverse.each do |name,cnt|
  $outary.push( { :action=>"call", :name=>name, :count => cnt } )
end

comments[1..-1].each do |c|
  content = c[1]
  $outary.push( { :action=>"comment", :val=>content} )
end

pp $outary
