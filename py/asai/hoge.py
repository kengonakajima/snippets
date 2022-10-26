import numpy
data = [50,24,50,55,55,72,76,54,57,]
var = (numpy.std(data))
data2 = 0
data2 = sum(data)
num = 1
for data3 in data:
    a = -1
    a = a + 1
    pt = (data[a+1] - (data3/9))*10 + 50
for pt2 in range(9):
    print(f'{num}人目の偏差値 is {round(pt,1)}')
    num = num + 1
