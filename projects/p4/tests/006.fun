a = argc

if (a >= 0 && a <= 5 && a != 2) {
    a=0
}


while (a < 10) {
    b = 0
    while (b < 4) {
        if (b % 2 == 1) {
            b = b + 1
        } else {
            b = b + 1
        }
    }
    print a
    a = a + 1
}

c = fun {
    if (it % 2 == 1) {
        return 1
    } else {
        return 0
    }
}

print c(5)
print c(10)


testing = keywords
main = 0
x0 = 0
X1 = 0
global = 0

print main
print x0
print X1
print global