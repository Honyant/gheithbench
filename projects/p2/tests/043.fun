if 1 < 2 > 3 {
    print 12
}
else {
    print 13
}

if 1 < 2 < 3 {
    print 14
}
else {
    print 15
}

a = 5, 3, 4

b = fun {
    a = a + it
    return a
}

if 2 < 1 || 1 
    print a

if 1 || b(3) {
    print a
}

if 1 && b(3) < 10 {
    print a
}

if 1 < a && 3 < 4 < 5 > 4 > 3 > 2 && b(3) < 10 {
    print 100
} 
else {
    print a
}

c = fun {
    if it == 1
        return 1
    if it == 0
        return 0
    return it + c(it - 1)
}

print c(5)

print a > 5
print 13 & 29
