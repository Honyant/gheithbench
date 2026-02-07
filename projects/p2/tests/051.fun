comment = testing, most, binary, operators

if 1 == 1 && ((4 <= 5 && 6 >= 7) || 2 != 3) {
    print 42
}

comment = testing, nested, if, else, statements
a = 3
b = 3
c = 3
d = 2
e = 5
f = 6

if a == b {
    if c != d {
        print 1
    } else {
        print 2
    }
} else {
    if e < f {
        print 3
    } else {
        print 4
    }
}

comment = testing, basic, while, loop
num = 10
while num > 0 {
    print num
    num = num - 1
}

comment = testing, return, and, recursive, functions

factorial = fun {
    if it == 1 || it==0 {
        return 1
    }
    return it * factorial(it - 1)
}

print factorial(5)

comment = testing, function, called, fun, and , basic, return
comment = not, running, however, since, it, would, be, invalid

fun = fun {
    if it > 0 {
        return it * 2
    } else {
        return it * 3
    }
}

comment = testing, nested, function, call

add = fun {
    return it + 1
}

multiply = fun {
    return it * 2
}

result = multiply(add(5))
print result

comment = testing, comma, operator
a = 1, 2, 3
b = a, 4, 5
print b

comment = full, logical, operator, test, except, bitwise
if (a > 1 && b <= 5) || (c == 3 && d != 4) {
    print 42
}

comment = testing, if, while, else, statement, combination
if a > 0 {
    while b < 10 {
        print a * b
        b = b + 1
    }
} else {
    print 1
}

comment = testing, if, without, brackets

if 3<=4
   print 1
else
   print 0

comment = testing, while, without, brackets

var = 5

while var>0
   var = var - 1

print var

comment = testing, incorrect, if, statement, skipped, to, else
if 2>3
    print 0
else
    print 1

comment = testing, only, false, if, block, is, skipped
if 2>3
    print 0

comment = testing, if, false, expression, while, is, skipped

while 2>=3
    print 0

comment = testing, if, while, statement, is, 0, is, skipped
while 0
    print 1

comment = bitwise, and, test
bitwise = 212&167
print bitwise

comment = testing, if, with, crazy, brackets, edge, case
comment = also, includes, all, binary, operator, testing

if 2<3 {
    var1 = 7324654
    print var1
    if 3<= 4             {
        var2 = 9774744
        print var2
        if 5>4                    {
            var3 = 6438264
            print var3
            if 6>= 5              {
                var4 = 25435354
                print var4
                if 7<8 && 8<9     {
                    var5 = 8487543
                    print var5
                    if 8>9 || 9<10         {
                        var6 = 6345528
                        print var6
                        if 10==10        {
                            var7 = 5234556
                            print var7
                            if 11 != 12        {
                                var8 = 1486354
                                print var8
                            }
                        }
                    }
                }
            }
        }
    }
}

comment = while, stress, test, for, storage, stress
comment = tons, of, variable, overriding

i = 10000
j = 452636
k = 87679

while i>0 {
    i = i - 1
    j = j - 1
    j = j + i
    k = k - 1
    k = j + k
}

print i
print j
print k

comment = while, with, crazy, nested, if, for, time, stress, test

m = 100

while m > 0 {
    if ((1 >= 2) || (3 == 4) || ((m- 82) > 6) || (7 == 8) || (9 != 9) || (11 > 12) || (13 == 14) || (15 >= 16) || (17 > 18) || (19 == 20) ||
    (21 != 21) || (26 < 24) || (25 == 26) || (27 != 27) || (29 > 30) || (39 <= 32) || (33 != 33) || (35 > 36) || (37 == 38) || (40 != 40) ||
    (41 > 42) || (45 <= 44) || (45 != 45) || (47 > 48) || ((m + 1) == 50) || (51 != 51) || (53 > 54) || (57 <= 56) || (57 != 57) || (59 > 60) ||
    (61 >= 62) || (63 == 64) || (65 > 66) || (67 == 68) || (69 != 69) || ((m/2) > 72) || (73 == 74) || (75 != 75) || (77 > 78) || (81 <= 80) ||
    (81 != 81) || (83 > 84) || (85 >= 86) || ((m- 20) != 88) || (89 > 90) || (91 >= 92) || (93 != 93) || (95 > 96) || (97 >= 98) || (99 != 100 && 100==100)) {
        print m
    }
    m = ((((m*2)/2) + 2) - 2) - 1
}

comment = very, long, expression, to, evaluate

if ((1 >= 2) || (3 == 4) || (5 > 6) || (7 == 8) || (9 != 9) || (11 > 12) || (13 == 14) || (15 >= 16) || (17 > 18) || (19 == 20) ||
(21 != 21) || (26 < 24) || (25 == 26) || (27 != 27) || (29 > 30) || (39 <= 32) || (33 != 33) || (35 > 36) || (37 == 38) || (40 != 40) ||
(41 > 42) || (45 <= 44) || (45 != 45) || (47 > 48) || (49 == 50) || (51 != 51) || (53 > 54) || (57 <= 56) || (57 != 57) || (59 > 60) ||
(61 >= 62) || (63 == 64) || (65 > 66) || (67 == 68) || (69 != 69) || (71 > 72) || (73 == 74) || (75 != 75) || (77 > 78) || (81 <= 80) ||
(81 != 81) || (83 > 84) || (85 >= 86) || (88 != 88) || (89 > 90) || (91 >= 92) || (93 != 93) || (95 > 96) || (97 >= 98) || (99 != 100 && 100==100)) {
    print 500
}
