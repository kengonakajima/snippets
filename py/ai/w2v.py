import gensim

# Load your text data.
data = ["I love natural language processing!", "word2vec is a great technique!"]

# Train a word2vec model on the data.
model = gensim.models.Word2Vec(data, vector_size=100, window=5, min_count=1, workers=4)

# You can then access the word vectors for each word in the vocabulary using the word2vec model.
#print(model['natural'])
#print(model['great'])

model.save("word2vec.model")

