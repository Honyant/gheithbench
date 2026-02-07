comment = testing - argc - with - operators

print argc % 0
print argc / 0
argc = argc + 2
print argc % 4
print argc / 4
print argc & 123
print argc && 123

comment = testing - argc - with - comparison - operators

print argc < argc
print argc <= argc
print argc == argc
print argc > 0

comment = weird - situation - with - unsigned - integers

x = 0 - 5
print x > 5

print 999999999999999901

comment = collatz - conjecture - thing

while argc != 1 {
    if argc % 2 == 0 argc = argc / 2
    else argc = 3 * argc + 1
    print argc
}

print 999999999999999902

comment = nested - whil - loops

while argc < 3 {
    y = 0
    while y < 3 {
        z = 0
        while z < 3 {
            print argc + y + z
            z = z + 1
        }
        y = y + 1
    }
    argc = argc + 1
}

print 999999999999999903

comment = nested - ifq - elseq - statements

a = 9
b = 8

d = 0
if a < 10 if b < 10 if argc < 3 if d < 10 print 4 else print 5 else print 6 else print 7 else print 8

a = fun {
    b = fun {
        print it % 5
        return it % 5
    }
    print b(it)
    return b(it / 2)
}

print a(52)

comment = comma - testing

f = fun {g = 5}
print f(0), g

print 999999999999999904

comment = short - circuiting - with - argc

if 0 && b(52) f = 0
if argc || b(52) f = 0

print 341 & 682
print 341 && 682


comment = recursion - with - argc - to - test - link - register - and - local - variable - and - frame - pointer - manipulation


fibonacci = fun {
    if it <= 1 return it
    return fibonacci(it-1) + fibonacci(it-2)
}

argc = argc + 3

print fibonacci(argc)


comment = check - ifq - you - can - still - do - arithmetic - on - variables - that - store - fuctions

fibonacci = fibonacci + argc - argc

print fibonacci(argc)
