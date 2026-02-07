test = IFS - inside - FUNCTIONS

abc = fun {
    if(it > 3) {
        return 1
    } else {
        return 2
    }
}
print abc(4)
print abc(2)

test = redefining - variables
a = 1
a = 2
print a

test = redefining - FUNCTIONS

c = fun {
    return it + 1
}

c = fun {
    return it + 2
}

print c(argc)

test = operations - on - ARGC

print argc + 1
print argc - 1
print argc + argc
print argc * argc
print argc / 3
print argc, 1000

test = recursion

r1 = fun {
    print it
    if it == 1
        return 1
    return r2(it) 
}
r2 = fun {
    print it
    if it == 1
        return 1
    return r3(it) 
}
r3 = fun {
    print it
    if it == 1
        return 1
    return r4(it) 
}
r4 = fun {
    print it
    if it == 1
        return 1
    return r1(it-1) 
}

var = r1(5)
print var

test = IF - variations

if(1)
    print 1
else print 2 

if(0)
    print 1
else print 2 

if(1) {
    print 3
}
else print 4

if(0) {
    print 3
}
else print 4 

if(1) {
    print 5
} else {
    print 6
}

if(0) {
    print 5
} else {
    print 6
    print 7
}
