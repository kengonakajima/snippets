import numpy as np

A=np.matrix([[3.,1.,2.],[2.,3.,1.]])

print(np.shape(A))
print(A)

c=2.
print(c*A)

B=np.matrix([[-1.,2.,4.],[1.,8.,6.]])
print(A+B)

B=np.matrix([[4.,2.],[-1.,3.],[1.,5.]])
print(np.matmul(A,B))

print(np.eye(N=8))
