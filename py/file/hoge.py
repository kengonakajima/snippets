import io
with io.open("/Users/ringo/Downloads/blender-2.81a-macOS.dmg","rb") as f:
    dat=f.read()
    print(len(dat))
    
