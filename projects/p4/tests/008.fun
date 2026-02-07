Hello = and - Welcome - To - My - Test - Exclamation - Mark

This = is - meant - to - be - a - comprehensive - test

The = beginning - tests - focuses - on - p2 - FUNctionality 
    this = implicitly - tests - your - ARM - FUNCtionality

The = end - tests - focuses - on - p4 - FUNctionality
        for = your - convenience
        the = p4 - FUNctionality - is - modular
        so = you - can - copy - over - only - that - section - of - this - test

This = will - test - some - edge - cases - as - well
because = this - is - FUN - smile - emoji - heart - emoji

delimiter = 1000000000000000

test = that - nested - FUNCTION - definitions - and - recursion - works

f3 = fun {
    f4 = fun {
        print it
        f5 = fun {
            print it 
        }        
    }
    if (it > 1) {
        print f3(it - 1)
    } else print it 

    print it + 1729 
    return it
}

print f3(2)

print delimiter
btw = this - is - line - 6 - of - the - ok - file

test = calling - a - FUNCTION - defined - in - another - FUNCTION

print f4(1)
print f5(1)

print delimiter + 1
btw = this - is - line - 11 - of - the - ok - file

test = that - FUNCTIONS - work - with - comma - operator

f = 3, 4, fun print it

print f(1)

test = that - you - can - modify - a - variable - storing - a - FUNCTION 
and = it - will - no - longer - be - equal

copy = f 
f = f - 1 
print copy != f

print delimiter + 2
btw = this - is - line - 15 - of - the - ok - file

test = that - FUNCTIONAL - programming - works

f6 = fun {
    print 17
    print it(0)
}
print f6(copy)

print delimiter + 3
btw = this - is - line - 20 - of - the - ok - file

test = that - IF - works

if 1 print 1 else print 0
if (1) print 1 else print 0
if (1) { print 1 } else print 0
if (1) { print 1 } else { print 0 }

test = that - nested - IF - works

if 1 print 1 if 1 print 2 if 1 print 3

if (1) {
    if (2) {
        if (3) {
            if (4) {
                if 5 if 6 if 6 if 7 if 8 if 9 if 17 if 18 print 10101
            }       
            print 101111
        }
        print 34532
    }
    print 3423
}

print delimiter + 4
btw = this - is - line - 32 - of - the - ok - file

test = that - nested - IF - ELSE - works

if 1 { print 1 if 2 print 2 else print 0 } else print 0

if 1 { print 1 if 0 print 2 else print 7 if 1 print 3 else print 9 } else print 11

if (1) {
    if (2) {
        if (3) {
            if (4) {
                if 5 { 
                    if 6 print 6 
                    else { 
                        if 6 {
                            if 7 print 7 
                            else { 
                                if 8 if 9 if 17 if 18 print 10101 
                            }
                        }
                    } 
                }
            }       
            print 101111
        } else {
            print 141984194812948219
        }
        print 34532
    }
    print 3423
} else {
    print 123497147
}

print delimiter + 5 
btw = this - is - line - 42 - of - the - ok - file

test = that - WHILE - loops - work

w = 0
while (w < 3) && w < 3 {
        print w
        w = w + 1
}

print delimiter + 6 
btw = this - is - line - 46 - of - the - ok - file 

test = WHILE - and - IF - together

this = pRINTS - the - letter - E

line = 1 
while (line <= 5) {
    if line == 1 || line == 3 || line == 5
        print 33333
    else 
        print 3
    line = line + 1
}

test = that - nested - WHILE - loops - work 

i = 0
while (i < 5) {
    j = 0
    while (j < 5) {
        k = 0
        while (k < 5) { 
            print i + j + k
            k = k + 1
        }
        j = j + 1
    }
    i = i + 1
}

print delimiter + 7 
btw = this - is - line - 177 - of - the - ok - file

test = combinations - of - FUNCTIONS - IF - and - WHILE

box = fun {
    row = 1 
    while (row <= it) {
        col = 1
        num = 0
        while (col <= it) {
            if row == 1 || row == it || col == 1 || col == it
                num = num * 10 + 1
            else
                num = num * 10 + 3
            col = col + 1
        }
        print num
        row = row + 1
    }
}

