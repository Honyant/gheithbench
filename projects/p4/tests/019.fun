print 111111
this = is - to - test - short - circuiting
a = fun {
    print 321
    return 5
}
if(2 < 3 || a(0)) print 123
if(3 < 2 && a(0)) print 234

this = tests - nested - If - and - While - statements
print 222222
if(1) {
    if(1) {
        print 1
        x = argc
        print x
        while(x < 300) {
            if(x % 15 == 0) print x
            x = x + 2
        }
        print x
    }
    else {
        if(0) print 2
        print 3
    }
    if(0) {
        if(1) print 4
    }
    else print 6
    print 7
}
if(0) print 5
else print 6

finally = these - are - some - Function - tests
print 333333
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

print 444444
g = fun print 123
h = fun {
    while(it < 123) {
        if(it % 6 == 0) return it / 6
        it = it + 5
    }
}
k = a, g(0), 1234, (h)(10)
print k

print 555555
l = fun {return it} (10)
print l