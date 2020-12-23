import glob
import io

files=glob.glob("./*.txt")
path=files[0]
print(path)
with io.open(path,"rb") as f:
    bytes=f.read()
    print(len(bytes))
