require "rubygems"

class MysqlWrapper
  def initialize(host,user,pw,db)
    require "mysql"
    @my = Mysql::new(host,user,pw,db)
  end
  def conv(t,v)
    case t
    when Mysql::Field::TYPE_TINY, Mysql::Field::TYPE_SHORT, Mysql::Field::TYPE_LONG, Mysql::Field::TYPE_INT24, Mysql::Field::TYPE_LONGLONG, Mysql::Field::TYPE_DECIMAL, Mysql::Field::TYPE_YEAR	
      return v.to_i
    when Mysql::Field::TYPE_FLOAT, Mysql::Field::TYPE_DOUBLE
      return v.to_f
    when Mysql::Field::TYPE_TIMESTAMP, Mysql::Field::TYPE_DATE, Mysql::Field::TYPE_TIME, Mysql::Field::TYPE_DATETIME	
      return Time.parse(v)
    when Mysql::Field::TYPE_STRING, Mysql::Field::TYPE_VAR_STRING, Mysql::Field::TYPE_BLOB, Mysql::Field::TYPE_CHAR
      return v
    when Mysql::Field::TYPE_SET, Mysql::Field::TYPE_ENUM, Mysql::Field::TYPE_NULL
      raise "column type #{t} is not implemented"
    else
      raise "column type #{t} is not known"
    end
  end
  # return an array of hashes
  def query(s)
    res = @my.query(s)
    return nil if !res

    # 
    out = []
    print("nr:", res.num_rows, "\n")
    fields = res.fetch_fields
#    fields.each do |f| print("FF:", f.type, "\n" )end
    res.each do |row|
      ent = {}
      fields.size.times do |fi|
        rn = fields[fi].name
        rt = fields[fi].type
        rv = conv(rt, row[fi])
        ent[rn] = rv
#        print( "ent[#{rn}] = #{rv}, #{rv.class} #{rt}\n")
      end
      out.push(ent)
    end
    return out
  end
end


