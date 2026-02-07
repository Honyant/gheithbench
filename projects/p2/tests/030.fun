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