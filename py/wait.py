import os
import subprocess

print "popen..."

proc = subprocess.Popen( "/bin/cat", bufsize=0, cwd=".", stderr=subprocess.PIPE, stdout=subprocess.PIPE)


print "wait.."

def x():
    try:
        proc.wait()
    except OSError, e :
        return proc.returncode
    return proc.returncode

output = x()

print output


