Hello = and - welcome - to - my - test
This = is - meant - to - be - a - comprehensive - test 
that = goes - in - the - order - of - most - implementations 
so = that - you - can - incrementally - pass - this - entire - test - case
as = you - complete - operators - then - FUNCTIONS - then - IF - then - WHILE

This = test - will - also - test - some - edge - cases - beyond - general - behavior 
because = this - is - FUN - smile - emoji - heart - emoji

delimiter = 1000000000000000

test = that - bitwise - and - works

x = 1
print x

x = x & 2
print x

x = 1 & 3
print x

print delimiter 
btw = this - is - line - 4 - of - ok - file

test = that - relational - operators - work

x = 1 <= 2
print x

x = 1 <= 1
print x

x = 1 < 3
print x

x = 3 >= 3
print x

x = 3 >= 2
print x

x = 3 > 1
print x

x = (3 > 1) >= 1
print x

x = 5 <= 2
print x

x = 5 < 2
print x

x = 2 >= 5
print x

x = 2 > 5
print x

x = (2 > 5) >= 1
print x

x = (5 < 3) <= 1
print x

print delimiter + 1
btw = this - is - line - 18 - of - the - ok - file

test = that - logical - AND - and - OR - work
a = 1
b = 1

print a && b 
print a || b

a = 0

print a && b 
print a || b

a = 1
b = 0
print a && b
print a || b

a = 0
print a && b
print a || b

print delimiter + 2
btw = this - is - line - 27 - of - the - ok - file

test = that - equivalency - works

did = you - know - that - 1729 - is - the - hardy - ramanujan - number
aka = it - is - the - smallest - number - that - can - be - expressed - as - the - sum - of - two - cubes

print 1729 == 1729
print 1720 != 1728
print 1729 != 1729
print a == b 
print a != b
print (3 >= 2) == (5 < 8)

print delimiter + 3
btw = this - is - line - 34 - of - the - ok - file

test = that - comma - operator - works

c = 1, 2, 3
print c
c = 1, 2, a + b 
print c 

test = nested - comma - operator

c = (1, 2), 3, 4
print c
c = ((1, 2), 3, 4)
print c

print delimiter + 4
btw = this - is - line - 39 - of - the - ok - file

test = that - FUNCTIONS - work

f1 = fun {
    print 17 
    return 0
}

print f1(0)

f2 = fun {
    it = 3, 4, it + 5
    return it 
}

print f2(5)

print delimiter + 5
btw = this - is - line - 43 - of - the - ok - file

test = that - nested - FUNCTION - definitions - and - recursion - works
you = will - need - IF - implemented - hopefully - you - do

f3 = fun {
    f4 = fun {
        print it
        f5 = fun {
            print it 
        }        
    }
    if (it > 1) {
        print f3(it - 1)
    } else print it 

    print it + 1729 
    return it
}

print f3(2)

print delimiter + 6
btw = this - is - line - 49 - of - the - ok - file

test = calling - a -FUNCTION - defined - in - another - FUNCTION

print f4(1)
print f5(1)

print delimiter + 7
btw = this - is - line - 54 - of - the - ok - file

test = that - FUNCTIONS - work - with - comma - operator

f = 3, 4, fun print it

print f(1)

test = that - you - can - modify - a - variable - storing - a - FUNCTION 
and = it - will - no - longer - be - equal

copy = f 
f = f - 1 
print copy != f

print delimiter + 8
btw = this - is - line - 58 - of - the - ok - file

test = that - FUNCTIONAL - programming - works

f6 = fun {
    print 17
    print it(0)
}
print f6(copy)

print delimiter + 9
btw = this - is - line - 63 - of - the - ok - file

test = that - IF - works

if 1 print 1 else print 0
if (1) print 1 else print 0
if (1) { print 1 } else print 0
if (1) { print 1 } else { print 0 }

test = that - nested - IF - works

if 1 print 1 if 1 print 2 if 1 print 3

if (1) {
    if (2) {
        if (3) {
            if (4) {
                if 5 if 6 if 6 if 7 if 8 if 9 if 17 if 18 print 10101
            }       
            print 101111
        }
        print 34532
    }
    print 3423
}

print delimiter + 10
btw = this - is - line - 75 - of - the - ok - file

test = that - nested - IF - ELSE - works

if 1 { print 1 if 2 print 2 else print 0 } else print 0

if 1 { print 1 if 0 print 2 else print 7 if 1 print 3 else print 9 } else print 11

if (1) {
    if (2) {
        if (3) {
            if (4) {
                if 5 { 
                    if 6 print 6 
                    else { 
                        if 6 {
                            if 7 print 7 
                            else { 
                                if 8 if 9 if 17 if 18 print 10101 
                            }
                        }
                    } 
                }
            }       
            print 101111
        } else {
            print 141984194812948219
        }
        print 34532
    }
    print 3423
} else {
    print 123497147
}

print delimiter + 11 
btw = this - is - line - 85 - of - the - ok - file

test = that - WHILE - loops - work

w = 0
while (w < 3) && w < 3 {
        print w
        w = w + 1
}

print delimiter + 12 
btw = this - is - line - 89 - of - the - ok - file 

test = WHILE - and - IF - together

this = pRINTS - the - letter - E

line = 1 
while (line <= 5) {
    if line == 1 || line == 3 || line == 5
        print 33333
    else 
        print 3
    line = line + 1
}

test = that - nested - WHILE - loops - work 

i = 0
while (i < 5) {
    j = 0
    while (j < 5) {
        k = 0
        while (k < 5) { 
            print i + j + k
            k = k + 1
        }
        j = j + 1
    }
    i = i + 1
}

print delimiter + 13 
btw = this - is - line - 220 - of - the - ok - file

test = combinations - of - FUNCTIONS - IF - and - WHILE

box = fun {
    row = 1 
    while (row <= it) {
        col = 1
        num = 0
        while (col <= it) {
            if row == 1 || row == it || col == 1 || col == it
                num = num * 10 + 1
            else
                num = num * 10 + 3
            col = col + 1
        }
        print num
        row = row + 1
    }
}

print box(5)
print box(7)

print delimiter + 14 
btw = this - is - line - 234 - of - the - ok - file

test = lowercase - FUN - as - a - variable - name

fun = 3

you = should - not - fail - at - offset - here

test = calling - a - FUNCTION - that - is - defined - later - in - the - file

f77 = fun {
  print f88(0)
}

f88 = fun {
  print 888
}

print f77(0)