print box(5)
print box(7)

print delimiter + 8 
btw = this - is - line - 192 - of - the - ok - file

test = lowercase - FUN - as - a - variable - name

fun = 3

you = should - not - fail - at - offset - here

test = calling - a - FUNCTION - that - is - defined - later - in - the - file

f77 = fun {
  print f88(0)
}

f88 = fun {
  print 888
}

print f77(0)

print delimiter + 9
btw = this - is - line - 196 - of - the - ok - file

P4 = TESTS - START - HERE
P4 = TESTS - START - HERE
P4 = TESTS - START - HERE
P4 = TESTS - START - HERE
P4 = TESTS - START - HERE

test = NEW - ARGC - FUNCTIONALITY 

test = ARGC - WITH - ALL - MATH - OPERATORS

print argc
print argc + argc
print argc - argc
print argc * argc
print argc / argc
print argc % argc
print argc & 24
print argc && argc
print argc || argc
print argc < argc
print argc <= argc
print argc > argc
print argc >= argc

print delimiter + 10
btw = this - is - line - 210 - of - the - ok - file

test = ARGC - WITH - IF - AND - WHILE

if (argc == 8) print argc

i = 0
while argc + i < 16 {
        i = i + 1
        print argc + i
}

test = that - you - can - name - FUNCTIONS - with - ARGC - prefix

argcFunction = fun {
        if argc + it > 0 print argc + it
}

print argcFunction(5)

print delimiter + 11
btw = this - is - line - 222 - of - the - ok - file

test = ARM - SPECIFIC - EDGE - CASES

test = MOVING - 64 - BIT - NUMBERS

uint64t = 18446744073709551615

print uint64t

print uint64t - 1

print uint64t / 2

test = that - you - can - name - variables - fmt
    this = is - because - of - gheiths - starter - code

print fmt 

fmt = uint64t 

print fmt

test = that - you - checked - for - division - and - mod - by - zero
    note = udiv - should - handle - this - automatically

print fmt / 0
print fmt % 0

print delimiter + 12
btw = this - is - line - 230 - of - the - ok - file

test = that - you - short - circuited - correctly
    and = that - you - use - one - and - zero - for - true - and - false
    some = people - may - use - nonzero - and - zero - for - true - and - false

print 0 && 1 
print 1 || 1 
print 1 || 0 
print 0 || 1 
print 0 || 0 

print 3 || 5
print 0 || 5
print 5 || 0 
print 0 && 5 
print 5 && 6

print delimiter + 13
btw = this - is - line - 241 - of - the - ok - file

test = common - label - names 
    hopefully = you - arent - aliasing - labels - and - variable - names
    you = should - be - using - underscores - instead
    because = FUN - variable - names - cant - contain - underscores

print label0
print label1

print mod0
print mod1

print cmp0
print cmp1

print compare0
print compare1 

print jump0
print jump1 

print skip0
print skip1 

print delimiter + 14
btw = this - is - line - 254 - of - the - ok - file

test = comma - operator - with - ARGC 

print 1, 2, argc 
print 1, 2, (argc, 1, argc, 3, argc)

test = common - ARM - instruction - names
        i = increment - here - to - help - you - figure - out - which - one - youre - struggling - with

print exit + 1
print ret + main + 2

print ldr + 3
print str + 4

print ldp + 5
print stp + 6

print data + 7
print text + 8
print global + 9
print string + 10

print sp + 11

print add + 12
print sub + 13
print mul + 14
print udiv + 15

print cmp + 16

print bl + 17
print bgt + bhs + 18
print blt + bhi + 19
print bge + blo + 20
print ble + bls + 21
print b + 22
print beq + 23
print bne + 24

print mov + 25

print and + 26

print cbnz + 27
print cbz + 28

print call + 29 

print delimiter + 15 
btw = this - is - line - 286 - of - the - ok - file

LETS = FINISH - WITH - STYLE

STRESS = TEST - TIME

fib = fun {
    if (it <= 1) return it
    return fib(it - 1) + fib(it - 2)
}

print fib(38)
