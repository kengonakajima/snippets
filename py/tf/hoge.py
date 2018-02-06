import datetime

print(datetime.datetime.now())

import numpy as np
import tflearn

from tflearn.datasets import titanic

print(datetime.datetime.now())

titanic.download_dataset("titanic_dataset.csv")

from tflearn.data_utils import load_csv

data, labels = load_csv( "titanic_dataset.csv", target_column=0, categorical_labels=True,  n_classes=2 )

def preprocess(data, columns_to_ignore ) :
    for id in sorted(columns_to_ignore,reverse=True) :
        [r.pop(id) for r in data]
    for i in range(len(data)):
        data[i][1] = 1. if data[i][1] == "female" else 0.
    return np.array(data, dtype=np.float32)

to_ignore=[1,6]

data = preprocess(data,to_ignore)


print("preprocess done", datetime.datetime.now())
#

net = tflearn.input_data(shape=[None,6])

net = tflearn.fully_connected(net,32)
net = tflearn.fully_connected(net,32)
net = tflearn.fully_connected(net,2, activation="softmax")
net = tflearn.regression(net)

print("setup done", datetime.datetime.now())

model = tflearn.DNN(net)
model.fit( data, labels, n_epoch=10, batch_size=16,  show_metric=True )

print("fit done", datetime.datetime.now())

#

dicaprio = [3, "jack dawson", "male", 19,0,0,  "N/A", 5.0 ]
winslet = [1, "rose dewitt bukater", "female", 17, 1,2, "N/A", 100 ]

dicaprio, winslet = preprocess([dicaprio,winslet], to_ignore )

pred = model.predict([dicaprio,winslet])

print(pred)

print( "Dic:", pred[0][1] )
print( "Wins:", pred[1][1] )


print("pred done", datetime.datetime.now())
