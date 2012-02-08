#
# 各種表現のテスト
#
#

module Main # golangの package mainに相当


  # literal
  def test_lit_i()          
    1  # lit 1
  end
  def test_lit_long()
    10000000000000000000000000000000000 #  lit 10...
  end
  def test_lit_neg()
    -1 # lit
  end
  def test_lit_hex()
    0xff90  # lit 65424
  end
  def test_lit_f()
    1.2e-3  # lit 0.0012
  end
  def test_lit_char0()
    'a'  # lit str'a'  "a"と区別がつかない　
  end
  def test_lit_char1()
    ?a    # lit 97
  end
  def test_lit_char2()
    ?\a    # lit 7
  end
  def test_lit_regex()
    /aho[0-9]+.*/   # lit (?-mix:aho[0-9]+.*)
  end
  def test_lit_s()
    %q!I said, "You said, 'She said it.'"! # str 'I said, "You said, 'She said it.'"'
  end


  # variable
  def test_var0()
    var i            # call call
  end
  def test_var1()   
    var i(int)        # call call call
  end
  def test_var2()
    var i = 0         #  call(nil,var) arglist lasgn i lit 0   varに続けて代入すると型予測していい
  end
  def test_var3()
    var i = "aho"
  end
  def test_var4()
    var i = j = 0    # call(nil,var) arglist lasgn(i) lasgn(j) lit 0
  end
  def test_var5()
    var i,j=0        # call(nil,var) arglist call(nil,i), lasgn(iii) lit 0   var に対して２つの引数が与えられた
  end
  def test_var6()
    var i=0,j=0      # call(nil,var) arglist lasgn, lasgn varに２つ引数があるのは上と同じ
  end
  def test_var7()
    var i="aho",j=0  # 上と同じく引数が２個になる
  end

  # 複数をカンマでならべるのはNG
  def test_asign_i0()
    iii = 0          # lasgn, lit
  end
  def test_asign_s0()
    iii = "str"      # lasgn, str
  end
  def test_asign_s1()
    a = <<EOF  # lasgn str
