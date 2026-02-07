delimiter = 999999
test1 = test + operators
print 1 > 0
print 1 < 0
print 1 & 1
print 1 && 1
print 1 && 0
print 0 || 1
print 1 || 0
print 1 || 1
x = 1, 2
print x

print delimiter

test1b = test + long + operators
print 1 > 2 <= 1 > 1 < 5 < 10 > 10
print 1 || 1 || 0 || 0 || 1
print 1 && 1 && 0 || 1
print 1 && 0 || 0

print delimiter

test2 = test + IF + ELSE
if 1 == 0 {
    print 8
} else {
    print 9
    print 10
}

test2b = test + nested + IF
if x < 2
    if x + 1 < 3
        print 10
    else 
        print 13

print delimiter

test3 = test + WHILE
x = 0
while x < 5 {
    if x % 2 == 1
        print x
    x = x + 1
}

print delimiter

test4 = test + FUNCTION
x = fun{
    print it
    return 10
}
print x(5)

test4b = test + FUNCTION + scope
it = 7
print it
y = fun{
    print it
    x = 5
    print x
    return it + x
}
print y(10)
print it
print x

test4c = test + RETURN + in + loop + and + conditional
y = fun{
    while it < 5 {
        it = it + 1
        print it
        if it == 3 
            return it
    }
}
print y(1)

print delimiter

test4d = test + anonymous + FUNCTION + and + parantheses + around + FUNCTION
print (fun{
    print 10
    return it
}) (5)

test5 = test + FUN + variable
fun = 10