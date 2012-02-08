#
# Bigtable implementation
#
#
#
#
#

# (row:string, column:string, time:int64) → string 
#
#
#
#

# Simply:    
#  aho["hoge"]["fuga:muga"] = "aho"
#  aho["com.cnn.www"]["anchor:www.c-span.org"] = "CNN"
#  aho["com.cnn.www"]["contents:"] = "<html>............</html>"
#  



#{ "com.cnn.www" => { "contents" => "<html> .... ",
#                     "anchor:cnnsi.com" => "CNN",
#                     "anchor:my.look.ca" => "CNN.com" } ... }
                   
                     
#                     { "com.cnn.www" => { "contents" => "<html> .... ",
#                                           "anchor:"  => { "cnnsi.com"  => "CNN",
#                                                           "my.look.ca" => "CNN.com" } } ... }                                                           

#bt = Bigtable.new( "/home/ringo/bigtablefile" )
#bt.transact( "com.cnn.www" ) { |row|
#　row.set( "anchor:www.c-span.org" => "CNN" )
#　row.delete( "anchor:www.abc.com" )
#}

#scanner = bt.newScanner( "com.cnn.www" )
#scanner.version = ALLVERSIONS
#scanner.family = "anchor"
#scanner.each { |ent|
#　print scanner.family, " ", ent.columnname, " ", ent.timestamp, " ", ent.value, "\n"
#}

#Table *T = OpenOrDie( "/bigtable/web/webtable" );
#RowMutation r1( T, "com.cnn.www" ) ;
#r1.Set( "anchor:www.c-span.org", "CNN" );
#r1.Delete( "anchor:www.abc.com" );
#Operation op;
#Apply( &op, &r1 );

#Scanner scanner(T);
#ScanStream *stream;
#stream = scanner.FetchColumnFamily( "anchor" );
#stream->SetReturnAllVersions();
#scanner.Lookup( "com.cnn.www" );
#for (; !stream->Done() ; stream->Next() ){
#　printf( "%s %s %lld %s\n",
#　scanner.RowName(),
#　stream->ColumnName(),
#　stream->MicroTimestamp(),
#　stream->value() );
#}

# identical with ( columunname, timestamp )
class Value
  @columnname = nil
  @rowname = nil
  @timestamp = nil
  @value = nil
  def initialize(rowname,columnname,timestamp,value)
    @rowname = rowname
    @columnname = columnname
    @timestamp = timestamp
    @value = value
  end
  def rowname
    @rowname
  end
  def columnname
    @columnname
  end
  def timestamp
    @timestamp
  end
  def value
    @value
  end
end

