g = fun {
    print it
    it = it * 2

    if(it < 20) {
        print it
    }

    if(it <= 40) {
        temp = it + 1
        print temp
    }
    else {
        print 1
    }
}

a = g(5)
a = g(10)
a = g(20)
a = g(30)

h = fun {
    if (it == 0)
        return 5

    while(it > 0) {
        print it
        it = it - 1
    }
    return 10
}
    
b = h(0)
print b
b = h(3)
print b 

i = fun {
    print (2 * (it + 1))
    return 14
    return 8
}

c = i(0)
c = i(2)

if((c % 7) == 0)
    print c

x = 2
y = 3
z = 5

if((x == z) && (y != z))
    print x
else
    print y

if(z < y) {
    z = 7
}
else {
    x = 3
    if(x == y) {
        y = 8
        z = 1
    }
    y = 9
    z = 2
}

print y
print z

