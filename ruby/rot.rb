# port from gl-matrix
def rotx(v, o, rad)
  # Translate point to the origin
  p=[ v[0]-o[0], v[1]-o[1], v[2]-o[2] ]
  # perform rotation
  r= [ p[0], p[1] * Math.cos(rad) - p[2] * Math.sin(rad), p[1] * Math.sin(rad) + p[2] * Math.cos(rad) ]
  # translate to correct position
  return [ r[0] + o[0], r[1] + o[1], r[2] + o[2] ]
end

print rotx( [0,1,0], [0,0,0], Math::PI/4.0 ), "\n" # 0, 0.707, 0.707
print rotx( [0,1,0], [0,0,0], Math::PI ), "\n" # 0, -1, 0
print rotx( [0,1,0], [0,0.5,0], Math::PI ), "\n" # 0,0,0

def rotz(v,o,rad)
  # Translate point to the origin
  p=[ v[0]-o[0], v[1]-o[1], v[2]-o[2] ]
  # perform rotation
  r=[ p[0] * Math.cos(rad) - p[1] * Math.sin(rad), p[0] * Math.sin(rad) + p[1] * Math.cos(rad), p[2] ]
  # translate to correct position
  return [ r[0] + o[0], r[1] + o[1], r[2] + o[2] ]
end

print rotz( [1,0,0], [0,0,0], Math::PI/4.0), "\n" #  0.707, 0.707, 0
print rotz( [1,0,0], [0,0,0], Math::PI), "\n" # -1,0,0
print rotz( [1,0,0], [0.5,0,0], Math::PI), "\n" # 0,0,0

def roty(v,o,rad)
  # Translate point to the origin
  p=[ v[0]-o[0], v[1]-o[1], v[2]-o[2] ]
  # perform rotation
  r=[ p[2] * Math.sin(rad) + p[0] * Math.cos(rad), p[1], p[2] * Math.cos(rad) - p[0] * Math.sin(rad) ]
  # translate to correct position
  return [ r[0] + o[0], r[1] + o[1], r[2] + o[2] ]  
end

print roty([1,0,0], [0,0,0], Math::PI/4.0), "\n" # 0.707, 0, 0.707
print roty([1,0,0], [0,0,0], Math::PI), "\n" # -1,0,0
print roty([1,0,0], [0.5,0,0], Math::PI), "\n" # 0,0,0