# [spri][fam][col]
# Hash Hash Array
# row: spri
# col: fam:col
#
class InfArray
  @top = nil
  @top2 = nil
  def initialize()
    @top = Hash.new(nil)
    @top2 = Hash.new(nil)
  end
  def push( spri, ssec, vstr ) 
    if( @top[spri] == nil ) then
      @top[spri] = Hash.new
    end
    if( @top2[ssec] == nil ) then
      @top2[ssec] = Hash.new
    end
    if( @top[spri][ssec] == nil ) then
      @top[spri][ssec] = Array.new
    end
    if( @top2[ssec][spri] == nil ) then
      @top2[ssec][spri] = Array.new
    end
    v = Value.new(spri, ssec, Time.now, vstr)
    @top[spri][ssec].push(v)
    @top2[ssec][spri].push(v)
  end
  
  def pop( spri, ssec )
    return nil if( @top[spri] == nil )
    return nil if( @top[spri][ssec] == nil )
    @top[spri][ssec].pop.getval
  end
  
  def each( spri, ssec )
    return nil if( @top[spri] == nil )
    return nil if( @top[spri][ssec] == nil )    
    @top[spri][ssec].each { |e|
      yield e.value
    }
  end
  def scanner( spri, ssec )
    Scanner.new( spri, ssec, self )
  end
  # get all columns in a row
  def rowAll(spri)
    @top[spri].keys.each { |x|
      @top[spri][x].each { |y|
        yield x,y
      }
    }
  end
  # get all rows in a column
  def colAll(ssec)
    if( ssec =~ /^([a-zA-Z0-9_]+):.*/ ) then
      family = $1
      @top2.keys.each { |sk|
        if( sk =~ /^(#{family}):.*/ ) then
          @top2[sk].keys.each { |ssk|
            @top2[sk][ssk].each { |y|
              yield ssk,y
            }
          }
        end
      }
    else
      @top2[ssec].keys.each { |x|
        @top2[ssec][x].each { |y|
          yield x,y
        }
      }
    end
  end
end

class Scanner
  @rowkey = nil
  @colkey = nil
  @family = nil
  @ia = nil
  def initialize(rowkey, colkey, infarray)
    @rowkey = rowkey
    @colkey = colkey
    @ia = infarray
  end
  def version=(arg)
    @version = arg
  end
  def family=(arg)
    @family = arg
  end
  def family
    @family
  end
  def rowname
    @rowkey
  end
  def colname
    @colkey
  end
  def rowAll
    return nil if @rowkey == nil
    ary = []
    @ia.rowAll(@rowkey) { |col,val|
      next if( family and !(col =~ /^#{family}:/) )
      ary.push(val)
    }
    ary.sort { |v1,v2|
      if( v1.timestamp < v2.timestamp ) then
        -1
      elsif( v1.timestamp > v2.timestamp ) then
        1
      else
        0
      end
    }
    return ary
  end
  def colAll
    return nil if @colkey == nil
    ary = []
    @ia.colAll(@colkey) { |row,val|
      ary.push(val)
    }
    ary.sort { |v1,v2|
      if( v1.timestamp < v2.timestamp ) then
        -1
      elsif( v1.timestamp > v2.timestamp ) then
        1
      else
        0
      end
    }
    return ary
  end
  def getNewestCol
    ary = colAll
    return nil if( ary == nil )
    ary.pop
  end
  def getOldestCol
    ary = colAll
    return nil if( ary == nil )
    ary.shift
  end
  def getNewestRow
    ary = rowAll
    return nil if( ary == nil )
    ary.pop
  end
  def getOldestRow
    ary = getAll
    return nil if( ary == nil )
    ary.shift
  end
end


def main
  ia = InfArray.new
  ia.push( "asdf", "fuga", "qqq" )
  sleep 1
  ia.push( "asdf", "fuga", "ppp" )
  sleep 1
  ia.each( "asdf", "fuga" ) { |e|
    print e, "\n"
  }
  ia.push( "asdf", "fuga:AAA", "CNN")
  sleep 1
  ia.push( "asdf", "fuga:BBB", "CCCC")
  sleep 1
  ia.push( "asdf", "hoge:CCC", "DDDDDDDD")
  sleep 1
  ia.push( "qwer", "fuga:AAA", "KKKK")
  sleep 1
  ia.push( "qwer", "fuga:BBBB", "JJJJJJ")


  scanner = ia.scanner( "asdf", nil )
  scanner.family = "fuga"
  scanner.rowAll.each { |ent|
    print scanner.rowname, " ", scanner.family, " ", ent.columnname, " ", ent.timestamp, " ", ent.value, "\n"
  }
  
  scanner.family = "hoge"
  ent = scanner.getNewestRow
  if(ent) then
    print scanner.rowname, " ", scanner.family, " ", ent.columnname, " ", ent.timestamp, " ", ent.value, "\n"
  end

  STDERR.print "Colscan:\n"
  sc2 = ia.scanner( nil, "fuga:" )
  sc2.colAll.each { |ent|
    print sc2.colname, " ", scanner.family, " ", ent.rowname, " ", ent.timestamp, " ", ent.value, "\n"    
  }
  ent = sc2.getNewestCol
  if( ent ) then
    print sc2.colname, " ", scanner.family, " ", ent.rowname, " ", ent.timestamp, " ", ent.value, "\n"    
  end
  
  

  
end


#
#
#

main()
