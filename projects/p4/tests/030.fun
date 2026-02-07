THIS = TEST - WILL - BE - FOCUSED - ON - TESTING - FUMCTIONS

test = recurisvely - called - fumctions
fib = fun {
    if it == 1 || it == 0
        return 1

    return fib(it-1) + fib(it-2)
}

this = will - print - out - each - time - to - make - it - easier - to - debug
i = 0
while i < 20 {
    print fib(i)
    i = i + 1
}



test = this - time - we - can - do - the - same - thing - but - use - an - implicit - var - it - and - double - the - values
fib2 = fun {
    return it(temp) + it(temp)
}

again = this - will - help - debug
make = sure - to - check - if - your - it - variable - is - being - handled - with - correct - scope
it = 111
count = 0
while count < 20 {
    temp = count
    print fib2(fib)
    count = count + 1
}
print it



test = this - time - it - will - be - about - anonymous - fumctions
print fun{a = 1 return it}(10)
print a
b = fun return a
c = fun return b
print fun{return c}(5)(10)(10)
print fun{}(0)
print fun{ return fun { return fun { return fun { return 1}}}}(0)(0)(0)(0)



number = of - parenthesis - should - not - be - a - problem
nor = should - performing - mathematical - calculations - that - dont - change - the - value
print ((((((((((fib))))))(3)))))
print (fib)(((((((((((((((((5)))))))))))))))))
print (fib - 1 + 1 + 7 - 7 + (2/2) - 1)(3)



this = is - an - argc - test
print argc
print argc * argc + argc - argc / argc

hint = check - that - you - are - branching - when - the - denominator - is - zero
argc = 15 % 0
print argc / argc
print argc

hint = check - that - you - are - performing - modulo - correctly - with - unsigned - nnumbers
argc = (0 - 1) * argc % (1 - 5)
print argc



tricky = test - for - conflicting - variable - naming
hint = you - can - fix - this - by - adding - invalid - characters - to - the - beginning - of - those - variables
ldr = 5
print ldr
exit = 1
print exit
fmt = 300
if (exit)
    if (exit || 0)
        if (exit && 0)
            print 1111111
        print 8446744073709551615
    print fmt

main = fun {
    return it
}
print main(10)



testing = a - very - large - number - to - see - overflow - handling
veryLargeNumber = 999999999999999
print veryLargeNumber

hint = make - sure - that - the - multiplication - instruction - uses - x - registers - not - w
veryLargeNumber = veryLargeNumber * 10
print veryLargeNumber
veryLargeNumber = veryLargeNumber - 70000000000000 * 3
print veryLargeNumber



testing = short - circuiting - please - work
hint = make - sure - you - do - not - have - a - check - for - equals - zero, but - rather - a - check - for - not - equals - zero
print 0 || 3
print 3 || fun {print 5}(1)
print 0 && fun {print 6}(1)
print fun {return it}(0) && 0
print 1 || 3 && 0