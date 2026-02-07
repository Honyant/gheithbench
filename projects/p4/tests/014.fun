delimiter = 999999999999999999

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

z = ((recursive1(1)-5)(2)(1)-5)(2)
print z == recursive1
print a print b

print delimiter


Test = Short - Circuiting, Absence - Thereof, And - Commas

x = 0
change = fun x = x + 1
f = fun if 1 != 1 && change(0) print 9 else print it
y = f(0-1)
print x
if 1 != 1 & change(0) print 9 else print 10
print x
y = change(0), 100
print y print x

print delimiter


Test = 0 - Division, Argc, While - Loops, Etc

x = 5 / 0
y = 5 % 0
print x print y
argc = argc + 2
print argc
while argc >= 1 {
    if argc % 2 == 1
        print argc
    argc = argc - 1
}
