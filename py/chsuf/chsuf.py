#
# print out command line for changing file name suffix in a directory
# > ls
# aho.lib hoge.lib quiquiq_dbg.lib
# > python chsuf.py . _mdd
# mv aho.lib aho_mdd.lib
# mv hoge.lib hoge_mdd.lib
# mv quiquiq_dbg.lib quiquiq_mdd_dbg.lib
#

import os
import sys
import re

args = sys.argv

if len(args) != 3:
    raise Exception("arg error. specify WORKDIR and ADDSTRING! ")
    
pin = os.popen( "find " + args[1] )

for line in pin:
    x = line.strip()
    m = re.match( '(.*)(_dbg\.lib)$', x )
    if not m:
        m = re.match( '(.*)(\.lib)$', x )
    if m:
        body = m.group(1) + args[2] + m.group(2)
    else:
        body = None
    if body:
        print "mv " + x + " " + body
        
    
