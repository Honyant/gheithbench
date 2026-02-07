a = 0

if (a >= 0 && a <= 5 && a != 2) {
    print a
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