f=File.open("gooo","w")
f.print(Process.pid)
f.close
Process.daemon(true,true)

f=File.open("zooo","w")
f.print(Process.pid)
f.close
