print 111111
a = ((2 * (8 - 3))) / 5 + 10 & (7) - (10 == 15)
print a
b = 10 < 5 && (6 >= 6 || a % 2 == 0) && 2 <= 10 && 3 < 8
print b
print (a * b) + 1

print 222222
if(b) 
    print 50
else 
    print 100

if a || b {
    a = a + b * 2
    b = b + 5
}
print a
print b

print 333333
while ((c < 20)) {
    c = c + 1
    if c % 2 == 0 print c
}
print c

print 444444
d = fun {
    while(it < 100)
        it = e(it)
    return it
    print 29
}

e = fun return 5 * it
f = d(it + 2)
print f
print it

print 555555
g = fun print 123
h = fun {
    while(it < 123) {
        if(it % 6 == 0) return it / 6
        it = it + 5
    }
}
k = a, g(0), 1234, (h)(10)
print k

print 666666
l = fun {return it} (10)
print l