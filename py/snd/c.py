import numpy as np

z=1.0+2.0j
u=2.0+3.0j

print(z)
print(u)

print("real:",np.real(z))
print("img:",np.imag(z))

print("z^*=",np.conjugate(z))
print("|z|=",np.abs(z))

v=z+u

print(v)

v=z-u
print(v)
v=z*u
print(v)

v=z/u
print(v)
