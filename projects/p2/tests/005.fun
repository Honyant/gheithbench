if 2 + 2 == 5
    print 1984
else
    print 2024

if 2 + 2 == 5 {
    print 1
    print 9
    print 8 
    print 4
} else {
    print 2
    print 0 
    print 2
    print 4
}


delimiter = 999999999999999999


print delimiter

x = fun {x = 5}
print x(0), x

print delimiter

test = for + iterative + fucntion + declaration + along + with + commas

trash = fun { n = fun {print fun {print fun {return 0}}}, fun {print 3}, fun {print it * it return it * it} print n(it) return n(it * 2)}

trash = trash(10)
print trash

print delimiter

test = fucntion + declaration + as + an + argument + to + a + fucntion

a = fun { return it(3)}
print a( fun {return it * it} )

print delimiter

test = calling - fucntions - when - they - are - declared
a = fun{return it * it * it}(10)
print a

print delimiter

i = 0
while i < 15 {
    print i * i
    i = i + 1
}
here = we + pass + a + fucntion + called + either + square + or + cube + into + another + fucntion + depending + on + a + condition + and + run + that + fucntion
it = isnt + very + useful + with + only + one + argument + though

square = fun {
    return it * it
}

cube = fun {
    return it * it * it
}

q = 9

runFun = fun {
    return it(q)
}
conditionalPower = fun {
    if it == 2
        return runFun(square)
    else 
        return runFun(cube)
}

print delimiter

print conditionalPower(2)
print conditionalPower(3)

print delimiter



test = chaining + fucntion + calls
a = fun { return 1337 }
b = fun { return a }
c = fun { return b }
d = fun { return c }

print d(1)(1)(1)(1)

print delimiter

factorial = fun {
    if it == 0
        return 1
    else
        return factorial(it - 1) * it
}

print factorial(3)
print factorial(10)
print factorial(1000)

print delimiter

this = tests + returning + in + the + middle + of + the + fucntion
which = was + actually + hard + for + me + to + address

fibonacci = fun {
    if it == 0
        return 1
    if it == 1
        return 1
    return fibonacci(it - 1) + fibonacci(it - 2)
}
counter = 0
while counter < 10 {
    print fibonacci(counter)
    counter = counter + 1
}
print delimiter

c = 0
i = 0
test = that + fucntions + arent + redefined
while i<300000 {
    n = fun {c = c + 1}(0)
    i = i + 1 
}

print c
print delimiter


this = prints - the - fib - number - of - the - input - but - it - tests - fucntions - as - expressions

fucnprintxfib = 3, 4, fun { print fibonacci(it) }

this = tests + that + it + prints + twice
once = when + it + executes + the + fucntion
another = time + when + it + calls + print + here
this = ALSO + tests + that + if + you + use + a + return + register + it + gets + reset + in + iterative + fucntions
print fucnprintxfib(4)


print delimiter

we = use - Nilakantha - series - to - calculate - pi
iterations = 500000000
multiplier = 10000000000000
piapprox = 3 * multiplier
sign = 1
term = 2

while term < iterations {
    piapprox = piapprox + sign * (4 * multiplier / (term * (term + 1) * (term + 2)))
    sign = sign * (0 - 1)
    term = term + 2
}

print piapprox

print delimiter


x = 7
xmorethan = x > 3
xlessthan = x < 3
print xmorethan
print xlessthan

print delimiter


y = 6
yLEQ = y <= 6
yGEQ = y >= 6
print yLEQ
print yGEQ

print delimiter

z = 100
zequals100 = z == 100
znotequals100 = z != 100
print znotequals100
print zequals100

print delimiter

binary101010 = 42
binary010101 = 21
binary0 = binary101010 & binary010101
print binary0

print 1 && 0

print delimiter

commatest = (2+2, 4+4) - should - equal - eight - because - the - first - is - ignored - in - C
print commatest
