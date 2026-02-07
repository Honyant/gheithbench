comment = testing - same - fnctionalities - as - p2- with- modular- testing

comment = basic- operations- with- different- var- naming

var1 = 1
VAR2 = 2
vA3r = 3
var4 = 4

print var1 + VAR2
print var4 - vA3r
print var4 * vA3r
print var4/VAR2
print var4 % vA3r

comment = edge- case - division - rounding

print var4/vA3r

comment = arithmetic - with - lots - of - brackets

print (2005 - (50-(9*(40/(17 % 9)))))

comment = bitwise- operators- and- comma

var6 = 10
var7 = 15
var8 = 38
var9 = 61
var10 = 81, 91, 100

print var6 & var7
print var10

comment = and - or

var11 = 0
var12 = 1

print var11 && var12
print var11 || var12

comment = if - else - statements -with -brackets

if 2 > 3 {
    print 0
} else {
    print 1
}

comment = if - else - statements -without -brackets

if 3 < 4
    print 0
else
    print 1

comment = if - else - statements - mixed- brackets

if 5 < 6
    print 0
else {
    print 1
}

comment = just- false - if  - statement

if 0 > 1
    print 5

comment = nested - if - else - no - brackets

if (5 +3 == 8) {
    print 1
    if (3 + 4 ==76)
        print 2
    else
        print 3
}
else    
    print 0

comment = stress - test- if- statement

if ((1 >= 2) || (3 == 4) || (5 > 6) || (7 == 8) || (9 != 9) || (11 > 12) || (13 == 14) || (15 >= 16) || (17 > 18) || (19 == 20) ||
(21 != 21) || (26 < 24) || (25 == 26) || (27 != 27) || (29 > 30) || (39 <= 32) || (33 != 33) || (35 > 36) || (37 == 38) || (40 != 40) ||
(41 > 42) || (45 <= 44) || (45 != 45) || (47 > 48) || (49 == 50) || (51 != 51) || (53 > 54) || (57 <= 56) || (57 != 57) || (59 > 60) ||
(61 >= 62) || (63 == 64) || (65 > 66) || (67 == 68) || (69 != 69) || (71 > 72) || (73 == 74) || (75 != 75) || (77 > 78) || (81 <= 80) ||
(81 != 81) || (83 > 84) || (85 >= 86) || (88 != 88) || (89 > 90) || (91 >= 92) || (93 != 93) || (95 > 96) || (97 >= 98) || (99 != 100 && 100==100)) {
    print 500
}

comment = while - loops

var13 = 7

while var13 > 0 {
    print var13
    var13 = var13 - ((6+7) % 12)
}

comment = long - while - loop - to - evaluate

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

comment = testing, if, false, expression, while, is, skipped

while 2>=3
    print 0

comment = testing, if, while, statement, is, 0, is, skipped

while 0
    print 1

comment = basic - fnctions 
fnc = fun {
    return it
}

print fnc(33)
print fnc(2)

comment = if- else - bracket- egde - case

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

comment = while - loop - bracket- edge - case- with - if - outside

a = 1, 2, 3
b = a, 4, 5

if a > 0 {                       
    while b < 10 {
        print a * b
        b = b + 1 
        }
} else           { 
    print 1
}

comment = more- fnctions

brackets = fun {
        it = it + 1
        return it
}

print brackets(1)

comment = fnctions - inside - fnction

settingfunc = fun {
    return it*it
}

fnc2 = fun {
    it = settingfunc(it)
    if (it % 2 == 0)
        return it * 2
    else
        return it
}

print fnc2(3)
print fnc2(4)

comment = recusive- fnction - calls

factorial = fun {
    if it == 1 || it==0 {
        return 1
    }
    return it * factorial(it - 1)
}

print factorial(5)

comment = variables- named- it- outside- fnc

it = 3
print it
print factorial(4)
print it

comment = testing - argc - value
print argc

argc = 8
print argc + 1

comment = nested- fnction- calls
print factorial(fnc2(fnc(2)))