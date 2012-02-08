
module Fuck1

	def fuck
		print "1\n"
	end
end

module Fuck2
	def fuck
		print "2\n"
	end
end



class Aho

include Fuck2
include Fuck1


	def initialize( a )
		fuck
	end

end


a = Aho.new(2)