EOF
;
  end
  def test_asign_s2()
    a = "aho#{b}hoge#{c}fuga#{d}"    # lasgn(a) dstr( 'aho' evstr call str evstr call .. )
  end

  def test_asign_var()
    aaa = bbb        # lasgn, call
  end
  def test_asign_var2()
    aaa = bbb = ccc   #lasgn, lasgn, call 
  end
  def test_asign_multi()
    a,b = 1,2         # masgn array array  これはエラーになる：a(int),b(int) = 1,2
  end

  def test_semicolon()
    a = 1 ; b = 1         # 問題なし
  end

  # ops
  def test_op_plus()
    aaa = 1 + 2      # lasgn call lit arglist lit           lit1に対して、+メソッドを 2という引数で呼ぶ
  end
  def test_op_plus2()
    aaa = 1 + 2 + 3  # lasgn call call 
  end

  def test_op_inc0()
    aaa +=1            # Rubyでは++がどうしても使えないのだ
  end
  def test_op_inc1()
    ++aaa               # call( call(aaa,+@), +@) になってしまう
  end
  def test_op_inc2()
    a = b[++x]          # 通るが + lasgn a call b [] arglist call call x +@ +@　になり意図していない結果になる
  end
  def test_op_inc3()
    a = b[x+=1]         # lasign x call x + 1が一番内側にくるので、C言語と同じく、1ふえたxが[]の引数として与えられる
  end
  def test_op_mixed()
    aaa = 1 + 2 * 3 + 4 # lasgn  call call call
  end
  def test_op_single0() # lasgn not true
    aaa = !true
  end
  def test_op_single1()
    aaa = -1            # lasgn lit -1
  end
  def test_op_single2()
    aaa = +2            # lasgn lit 2
  end
  def test_op_cond()
    aaa = true ? 1 : 2   # if( true, lit, lit )
  end
  def test_op_lt()
    a = 1 < 2           # call(1,<)
  end
  def test_op_gt()
    a = 2 > 1           # call(2,>)
  end
  def test_op_pluseq()
    a += "str"          # call(lvar a, arglist(str))
  end

  def test_op_and0()
    a = true and false # and(lasgn a true, false )  ( a = true ) and false になってしまう
  end
  def test_op_and1()
    a = ( true and false )  # lasgn a (and true false )   カッコが必要
  end
  def test_op_and2()
    a = true && false   # これだと上と同じになりカッコがいらない。 andは = よりひくいが &&は=より高い
  end
  def test_op_iand()
    a = 1 & 2          # call( 1,&)
  end

  # ptr
  def test_ptr_init0()
    var i(*int)        # call(nil-var), call(i), splat call(int)
  end



  def test_ptr_acc0()
    f( @p )                  # (nil,f,arglist)     arglistは、  ivar p
  end
  def test_ptr_acc1()
    f( @p[0])               #  上の call ivar p []
  end
  def test_ptr_calc0()
    a = @p          # ivar
  end

  # structure

  def test_if0()
    if a > b then    # if( call, call
      print          # １行だとブロックにならないが、２行だとblockがはさまる
      print
    end
  end

  def test_ifelse()
    if a > b then
      print0
      print1
    else
      print2
      print3
    end
  end

  def test_while()
    while f()        # while call.f 
      p0
    end
  end
  def test_case()
    case x()            #  case  call when array  when array lit lit
    when 0
    when 1,2
    end
  end

  def test_times0()
    100.times {}    # iter call(times,nil) lit 
  end



  def test_times1()
    100.times { |i| 
		print i     # iter call(times, lasgn i)
    }
  end

  def test_times2()
    100.times(2) { |i| 
		print i     # iter call(times, arglist, lasgn i)
    }
  end
  
  def test_for0()
    for i in [1,2,3]  #  for array lit lit lit lasgn(i) call p0
      p0
    end
  end
  def test_for1()
    for i in a    # for call(a) lasgn(i) call p0
      p0
    end
  end
  def test_break()     # while  block(break next)
    while true
      break
      next
    end
  end

  # range init
  def test_range_init()
    a = [1..2]      # lasgn array lit 1 2 4312079640これなんだろ
  end
  def test_range_init2()
    a = [b..c]      # lasgn array dot2 call(b) call(c)   dot2というトークンがあるのね。
  end

  # array init
  def test_ary_init0()
    a = [1,2,3]      # lasgn, array, lit, lit, lit
  end
  def test_ary_init1()
    var i(int[10])   # []を呼ぶ
  end
  def test_ary_init2()
    var i(int[10][10]) # []を２回呼んでいる
  end
  
  def test_ary_access0()
    b = a[0]         # lasgn, call( call(a) ) lit
  end
  def test_ary_access1()
    b = a[1][2]   # [] を２重に
  end
  def test_ary_asgn0()
    b[0] = 1     # []=というメソをよぶ
  end
  def test_ary_asgn1() #  bの[]を []=のあとによぶ
    b[0][1] = 2
  end
  def test_ary_asgn2()  # []=のひきすうを２つでよぶ。２次元以上の配列はこっちのほうがいいのかな
    b[0,1]=2
  end
  def test_ary_asgn3()
    a[0].b[1] = 2    # attrasgnが外側にきて、その第１引数が call(a,[])で第２引数が(1,2)　正確にパースするなあ。。
  end
  
  # hash init
  def test_hash_init()
    h = { 1 => "a", 2 => "b" }   # lasgn, hash, lit, str, lit, str
  end
  def test_hash_access0()
    b = h[1]                  # lasgn, call, call
  end
  def test_hash_access1()
    b = h["aho"]             #  lasgn, call, call
  end
  # subrontine
  def test_sub0()
    func( t0(Type0), t1(Type1) ) do   # iter call func arglist..
      p0
      p1
    end
  end
  def test_sub_call1()
    func( a0 )
  end
  def test_sub_call2()
    func( a0, a1 )
  end
  def test_sub_call3()
    func( @a0, @a1 )   # @をポインタ演算に使うとした ivar.
  end
  def test_sub_call4()
    x.func( a0 )   
  end


  # objects, types
  def test_object0()       # iter call(type) arglist block .. |arglist|　は無し
    type(Player) do
      i(int)
      j(int)
      k(Status)
    end
  end

  def test_object_asgn1()
    p.x = 1          #  attrasgn call(p,x=,arglist)
  end
  def test_object_asgn2() 
    y = p.x          # lasgn y call(p,x)
  end
  def test_object_asgn3()
    p.x.y = 1          # p.xが先によばれて、その結果の yに1を代入するとなる ( attrasgnが最も外側 ) それでいい。
  end

  def test_object_alloc1()
    p = Player.new(1,"aho")        #  lasgn call(const Player,new, arglist..) 
  end
  def test_object_alloc2()
    p = Player.pnew(1,"aho")        #  newとメソ名が違っても同じようにパースされる
  end
  def test_object_ptr_alloc()
    var p = Player.new(1,"aho")     # 宣言時に代入できる場合は型予測を使うので型を書かない
  end
  

  
  # method
  def test_meth()       # methで定義した場合は第１引数がレシーバになる
    meth(p(*Player),n(str)) do    #  iter call method arglist ..
      p0
      p1
    end
  end

  # meth chains
  def test_methchain()
    a.b.c().d.e()  #  call call call call call   ()の書き方に影響はない
  end

  # wait
  def test_wait()
    ret = wait x.send_y(a,b)  #   call(nil,wait)になってたらok
  end


end
