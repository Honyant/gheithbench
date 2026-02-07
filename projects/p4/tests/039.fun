TESTS = ARGCVARIABLES
print argc

TESTS = ILLEGAL - VARIALBE - NAMES
exit = 0
main = 0
fmt = 0
print exit + printf + main + fmt + 1

COOL = TOWER - OF - HANOI
towerofhanoi = fun {
    if it == 1 return 1
    return 2 * towerofhanoi(it-1) + 1
}
print towerofhanoi(10)


delimiter = 99999999
print delimiter
TESTS = BASIC - IF - STATEMENTS
x = 40
if x > 20 print x
else print 0
if x == 40 && 1 {
    x = 20
    print x
}
else {
    x = 40
    print x
}
if 60
    print 30


print delimiter
TESTS = COMMAS
x = 40,20,30
print x


print delimiter
TESTS = WHILE - LOOPS
while x >= 30 && x / 2 <= 20 
    x = x + 1
print x


print delimiter
TESTS = FUNCTION - CALLING
f1 = fun {
    return it + 1
}
print f1(2)

recurse = fun {
    if it == 0
        return 20
    print it
    return recurse(it - 1)
}
print recurse(3)

print delimiter
TESTS = RECURSION - BETWEEN - TWO - FUNCTION
f1 = fun {
    print it
    return f2(it*5)
}

f2 = fun {
    print it
    if it == 20
        return 1
    return f1(it / 5 + 1)
}
print f1(2)


print delimiter
TESTS = INNER - AND - OUTER - FUNCTIONS
f1 = fun {
    f2 = fun {
        print it
        return it + 1
    }
    print f2(it+3)
    return it-2
}
print f1(1)


print delimiter
TESTS = ANONYMOUS - FUNCTIONS
print fun {print it+2 return it-2}(0)
print fun {
    return 1 + 
        fun {
            return it + 2
         } (it) 
} (2)


print delimiter
TESTS = SELF - DESTRUCTING - FUNCTIONS
f1 = fun print it
fnum = fun {fnum = 99}
print fnum(fnum), f1(30), fnum


print delimiter
TESTS = BINARY - OPERATORS
print 3 & 3 & 1


print delimiter
TESTS = EQUALITY - CHAINING - AND - SHORT - CIRCUITING
thisShouldNotRun = fun {print 2}
if 3 >= 2 == 1 || thisShouldNotRun(3)
    print 43
if 0 && thisShoudNotRun(4)


print delimiter
TESTS = FUNCTION - QUIRKS
f1 = fun {
    print it+it*it
    return it / it
}
print (((1*f1-1+1))(20))


print delimiter
TESTS = FUNCTIONAL - PROGRAMMING
f1 = fun {
    if 1
        return f2(3)
    return 0
}
f2 = fun {
    print it + 5
    return it - 2
}
print f1(f2)


print delimiter
COOL = TRIBONNACI - SEQUENCE
tribonnaci = fun {
    if it == 0
        return 0
    else if it == 1
        return it
    else if it <= 2
        return 1
    return tribonnaci(it-1) + tribonnaci(it-2) + tribonnaci(it - 3)
}
print tribonnaci(6)



