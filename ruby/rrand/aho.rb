require "md5"

srand( Time.now.to_i )
m = MD5.new( (100000 * rand() ).to_s )
print m.hexdigest[0..7]
