#
# 1000 file per dir
#
# 100 * 100 * 100 = 1M dir
# 00/01/01/99.txt
#

d = "/Volumes/Unlabeled"

100.times { |x|
	begin
        Dir.mkdir( "#{d}/#{x}" )
	rescue
	end
    100.times { |y|
		begin
			Dir.mkdir( "#{d}/#{x}/#{y}" )
		rescue
        end
        100.times { |z|
		    begin
                Dir.mkdir( "#{d}/#{x}/#{y}/#{z}"  )
            rescue
            end
            100.times { |n|
                fn = "#{d}/#{x}/#{y}/#{z}/#{n}.txt"
                f = File.open( fn, "w" )
                f.print fn 
                f.close 
                
            }
            print "#{d}/#{x}/#{y}/#{z}\n"
        }
    }
}
