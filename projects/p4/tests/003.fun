THIS = IS - AN - IF - STATEMENT - TEST

TESTONE = BASIC - IF
print 111111111111111
a = 1
if (a) {
    print 1
}

TESTTWO = ADD - AN - ELSE
print 222222222222222
b = 0
if (b) {
    print 0
} else {
    if (a) {
        print 1
    }
}

TESTTHREE = NO - BRACES
print 333333333333333
if b print 0 else if a print 1

TESTFOUR = WHO - IS - YOUR - OWNER - QUESTIONMARK
print 444444444444444
if (b) {
    print 0
    if (argc) {
        print 1
    } else {
        print 0
    }
} else {
    print 1
    if (argc) {
        print 1
    } else {
        print 0
    }
}

TESTFIVE = DANGLING - ELSE - NO - BRACES
print 555555555555555
if (b) if (argc) print 3 else print 2 else if (argc) print 1 else print 0

TESTSIX = NEST - TOO - MANY - OF - THEM
print 666666666666666
if (a) {
    print 1
    if (a) {
        print 2
        if (a) {
            print 3
            if (a) {
                print 4
                if (a) {
                    print 5
                    if (a) {
                        print 6
                        if (a) {
                            print 7
                            if (a) {
                                print 8
                                if (a) {
                                    print 9
                                    if (a) {
                                        print 10
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

TESTSEVEN = BASIC - AND - SHORT - CIRCUIT
print 777777777777777
i = 0
inc = fun i = i + it
if b && a && inc(1) print 1 else print 0
print i

TESTEIGHT = BASIC - OR - SHORT - CIRCUIT
print 888888888888888
i = 0
if a || b || inc(1) print 1 else print 0
print i

TESTNINE = KEEP - GOING - AND
print 999999999999999
if a && 1 && 1 && 1 && 0 print 1 else print 0

TESTTEN = KEEP - GOING - OR
print 101010101010101
if b || 0 || 0 || 0 || 1 print 1 else print 0

TESTELEVEN = SHORT - CIRCUIT - IN - FUNCTION
THIS = CHANGES - PATHS - DEPENDING - ON - HOW - ITS - CALLED
print 111111111111111
swp = fun {
    i = 0
    if (it && it - 1 && it - 2 && inc(1) + 1) {
        print 3
    } else if (it || it + 1 || inc(1)) {
        print 2
    } else {
        print 1
    }
    print i
}
useless = swp(0)
useless = swp(3)