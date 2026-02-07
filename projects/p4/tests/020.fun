delimiter = 100000000000000000

print delimiter + 1
TEST1 = RESERVED-LABELS
main = 1
print main
fmt = 2
print fmt

print delimiter + 2
TEST2 = ARGC-ASSIGNMENT
print argc
argc = 5
print argc

print delimiter + 3
TEST3 = MOD-ZERO
print 3 % 0

print delimiter + 4
TEST4 = VARIABLE-BEFORE-DEFINITION
print x
x = 5

print delimiter + 5
TEST5 = NO-RETURN-IN-FUNCTION
f1 = fun {
    print 2
}
print f1(3)

print delimiter + 6
TEST6 = PRESERVE-LOCAL-ARG
f1 = fun {
    print f2(it)
    return it
}
f2 = fun {
    it = it + 5
    return it
}
print f1(5)

print delimiter + 7
TEST7 = CHAIN-OPERATORS
print 0 && 1 && 1
print 0 || 0 || 1 || 0
print 7 & 3 & 2
print 3 == 5 || 1 != 2 && 3 & 2
print 3 < 2 < 1
print 1, 2, 3

print delimiter + 8
TEST8 = NESTED-ELSE-IF
x = 3
if x > 2
    if x < 3
        print 1
    else if x > 3
        print 2
    else
        print 3
else
    print 4

print delimiter + 9
TEST9 = WHILE-LOOP-OVERFLOW
num = 3
count = 1
while (count <= 10) {
    print num
    num = num * num
    count = count + 1
}

print delimiter + 10
TEST10 = GLOBAL-IT-WITH-FUNCTIONS
it = 5
f1 = fun {
    it = 3
    return it == 5
}
print f1(it)
print it

print delimiter + 11
TEST11 = NESTED-FUNCTIONS-CALLS
f1 = fun {
    it = it + 1
    return it
}
f2 = fun {
    it = it + 2
    return it 
}
f3 = fun {
    it = it + 3
    return it 
}
print f1(f2(f3(1)))

print delimiter + 12
TEST12 = FUNCTION-ARITHMETIC
f1 = f1 + f2 + 5
print (f1 - f2 - 5)(1)

print delimiter + 13
TEST13 = FUNCTION-PARAMETER
a = fun {
    return it(it) == it + 1
}
b = fun {
    return fun { return it + 1 } (it)
}
print a(b)

print delimiter + 14
TEST14 = CALLING-FUNCTIONS-WITH-FUNCTIONS
getid = fun {
    return it
}
f1 = fun {
    return 1
}
print getid(f1)(0)

print delimiter + 15
TEST15 = RETURN-IN-THE-MIDDLE-OF-FUNCTION
x = 0
f1 = fun {
    it = 1
    x = it + f2(x)
    return x
    x = x + 1
}
f2 = fun {
    it = it + 1
    return it
}
print f1(x)
print x
