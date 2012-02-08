class Vector3
    constructor: (x,y,z) ->
        @x=x
        @y=y
        @z=z
    translate: (v) ->
        @x += v.x
        @y += v.y
        @z += v.z
NVEC=10000
NLOOP=10000
ary=[]
ary1=[]
for i in [0..NVEC]
    ary[i] = new Vector3(0,0,0)
    ary1[i] = new Vector3(1,1,1)
for i in [0..NLOOP]
    for j in [0..NVEC]
        ary[j].translate( ary1[j] )
v=ary[NVEC/2]
console.log v.x, v.y, v.z