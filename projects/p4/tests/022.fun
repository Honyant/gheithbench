print argc

first = we - test - basic - operators
print 100000
    zero = 0
    one = 1

    a = 1 
    b = 2 
    c = 3
    d = 3
    e = 0

    print a + b
    print c - b
    print b / b
    print a * c
    print (c * d) / b

print 100001
    print c > a
    print b < a
    print c < d
    print d >= c
    print d <= c
    print c == c
    print a == b
    print a != a
    print c != d
    print a != d
    print b == d

print 100002
    print zero && one
    print zero || one
    print one || one
    print zero || e
    print d && c
    print (zero && one) || e
    print (zero && one) || one
    print zero && (one || a)

more = complex - comparisons
print 100003
    print (b - a) == (b != d)
    print (c - b) >= (a * e)
    print ((d == a) + a) <= (c * d > b)

print 100004
    print ((a, c), b)
    print a, (d, e), b

print 100005
    f = 27
    g = 33
    h = 144
    i = 98

    print f & g
    print f & h
    print f & i

simple = order - of - operations
print 100006
    print 3 * (4 + 5)
    print 6 * (7 % 5)
    

some = more - complex - order - of - operations
print 100007
    print (f, (g, h)) & i
    print a, b, c * h
    print a & b + c / d & f, g, h
    print h, g, a & b + c / d & f
    
    print d * (b + 
        c & f, g, 
             h) / 
          i

now = we - test - if - and - else - loops - with - less - structure - in - later - tests
print 100008
    if (f < 30) {
        print 555
    }

    if (f < 30) {
        print 555
    } else {
        print 666
    }

    if (h < 30) {
        print 777
    } else {
        print 666
    }

    if f < 30 {
        print 555
    }

    if f < 30 print 555

    if h < 30 print 777 else print 666

    if 
                g < 40
            print 777
                    else
        print 555

nested = ifs
print 100009
    if (f < 30) {
        print 111
        if(f > 20) {
            print 222
        }
    }

    if (f < 30) {
        print 111
    } else {
        if(f > 20) {
            print 222
        } else {
            print 333
        }
        print 444
    }

    if (f > 30) {
        print 111
    } else {
        if (f > 20) {
            print 222
        } else {
            print 333
        }
        print 444
    }

    if (f < 30) {
        if (f > 20) {
            if (1) {
                if 1 {
                    if 1 if 1 if 1 if 1 if 1 print 1234
                }
            }
        }
    }

    if i > 100 print 555 else if i < 50 print 666 else print 777

    x = 6
    if (x > 5) {
        if (x > 7) {
            print 11
        } else {
            print 10
        }
    } else {
        if (x < 2) print 10
        else print 4
    }
    y = 7
    if (y < 6) {
        print 66
    } else {
        if (x > 3 || y == 2) {
            print 88
        } else {
            print 99
        }
        print 77
    }



now = we - test - while - loops
print 100010
    aa = 0
    while (aa < 5) {
        print aa
        aa = aa + 1
    }

    while aa <= (5 * 5 * 5 * 5) {
        print aa
        aa = aa * 5
    }

    while aa > 100 aa = aa - 5
    print aa

nested = while - loop
    aa = 1
    while (aa < 5) {
        bb = 1
        while (bb < 2) {
            print bb
            bb = bb + 1
        }
        print aa
        aa = aa + 1
    }

now = we - will - test - basic - vunctions
print 100011

    f1 = fun {
        return (it * 2) + 3
    }
    print f1(3)

calling = vunction - inside - vunction
    f2 = fun {
        x = f1(it)
        return x + 4
    }
    print f2(2)

special = out - of - order - case - with - vunctions
    f3 = fun {
        x = f4(it)
        return x - 2
    }

    f4 = fun {
        j = it + 3
        return j
    }
    
    print f3(5)

recursive = vunctions
    factorial = fun {
        if (it == 0) {
            return 1
        }
        return it * factorial(it - 1)
    }
    print factorial(4)

nested = vunctions 
    nestt = fun {
        print it
        other = fun {
            print 2 * it
        }
    }

    a = nestt(1)
    b = other(2)

now = we - will - test - short - circuting - in - logical - ops
print 100012
    print 1 || undefined(3)
    print 0 && otherUndefined(4)

now = we - will - test - handling - large - values
look = at - your - mov - instruction - for - this
print 100013
    print 99999999
    print factorial(10)

print 100014
other = edge - cases
    while (argc > 0) {
        print argc
        argc = argc - 1
    }

    print 3 / 0