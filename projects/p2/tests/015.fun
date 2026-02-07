
Test = Functions - That - Return - Functions - And - Function - Math

a = 0 b = 1
recursive1 = fun {
    a = a + it
    return recursive2 + 5
}

recursive2 = fun {
    b = b * it
    return recursive1
}

z = (((((recursive1(1)-5)(2)(1)-5)(2)(1)-5)(2)(1)-5)(2)(1)-5)(2)
print z == recursive1
print a print b


Test = Short - Circuiting, Absence - Thereof, Commas, And - Lack - Of - Braces

x = 0
change = fun x = x + 1
f = fun if 1 != 1 && change(0) print 9 else print it
y = f(0-1)
print x
if 1 != 1 & change(0) print 9 else print 10
print x
y = change(0), 100
print y print x


Test = GCD - Formula

x = 72
y = 60
while y != 0 {
    temp = x
    x = y
    y = temp % y
}

print x print y
