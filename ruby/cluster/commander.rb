require "open3.rb"
#
# rsh commander.
# Usage: c = Commander.new( [ "192.168.1.2", "192.168.1.3" ] )
#        c.command( "cd /etc; ls" )
#
#
#
class Commander
	def initialize( hosts )
		hosts[0]       # raise exception when it's not array
		@hosts = hosts
	end
	def command( com )
		@h_out = {}
		@h_err = {}
		@h_thread = {}

		com.gsub( /\"/, "\\\"" )

		@hosts.each { |h|
			@h_thread[h] = Thread.start {
				i,o,e = Open3.popen3( "rsh #{h} \"#{com}\"" )
				oo = o.read
				ee = e.read
				@h_out[h] = oo if( oo ) 
				@h_err[h] = ee if( ee )
			}
		}

		# wait for threads finish
		@h_thread.keys.each { |i|
			@h_thread[i].join
		}

		out = ""
		@h_out.keys.each { |i|
			out += "Host #{i} output:\n"
			out += @h_out[i]
		}
		err = ""
		@h_err.keys.each { |i|
			err += "Host #{i} errout:\n"
			err += @h_err[i]
		}
		return [out,err]
	end
end






