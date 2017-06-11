require 'nbt_utils'

@file = NBTUtils::File.new
@tag = @file.read(ARGV[0])
puts @tag.to_s
@file.write( "hoge.nbt", @tag, false )


