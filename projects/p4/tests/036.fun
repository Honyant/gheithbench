test = new - operators
print 0 < 1000
a = 60 > 49
print a
b = 2 <= 2
print b
b = 1 <= 1
print b
a = 50 < 20
print a
d = ((5-10) < 0)
print d
e = 4 & 8
print e
f = 0 & 17
print f
g = 5 && 17
print g
h = 0 && 0 <= 1 || 10
print h
i = 1 & 0
print i
j = 1 || 0 || 1
print j
k = 1, 2
print k
l = 1+3, 9*7, 5&&17
print l

print 10000000

test = conditionals
if 0 < 1
    print 310
if 1 < 0
    print 311
print 312
if 0 || 0 && 1
    print 313
if (1 || 0)
    print 314
if (1 || 0)
    if 0 < 100
        print 316
if (1 || 0)
    if 0 < 100
        if 1 < 0
            print 315
if (1 || 0)
    if 0 >= 100
        if 0 < 1
            print 317
x = 5
if (x == 5) 
    print 318
if (x != 5)
    print 319
if (x != 5) {
    print 319
    print 321
}
if (x == 5) {
    print 320
    print 322
    print 324
}
if (x != 5) {
    print 323
    print 325
    print 327
}
if (x == 5)
    print 326

if (x != 5)
    print 329

if (x == 5)
    print 328
else
    print 331

if (x != 5)
    print 333
else
    print 330

x = 5
if (x != 5) {
    if (x == 5)
        print 335
    else print 337
}
else
    print 332
    
if (x == 5) {
    if (x != 5) {
        if (x == 5) {
            print 339
        } else print 341
    } else {
        print 334
    }
} else {
    print 343
}
if (4 || x && 20) & 8
    print 345
else if (4 || x && 20) & 9
    print 336
else if x == 5
    print 347
else print 349

test = while - loops
i = 0
while i < 5 {
    print i
    i = i + 1
}
i = 0
while (i < 2) {
    while (j < 5) {
        j = j + 1
        print j
    }
    i = i + 1
}
while (i > 5) {
    print 351
}
i = 0
while (i < 10) {
    if (i < 5) print i
    else print 0
    i = i + 1
}

print 20000000

test = basic - fuunctions
a = fun {
    print 338
}
print a(5)
a = fun return 1 + 1
print a(3)
b = fun {
    return 340
}
print b(5)
c = fun {
    return it
}
print c(342)
d = fun {
    if (it == 1000) print 344
    else if (it != 10) print 353

    if (it == 40) print 355
    else print 346

    while (it < 2000) {
        it = it + 200
        print it
    }
    return it
}
print d(1000)

print 30000000

test = return - early
e = fun {
    while (it < 2000) {
        it = it + 200
        if (it == 1600) return it
    }
}
print e(1000)
f = fun {
    if (it == 50) {
        print 357
    } else if (it == 20) {
        return it - 10
    } else if (it == 100) {
        return it - 20
    } else {
        return it + 10
    }
    print 359
    return 50
}
print 1 + f(100) / 2

g = fun {
    print 348
    return it
    print 361
}
print g(350)

print 40000000

test = it - scope
it = 23
h = fun {
    return it + 2
}
print h(2)
print it

print 50000000

test = recursion
factorial = fun {
    if it == 0 return 1
    return it * factorial(it - 1)
}
print factorial(0)
print factorial(5)
print factorial(20)

print 60000000

test = nested - fuunctions
i = fun {
    print 352
    j = fun {
        print 354
        return it
    }
    if (j(5) != 5) {
        print 363
    } else {
        print 356
    }
    return it
}
print i(358)

print 70000000

test = arg - c
print argc + 1
argc = 100
print argc

print 80000000

print divide / modulo - by % zero

integerOverflow = 438291057382184343217483920301
print integerOverflow

test = short - circuiting
v = fun {
    print 5
}
print 1 || v(3)
print 0 && v(5)

