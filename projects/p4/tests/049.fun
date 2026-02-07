abc = fun {
    it = it + 5
    return it
} (5)

print abc

print 1000000

if argc {
    print argc
    if (argc - 2) {
        print argc - 2
        if (argc - 5) {
            print argc - 5
            if (argc - 12) {
                print argc - 12
            }
            else {
                print 369
            }
        }
    }
}

print 1000001

a = 5, 3, 4

b = fun {
    a = a + it
    return a
}

a = b(3)
result = 0
while a > 1 {
    result = a + result
    g = 2
    while g > 0 {
        a = a - 1
        g = g - 1
    }
}

print result

print 1000002

a = 7

if 12 || b(20) * 100 {
    print a
}

print 1000003

if 1 && b(0) < 100 {
    print a
}

print 1000004

if 5 > a && 12 < 14 < 45 > 42 > 43 > 27 && b(55) < 1000 {
    print 100
} 
else {
    print a
}

print 1000005

c = fun {
    if it == 1
        return 1
    return it * c(it - 1)
}

print c(5)

print 1000006

print a > 11
print a < 11

print 1000007

print 58 & 122

print 1000008

print argc

print 1000009

argc = argc + 1
print c(argc)
