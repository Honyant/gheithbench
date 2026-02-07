delimiter = 100000000000000000

TEST1 = CHAIN-OPERATORS
print 1 * delimiter
print 0 && 1 && 1
print 0 || 0 || 1 || 0
print 7 & 3 & 2
print 3 == 5 || 1 != 2 && 3 & 2
print 3 < 2 < 1
print 1, 2, 3

TEST2 = NESTED-ELSE-IF
print 2 * delimiter
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

TEST3 = WHILE-LOOP-OVERFLOW
print 3 * delimiter
num = 3
count = 1
while (count <= 10) {
    print num
    num = num * num
    count = count + 1
}

TEST4 = GLOBAL-IT-WITH-FUNCTIONS
print 4 * delimiter
it = 5
f1 = fun {
    it = 3
    return it == 6
}
print it

TEST5 = NESTED-FUNCTIONS-CALLS
print 5 * delimiter
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

TEST6 = FUNCTION-ARITHMETIC
print 6 * delimiter
f1 = f1 + f2 + 5
print (f1 - f2 - 5)(1)

TEST7 = FUNCTION-PARAMETER
print 7 * delimiter
a = fun {
    return it(it) == it + 1
}
b = fun {
    return fun { return it + 1 } (it)
}
print a(b)

TEST8 = CALLING-FUNCTIONS-WITH-FUNCTIONS
print 8 * delimiter
getid = fun {
    return it
}
f1 = fun {
    return 1
}
print getid(f1)(0)

TEST9 = RETURN-IN-THE-MIDDLE-OF-FUNCTION
print 9 * delimiter
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
