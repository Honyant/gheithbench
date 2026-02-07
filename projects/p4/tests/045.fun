argcTest = 1
print(argc)

divisionByZeroTest = 1
print(1/0)
print(21/0)
print(2123/0)
print(a/0)
print(b/0)
print(c/0)
print(0/0)
print(0/4)

moduloByZeroTest = 1
print(4%0)
print(0%0)
print(0%4)

moreShortCircuitTesting = 1
print(1&&(a+b+c+d))
print(0||(a*b*a*a*a*a+3+31))

nestedWhile = 1
a = 1
b = 1
c = 1

while a {
    while b {
        while c {
            a = 0
            b = 0
            c = 0
        }
    }
}         


operatorTest = 1
a = 6
b = 1
c = 2
d = 4
e = 3
f = 5
g = a <= b >= c < d
h = e > f == f < e
i = g != h
j = d == e || d ==b || b!=c && b != e
l = a || d && g & 12 & 15
co = a+d, c+3, j + 1
print co
co = ((b+c*d-d), d+e+x, 3, 4), a * 7
print co
print g
print h
print i
print j
print l



shortCircuitTest = 1
a = 1 || 0
print a

andOperatorTest = 1
b = 4 & 5 && 6 & 7 && 8
print b


differentStylesOfIfElseTest = 1
a = 1
b = 0
if a
    print c
if b
    print d
if b {print d}
else {print e}
if a {
    if b != a {
        if a{
            if b > a{
                print c
            }
            else {
                print a
            }
        }
        else {
            a = b
        }
    }
    else {
        print f
    }
}


differentWhileTests = 0
a = 4
while (a < 20)
    a = a * 2
print a

while (a % 50 != 0) {
    a= a + 3
}
print a

basicFunctionTest = 1
womp = fun {
    inner = it
    return inner & inner
}
wompwomp = womp(14)
print wompwomp
wompwomp = womp(womp(24))
print wompwomp

embeddedFunctionTest = 1
happy = fun {
    sad = fun {
        return it*it-it*4
    }
    while (it > 0-6 && it < 96) {
        it = sad(it)
        sad = fun {
            return it*it+it*4
        }
    }
    return it
}

a = happy(6)
weirdSyntaxFunctionTest = 1
unhappy = fun {
    return it+1
}
sounhappyrn = unhappy(unhappy((4))) - unhappy + unhappy
print sounhappyrn
