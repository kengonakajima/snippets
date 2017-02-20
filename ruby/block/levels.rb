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
hoge = program(:gameprotocol) do
  group(:Chat) do
    function(:EnterRoom) do
      argument(:room_id, :int )
      argument(:nickname, :string )
    end
    function(:LeaveRoom) do
    end
  end
end


pp hoge,"\n"


