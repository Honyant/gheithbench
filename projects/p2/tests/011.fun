
fnctions = returning - fnctions

callback = fun {
    x = it
    return fun 
        return x * it
}

res = callback(13)
print res(2)
x = 12
print res(2)
print callback(14)(2)

inline = fnctions
print fun {return 2 * it} (10)


short = circuiting
x = 3
y = 4
print x && y
print x || y
x = 0
print x || y
print x && y

if x || y == 4 {print y} else print x

comma = operator
x = 3, 4, 5
fct = 1
while (x) {
    fct = fct * x
    x = x - 1
}
print fct

y = 3, 5, fun return it * fct
print y(2)

bitmasks = test - complete - test - of - operations - and - fnctions - and - recursion
pow = fun {
    if it == 0 return 1
    return 2 * pow(it-1)
}

t = pow(6)
s = 0
while (s < t) {
    x = 0
    sm = 0
    while (x < 6) {
        if s & pow(x) sm = sm + x
        x = x + 1
    }
    print sm
    s = s + 1
}



