test = focuses - on - stress - tests - through - loops -and - fnctions - then - short- circuiting 
delimiter = 999999999999
print delimiter
test = args - reading - correctly
print argc
if(argc > 7){
    print argc
}
else{
    print argc*2
}
print delimiter
test = common - labels

fmt = 6
mod1 = 7
main = 0
exit = 0

can = we - still - prnt

print 1

print delimiter

operation = testing

print 7 % 2
a = 11 % 0
b = 11/0
c = (11 >= 12) <= 1
d = 133 < c > a

print a
print b
print c
print d

basic = stress - tests - so - not - using - interpreter

print fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return fun{return it}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0)

i = 0
while(i < 200000000){
    i = i + 1
}
print i

print delimiter

get = all - primes - up - to - big - number - inefficiently - this - works - with - the - most - inefficient - compiler - implementation - around - seven - secs - so - it - should - be - a - good -litmus - test - for - speed

x = 0

primes = fun {
    num = 2
    while (num <= it) {
        isPrime = 1
        divisor = 2
        while (divisor * divisor <= num && isPrime) {
            x = x + 1
            if (num % divisor == 0) {
                isPrime = 0
            }
            divisor = divisor + 1
        }
        if (isPrime == 1 && num == 999983) {
            print num
        }
        num = num + 1
    }
}

maxNumber = 1000000
trash = primes(maxNumber)
print x

print delimiter

short = circuit - logic - is - hard - to - implement

sc = fun{
    print it
}

if(1 || sc(123) || sc(1234)){
    prnt = nothing
}
else{
    print 1
}

if(0 && sc(456) && sc(789)){
    prnt = nothing
}
else{
    print 1
}

print delimiter

fnction = testing

nest1 = fun{return fun{return it}}(0)(0)
print nest1

print fun{if it == 0 return 1 else return 0}(1)

recursive = fnction - with - rets - in -middle
factorial = fun {
    if it == 1
        return 1
    if it == 0
        return 0
    return it * factorial(it - 1)
}

print factorial(15)

print delimiter

advanced = fnction - testing

f1 = fun{
    return 123
}

f2 = fun{
    return 10
}
print f1(f2)

print (4*(f1 - 1 + (3 - 2))/4)((((f2))))

testing = fncs - declared -inside -each -other


subten = fun{
    sum = 0
    f4 = fun{
        if(it == 1){
            return it
        }
        else{
            return f4(it-1)
        }
    }
    while(it > 10){
        sum = sum + f4(it)
        it = it - 1
    }
    return sum
}

a = subten(20)

c = subten(subten(subten(subten(50))))

print a
print c

check = we - can - call - eff - four - still
sum = 0

b = f4(15)
print b

test = should - run - in - under - ten - seconds