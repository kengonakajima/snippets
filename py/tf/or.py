import datetime

print(datetime.datetime.now())

import tensorflow as tf
import tflearn

X = [[0., 0.], [0., 1.], [1., 0.], [1., 1.]]
Y = [[0.], [1.], [1.], [1.]]

with tf.Graph().as_default() :
    g = tflearn.input_data(shape=[None,2])
    g = tflearn.fully_connected(g,128,activation="linear")
    g = tflearn.fully_connected(g,128,activation="linear")
    g = tflearn.fully_connected(g,1,activation="sigmoid")
    g = tflearn.regression(g, optimizer="sgd", learning_rate=2., loss="mean_square")

    print("setup done", datetime.datetime.now())

    m = tflearn.DNN(g)
    m.fit(X,Y,n_epoch=100, snapshot_epoch=False )
    
    print("fit done", datetime.datetime.now())

    
    #
    print("tttt")

    print("0 or 0:", m.predict([[0., 0.]]))
    print("0 or 1:", m.predict([[0., 1.]]))
    print("1 or 0:", m.predict([[1., 0.]]))
    print("1 or 1:", m.predict([[1., 1.]]))    
    
