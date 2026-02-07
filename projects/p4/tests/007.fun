below = p2 + test + case
after = there + is + new + p4 + stuff

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

comment = above + outputs + 0-9

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

comment = above + outputs + 10-19

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

above = outputs + 20-29

test = recursive + FUNCTION

f3 = fun {
    if (it == 1) {
        return it
    }
    else return (it + f3(it-1))
}

b = f3(5)
print b*2

above = outputs + 30

p2 = test + case + done

below = new + things + to + test

print 9999999999999

argcShould = 31
is = argc + correct
print argc

variable = names + used + in + arm + program
fmt = 1
print fmt

ldr = 2
print ldr

is = this + legal
since = start + and + main + have + underscores + already
and = fix + for + vars + interfering + with + arm + is + underscores

main = 3
print main

start = 4
print start

ret = start + 1
print ret

x1 = main * 2
print x1

sp = start * 2
print sp - 1

think = thats + enough + of + that + kind

more = comprehensive + testing

a = fun {
    print it
    it = it * 1000
    print it
    return it
}

FUNction = in + loop
i = 0
while (i < 5) {
    bye = a(i)
    i = i + 1
}

b = fun {
    print it
    it = it * 10000
    print it
    return it
}

test = FUNCTION + call + in + other + FUNCTION

c = fun {
    if it<100 bye = b(it)
    else bye = a(it)
}

y = c(99)
print y

x = c (105)
print (x)

if y<x print 404
else print y-x

x = 5

bye = c(x)

dont = think + changing + it + inside + FUNCTION + changes + variable
print x