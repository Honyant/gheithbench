
gten = fun {
	if it > 10 return 1
	else return 0
}

if gten(10) print 1
else print 0

print 999999999999999999


x = 0
while x < 3 {
    y = 0
    while y < 3 {
        z = 0
        while z < 3 {
            print x + y + z
            z = z + 1
        }
        y = y + 1
    }
    x = x + 1
}

print 999999999999999999


a = 9
b = 8
c = 11
d = 0
if a < 10 if b < 10 if c < 10 if d < 10 print 4 else print 5 else print 6 else print 7 else print 8

a = fun {
    b = fun {
        c = fun {
            print it
        }
        return 5
    }
}

a = a(0)
b = b(0)
c = c(b)

a = fun {
    b = fun {
        print it % 5
        return it % 5
    }
    print b(it)
    return b(it / 2)
}

print a(52)

comma = testing

f = fun {g = 5}
print f(0), g

print 999999999999999999

short = circuiting

if 0 && b(52) f = 0
if 1 || b(52) f = 0

print 341 & 682
print 341 && 682

recur = test

fac = fun {
    if it <= 1 return 1
    return it * fac(it - 1)
}

print fac(5)

