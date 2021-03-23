def generator(n):
    for i in range(n):
        yield i

g=generator(100)

#for x in g:
#    print(x)


poo=(
    "hoge"
    for x in g
)

print(poo)
for z in poo:
    print(z)
