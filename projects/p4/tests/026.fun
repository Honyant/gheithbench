print 1000000000

x = 10
print x == 10
print x == 9
print x <= 10
print x <= 9
print x >= 10
print x >= 11
print x != 9
print x != 10
print x > 9
print x > 10
print x < 11
print x < 10
print x < 11 && x > 9
print x < 11 && x > 10
print x < 11 || x > 9
print x < 11 || x < 10
print x > 11 || x > 9
print x > 11 || x > 10
print 10 & 15
x = 14 - 2, 10 + 2, 12, 10 + 1
print x

print 1000000001

if (x < 9) {
    print 1
} else {
    print 12
}

print 1000000002

x = 7
if (x < 10) {
    if (x > 5) {
        print 1
    } else {
        print 2
    }
} else {
   if (x < 15) {
        print 3
    } else {
        print 4
    }
}
x = 3
if (x < 10) {
    if (x > 5) {
        print 1
    } else {
        print 2
    }
} else {
   if (x < 15) {
        print 3
    } else {
        print 4
    }
}
x = 13
if (x < 10) {
    if (x > 5) {
        print 1
    } else {
        print 2
    }
} else {
   if (x < 15) {
        print 3
    } else {
        print 4
    }
}
x = 17
if (x < 10) {
    if (x > 5) {
        print 1
    } else {
        print 2
    }
} else {
   if (x < 15) {
        print 3
    } else {
        print 4
    }
}

print 1000000003

x = 10
while (x > 0) {
    print x
    x = x - 1
}

x = 2
while (x > 0) {
    y = 5
    while (y > 0) {
        print y
        y = y - 1
    }
    x = x - 1
}

print 1000000004

f1 = fun {
    print 1
    return it
}
print f1(5)

f2 = fun {
    print it
    return it * it
}
print f2(5)

print 1000000005

f3 = fun {
    if (it <= 1){
        return 1
    } else {
        return it * f3(it - 1)
    }
}

print f3(1)
print f3(5)


it = 4
print it

print f3(3)

f3iterative = fun {
    x = it
    result = 1
    while (x > 1) {
        result = result * x
        x = x - 1
    }
    return result
}

print f3iterative(1)
print f3iterative(5)

print 1000000006

print argc

if argc > 3 {
    print 30
} else{
    print 31
}

print 1000000007

print f3(argc)

while argc > 0 {
    print argc
    argc = argc - 1
}

print 1000000008

print 10 % 0
print 10 % 3
print 10 / 0