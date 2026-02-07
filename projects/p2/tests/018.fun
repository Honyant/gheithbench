delimiter = 999999999
print delimiter
this = test - is - basic - FUNction - testing - first

c = ccccccccccccccccccccccccccccccccccccc

it = 6

x = fun{return it}
print x(10)

y = fun{
    while(it < 10){
        it = it + 1
    }
    return it
}

print it
print y(5)

print delimiter
c = cccccccccccccccccccccccccccccccccccccc

short = circuit - testing

print delimiter
c = cccccccccccccccccccccccccccccccccccccc

true = fun{return 1}
shouldnotexecute = fun{print 9876}

print true(0)

print 1 || shouldnotexecute(0)
print 0 && shouldnotexecute(0)

print delimiter
c = ccccccccccccccccccccccccccccccccccccc

conditional = test

x = 11

if(x >= 11){
    print 123
    if x < 12
        print x
    else
        print 12345
    if x == 11{
        if x < 10 print 34
        else print 45
    }else print 4
    if x > 10 print 2
} else {
    print 100
}

print delimiter
c = ccccccccccccccccccccccccccccccccccccc

anyonymous = FUNctions

print fun { return 200 + it }(456)

a = fun{print it}(789)

print delimiter
c = ccccccccccccccccccccccccccccccccccccc

stress = test

while(a < 60){
    a = a + 1
    b = 0
    while(b < 70){
        b = b + 1
        c = 0
        while(c < 80){
            c = c + 1
        }
    }   
}

print a
print b 
print c 

print delimiter
c = cccccccccccccccccccccccccccccccccccc

recursive = fnction - with - rets - in -middle
factorial = fun {
    if it == 1
        return 1
    if it == 0
        return 0
    return it * factorial(it - 1)
}

print factorial(5)