test = testing + operators + and + ifs

x = 5
y = 7

test = comparisons + that + are + true

if (x != y) {print 0}

if x < y print 1

if (x <= y) print 2

test = above + continued + and + brackets

if (y > x) {
    print 3
}

if y >= x {
    print 4
}

test = comparisons + that + are + false + and + elses + with + varying + brackets

if (x > y) { print 55 }
else print 5

if y < x print 66
else { print 6 }

if x >= y {
    print 77
} else {
    print 7
}

if (y <= x) print 88
else print 8

if y==x print 99
else print 9

comment = above + is + 0 - 9
test = logical + operators + both + true + and + false + for + each

a = 1
b = 0

if a & b {
    print 101010
}


else {
    print 10
}

if (a && b) {
    print 111111
} else print 11

if (a || b) print 12 else {print 121212}

b = 1

if (a&b) print 13

if (a &&b) print 14

a = 0
b = 0

if a || b print 151515
else print 15

test = while + loops

x = 16
while x < 20 {
    print x
    x = x + 1
}

comment = above + outputs + 10 - 19

test = skip + while + loop

while x < 10 {
    print x
    x = x-1
}

test = FUNCTIONS

a = fun {
    it = it * 2
    return it
}

y = 10
y = a(y)
print a(10)
print y + 1

test = ifs + and + whiles + in + FUNCTIONS

f1 = fun {
    if it >= 20 return it + 2
    else {
        if it < 10 {
            return it -2
        }
        else return it * 2
    }
}

b = f1(11)
print b

a = 21
a = f1(21)
print a

f2 = fun {
    while it < 30 {
        print it
        it = it + 1
    }
}

a = f2(24)

above = outputs + 20 - 29

test = recursive + FUNCTION

f3 = fun {
    if (it == 1) {
        return it
    }
    else return (it + f3(it-1))
}

b = f3(5)
print b*2

last = outputs + 30