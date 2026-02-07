delimiter = 1000000

print argc

print delimiter

basic = operations - tests

a = 0
b = 1

print a < b
print a > b
print a + 1 <= b
print a + 1 >= b
print a || b
print a && b
print a, b
print a + 1 & b + 2
print (a && b) || (a || b)

print delimiter + 1

basic = conditionals

if 100 - 99
    print 5
else
    print 4

odd = 2

if odd % 2
    while 1
        print 0
else 
    print 55

print delimiter + 2

basic = FUNctions

a = fun {
    print it
}

m = a(143)
m = a(a(143))

index = 10

evens = fun {
    if it > 0 {
        print it
        x = odds(it - 1)
    }
}

odds = fun {
    if it > 0 {
        print it
        y = evens(it - 1)
    }
}

m = evens(index)

print delimiter + 3

short = circuiting - and - commas - test
you = should - be - evaluating - all - expressions - within - commas

infinite = fun {
    while 1 {
        print 2024
    }
}

print 0 && infinite(53)
print 1 || infinite(199)

commaValue = 33

finite = fun {
    commaValue = 79
}

print finite(66), 4
print commaValue

print delimiter + 4

RETURNing = FUNctions - and - using - them - as - parameters

a = fun {
    return fun { print 555555 }
}

print a(5)(5)

b = fun {
    return it
}

c = fun {
    print it(6)
}

y = c(b)
