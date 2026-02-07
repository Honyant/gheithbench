print 000000000
comment = test - inputs - and - method - calling - before - declaration
print argc + 1
print 1 + argc

while(0 && argc) {
    x = shouldNotRunBcShortcircuiting(argc)
} 

shouldNotRunBcShortcircuiting = fun {
    print it
}

simpleMethod1 = fun {
    return simpleMethod2 + argc
}
simpleMethod2 = fun {
    if(it == 200) {
        print it
        return it
    }
    return it + simpleMethod1
}

comment = test - anonymous - calls - with - inputs
print (((simpleMethod1(argc) - argc)(5) - 5)(1) - argc)(200)

thisShouldRun = fun {
    print it
}

print 1111111111
comment = test - unitialized - var
print a < 5
x = 5

comment = test - comparison - ops
print x <= 0
print x < 0
print x > 5
print x >= 10

print x<=5
print x<15
print x>0
print x>=5


print 2222222222
comment = test - comparison - ops
y = 1
print y && 0
print y && 1
print y && 1
print 1 && 1 && 1
print y && 2 && 3
print 0 && shouldNotRunBcShortcircuiting(2222222221)

print 333333333
comment = test - comparison - ops
z = 1
print z || 0
print z || 1
print 0 || 1
print 0 || 0
print z || (z-1)
print 0 || 0 || 0
print 1 || shouldNotRunBcShortcircuiting(333333331)

print 4444444444
comment = test - comma - reassignment
a = 1, 2, 3
a = thisShouldRun(4444444441), 3
print a

print 5555555555
comment = test - bitwise
a = 10
b = 7
print (a & b)

print 6666666666
comment = test - equality
print (a == b)
print (a != b)
print (a==b) != (b==a)

print 7777777777
comment = test - basic - conditional
a = 5
b = 0

if (a > 0) {
    print a
}

if a > 0 {
    print a
}

if a > 0  print a

if (a > 0) {
    print a
} else {
    print b
}

if a > 0 {
    print a
} else {
    print b
}

if (a < 0) {
    print a
} else {
    print b
}

if a < 0 {
    print a
} else {
    print b
}

if a < 0
print a
else 
print b

if(a>0) {
    if(b==0) {
        print b
    }
} else {
    print a
}

print 8888888888
a = 0
print a
while ( a < 3) {
    a = a + 1
    print(a)
}
print a

comment = nested - loop - and - potential - variable - names
main = 5
exit = 10
while( exit > 0) {
    while( main > 0) {
        print main
        main = main - 1
    }
    exit = exit - 1
}
print main
print exit
