
bitmasks = test - complete - test - of - operations - and - fnctions - and - recursion
pow = fun {
    if it == 0 return 1
    return 2 * pow(it-1)
}

t = pow(6)
s = 0
while (s < t) {
    x = 0
    sm = 0
    while (x < 6) {
        if s & pow(x) sm = sm + x
        x = x + 1
    }
    print sm
    s = s + 1
}

there = are - only - thirtytwo - registers
a = 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9
print a + argc

nested = fnctions - hell

a = (fun {return fun {return (fun {return it*2} + 5)}}(0)(1)-5)(10)
print a

short = circuiting
f = fun {print it}
print 1 || f(2)
print 0 || f(2)
print 1 && f(2)
print 0 && f(2)