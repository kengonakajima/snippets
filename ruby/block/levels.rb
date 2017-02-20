require "pp"

# 最初、ブロックと@を使って階層構造を構築する
def program(name,&blk)
  @out={ :name=>name, :groups => {}, :structs => {}, :enums => {} }
  def group(name,&blk)
    @grp={}
    def function(name,&blk)
      @func={ :args=>[]}
      def argument(name,type)
        arg = { :name=>name, :type=>type }
        @func[:args].push(arg)
      end
      blk.call()
      @grp[name]=@func
    end
    blk.call()
    @out[:groups][name]=@grp
  end
  blk.call()
  return @out
end

# 以下が定義ファイルに対応
prog = program(:gameprotocol) do
  group(:Chat) do
    function(:EnterRoom) do
      argument(:room_id, :int )
      argument(:nickname, :string )
    end
    function(:LeaveRoom) do
    end
  end
end


pp prog,"\n"

# 最後に階層構造をたどりながらソース出力

progname = prog[:name]
prog[:groups].each do |grpname,grp|
  grp.each do |fncname,fnc|
    argdefs = fnc[:args].map do |h| "#{h[:type]} #{h[:name]}" end
    argdefstr = argdefs.join(",")
    print "void #{progname}_#{grpname}_#{fncname}(#{argdefstr}){\n"
    print "}\n"
  end
end

