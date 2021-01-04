import numpy as np

A=np.arange(9)

print(A)

A=A.reshape(3,3)

print(A)

print(np.einsum("ii",A))
