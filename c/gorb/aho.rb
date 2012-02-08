
class Main
  $g_ch_dbrequest
  func main() do

    [1,2,3].each { |x|
		print x,"\n"
    }
    h = { "a" => 1 }
    print h["a"]

    i0 = 1000000000000000000000000000000000 # 内部的にbignumがきたらエラーとしていいだろ
    i1 = 11
    i2 = ( i1 * 100 ) % 20
    $g_ch_dbrequest = channel.Create(Query)
    DBInit()
    gameproto.Listen() 
    while true
      event.Poll()
    end
  end
  func DBInit() do
    $my = mysql.Connect( "unix", "", "/opt/local/var/run/mysql5/mysqld.sock", "root", "","test" )	
    if $my == nil then
      log.Exit("db_goro: cannot connect to mysql\\n")
    end
    $my.Use("onto")
    $my.Query( "set names utf8")
  end

  func recv_Request(Query req)  do
    res = my.Query( req.q )
    var err bool
    var result DBResult
    if res == nil then
      log.Printf( "DBREQ: error. err=%#v\n",err)
      err = true
    else
      head = req.query[0..3]
      if head == "sel" then
        result = res.fetch_row()
        err = false
      end
    end
    send_RequestResult( result, err )

  end

end

class Query
  @q
  @result
  @err
end


class Player
  def LoadByName( name, limit )
    limitstr = ""
    if limit > 0 then
      limitstr = sprintf( "limit %d", limit )
    end
    req = Query.new
    req.q = sprintf( "select * from Player where name='%s' %s", name, limitstr )

    db.send_Request(req)
    res = wait db.recv_RequestResult()    # waitすることでエントリポイントを1個追加して継続する
    
    return res

  end

end

class Gameproto
  def recv_getMap( x,y,w,h )
    p = Player.LoadByName( self.authname )
    data = mem.Alloc(int,100)
    send_getMapResult( data, 100 )
  end
end

