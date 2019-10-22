
class GeoHash
  MAX_LAT = 90.0
  MIN_LAT = -90.0
  MAX_LONG = 180.0
  MIN_LONG = -180.0

  NORTH = 0
  EAST = 1
  SOUTH = 2
  WEST = 3
  LENGTH_OF_DEGREE = 111100

  class Interval
    attr_reader :high, :low
    attr_writer :high, :low
    def initialize(h,l)
      @high=h
      @low=l
    end
  end
  

  def self.encode(lat,lng,precision)
    # Normal 32 characer map used for geohashing 
    char_map = [ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'j', 'k', 'm', 'n', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' ]

    precision = 6 if precision < 1 || precision > 12

    hash=""
    precision.times do hash+="_" end

    if !(lat <= 90.0 && lat >= -90.0 && lng <= 180.0 && lng >= -180.0) then
      STDERR.print "invalid arg\n"
      return nil
    end
    
    precision *= 5
    lat_interval = Interval.new(MAX_LAT, MIN_LAT)
    lng_interval = Interval.new(MAX_LONG, MIN_LONG)
    interval=nil
    coord=0.0
    mid=0.0
    is_even = true;
    hashChar = 0 #uint
    for i in 1..precision do
      if is_even then
        interval = lng_interval
        coord = lng
      else
        interval = lat_interval
        coord = lat
      end
      mid = (interval.low + interval.high) / 2.0
      hashChar = hashChar << 1
      if coord > mid then
        interval.low = mid
        hashChar = hashChar | 0x01
      else
        interval.high = mid
      end
      if ((i % 5) == 0) then
        hash[(i - 1) / 5] = char_map[hashChar]
        hashChar = 0
      end
      is_even = !is_even
    end
    return hash
  end
end

p GeoHash::encode(36.386527438,136.5287203,12)  == "xn3nfzcnvu26"
p GeoHash::encode(-29.0861865288,27.7625945729733,12)  == "kdgdxc00emfg"


