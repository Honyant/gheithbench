DELIMETER = 999999999999
MINI = 9999999

LETS = TEST + EACH + OPERATOR 
print 1 < 2
print 2 < 1
print 1 < 1

print 1 > 2
print 2 > 1
print 1 > 1

print MINI

print 1 == 1
print 2 == 1
print 1 != 1
print 1 != 2

print MINI

print 1 >= 1
print 1 >= 2
print 2 >= 1

print 1 <= 1
print 1 <= 2
print 2 <= 1

print MINI

print 1 & 0
print 18 & 3
print 1 && 0 
print 16 && 3
print 1 || 0
print 16 || 3

print MINI

print 1, 3
print 1, 2, 3, 4, 5, 6

print DELIMETER 





NOW = FOR + IF + STATEMENTS

if 2 == 3 {
    print 1
}

if 1 {
    print 2
} else {
    print 3
}

if 0 {
    print 4
} else {
    if 1 {
        if 0 {
            print 5
        } else {
            print 6
        }
    } else {
        print 7
    }
}

print DELIMETER 




WHILE = LOOP + TIME

while (0) {
    print 0
}

a = 1
while (a < 5) {
    print a
    a = a + 1
}

i = 0
a = 0
while (i < 10) {
    j = 0
    while (j < 10) {
        a = a + 1
        j = j + 1
    }
    i = i + 1
}
print a

print DELIMETER




NOW = WE + GET + TO + DO + FUNCTIONS

a = fun { return it }

if (a(1001) != 1001) {
    print 1
}

b = fun { return a }
c = fun { return b }
if (c(0)(0)(100) != 100) {
    print 2
}

sumDigits = fun {
    if it == 0 {
        return 0
    }
    return (it % 10) + sumDigits(it / 10)
}

if (sumDigits(1234) != 10) || (sumDigits(sumDigits(12345)) != 6) {
    print 3
}

doubler = fun {
    f = it
    return fun { return 2 * f(it) }
}
if (doubler(sumDigits)(1234) != 20) {
    print 4
}

it = 100
sumNumbers = fun {
    out = 0
    while (it > 0) {
        out = out + it
        it = it - 1
    }
    return out
}
if (sumNumbers(5) != 15) {
    print 5
}
if (it != 100) {
    print 6
}
retTest = fun {
    i = 0
    while (i < 100) {
        i = i + 1
        if i == it {
            return i
        }
    }
    return i
}
if retTest(20) != 20 {
    print 7
}
if retTest(101) != 100 {
    print 8
}
print DELIMETER




OK = NOW + TIME + FOR + SOME + MISC + STUFF
LIKE = SHORT + CIRCUITING
a = fun {
    print it 
}
b = 0 && a(1)
c = 1 || a(2)

LETS = MAKE + SURE + LHS + OF + COMMA + IS + EXECUTED
d = a(3), 10

NESTED = LOOPS + AND + IF + STATEMENTS + JUST + CUZ
a = 0
while (a < 100) {
    b = 0
    while (b < 100) {
        if (a == b) {
            if (a == 50) {

            } else {
                if (a == 25) {
                    print 4
                }
            }
        } else {

        }
        b = b + 1
    }
    a = a + 1
}

DONT = FORGET + ORDER + OF + OPERATIONS
print 2, (1 <= 2) * 3 & 7 + 5 - (6 && 7 || 8)