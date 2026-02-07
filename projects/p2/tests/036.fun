basic = operator - tests

delimiter = 999999999

a = 1
b = 0
c = 300

print a < b 
print a <= a
print c > a
print c >= c
print a == b
print a != b
print a && b
print a || b
print a, b, c
print 2 & c

print delimiter

conditionals = tests

if x
    print 100
else
    print 200

a = 1
b = 2
c = 3 

if a > b
    if a > c
        print a
    else
        print c
else
    if b > c
        print b
    else 
        print c

a = 10

while a > 0 {
    print a 
    a = a - 1
}

b = 5
c = 4

while b > 0 {
    while c > 0 {
        print b + c
        c = c - 1
    }
    c = 4
    b = b - 1
}

print delimiter

now = time - for - FUNCTION - tests

x = fun  {
    while 1
        return 3
}

print x(1000)

y = fun {
    return fun { return 143 }
}

print y(1)(43)

a = x + y - y

print a(1)

w = fun {
    if it == 0
        return it
    else
        return it + w(it - 1)
}

print w(100)

print (w + x + y + z - x - y - z)(100)
