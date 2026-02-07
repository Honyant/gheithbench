comment = recursion - through - four - FUNCTIONS
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
 
comment = IF - variations

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
