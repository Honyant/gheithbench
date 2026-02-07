
Check = basic - boolean - operations
Hint = for - debugging - check - short - circuiting

print 0 && 0
print 0 && 1
print 1 && 0
print 1 && 1

print 0 || 0
print 0 || 1
print 1 || 0
print 1 || 1


Check = complex - boolean - operations

print 0 && 0 && 0
print 0 && 0 || 0
print 0 || 0 && 0
print 0 && 0 && 0
print 1 && 0 && 0
print 1 && 0 || 0
print 1 || 0 && 0
print 1 && 0 && 0
print 0 && 0 && 1
print 0 && 0 || 1
print 0 || 0 && 1
print 0 && 0 && 1
print 1 && 0 && 1
print 1 && 0 || 1
print 1 || 0 && 1
print 1 && 0 && 1
print 1 && 1 && 1
print 1 && 1 || 1
print 1 || 1 && 1
print 1 && 1 && 1


Check = basic - Function - calls

Proper = Function - call
a = fun {
    print 2
}   
a0 = a(2)

Proper = Function - input - calls
b = fun {
    print it
}   
print b(3)

Proper = Function - output - calls
c = fun {
    return it
}   
print c(4)


Check = complex - Function - calls
Check = extra - parentheses
print (c)(5)
print ((c))(6)
print ((c))((7))

Check = recursive - Function
d = fun{
    if it == 0
        return 0
    else {
        print it
        dd = d(it - 1)
    }
        
}
d0 = d(5)

Check = early - Return - statements
e = fun{
    print 11
    print 22
    return 33
    print 44
    print 55
}
print e(0)

Check = early - Return - statements - and - recursion
f = fun{
    if it == 0 {
        return 0
        print 111
    }
    else {
        print it
        ff = f(it - 1)
    }
        
}
f0 = f(5)
