// port from https://raw.githubusercontent.com/simplegeo/libgeohash/master/geohash.c

using System;
using System.Text;

namespace geohash
{
    class GeoHash {
        const double MAX_LAT = 90.0;
        const double MIN_LAT = -90.0;

        const double MAX_LONG = 180.0;
        const double MIN_LONG = -180.0;

        const int NORTH = 0;
        const int EAST = 1;
        const int SOUTH = 2;
        const int WEST = 3;

        const double LENGTH_OF_DEGREE = 111100;
        
        class Interval {
            public double high;
            public double low;
            public Interval(double h,double l) { high=h; low=l; }
        };

        /* Normal 32 characer map used for geohashing */
        static char[] char_map = new char[32] { '0', '1','2','3','4','5','6','7','8','9','b','c','d','e','f','g','h','j','k','m','n','p','q','r','s','t','u','v','w','x','y','z' };
        
        
        static public string encode(double lat, double lng, int precision ) {
            if(precision < 1 || precision > 12) precision = 6;
            StringBuilder hash = new StringBuilder("__________________");


            if(lat <= 90.0 && lat >= -90.0 && lng <= 180.0 && lng >= -180.0) {
                precision *= 5;
                Interval lat_interval=new Interval(MAX_LAT, MIN_LAT);
                Interval lng_interval=new Interval(MAX_LONG, MIN_LONG);
                Interval interval;
                double coord, mid;
                bool is_even = true;
                uint hashChar = 0;
                int i;
                for(i = 1; i <= precision; i++) {
                    if(is_even) {
                        interval = lng_interval;
                        coord = lng;                
                    } else {
                        interval = lat_interval;
                        coord = lat;   
                    }
            
                    mid = (interval.low + interval.high) / 2.0;
                    hashChar = hashChar << 1;
                    if(coord > mid) {
                        interval.low = mid;
                        hashChar |= 0x01;
                    } else {
                        interval.high = mid;
                    }
                    if((i % 5)==0) {
                        Console.WriteLine( "i:"+i+","+ char_map[hashChar]);                    
                        hash[(i - 1) / 5] = char_map[hashChar];
                        hashChar = 0;
                    }
                    is_even = !is_even;
                }
            }
            return hash.ToString();
        }
    }
        
    class Program
    {
        
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!" + GeoHash.encode(36.62392,137.13422,7));
        }
    }
}



