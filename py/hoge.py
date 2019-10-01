f=open("hoge_py.txt","w")
for i in range(0,1000000):
    f.write('hoge:{}\n'.format(i))
f.close()
