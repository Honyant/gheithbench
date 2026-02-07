testcase = test + equality + operators

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


testcase =  if + else

if (x < 9) {
    print 1
} else {
    print 12
}

testcase = nested + if + else

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

testcase = while + nestedwhile

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

testcase = basic + fnctions

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

testcase = recursive + fnctions

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