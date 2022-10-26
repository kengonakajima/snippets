from imposm.parser import OSMParser

class HWCounter(object):
    highways=0
    def ways(self, ways):
        # callback method for ways
        for osmid, tags, refs in ways:
            if 'highway' in tags:
              self.highways += 1

# instantiate counter and parser and start parsing
counter = HWCounter()
p = OSMParser(concurrency=4, ways_callback=counter.ways)
p.parse('/Users/ringo/osm/japan-latest.osm.pbf')

# done
print counter.highways
