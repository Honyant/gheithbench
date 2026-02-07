print argc

if (argc == 0) {
    if (1) {
        print jimmy(30)
    } else {
        print 5
    }
}

jimmy = fun {
    y = 35
    while (y > 0) {
        if (argc > 0) {
            print y
        }
        y = y - 1
    }
    x = it * 10
    if (it % 5 == 0) {
        x = it % 5
    }
    print x
}

print 111111111

jim = fun {
    if (it > 0) {
        print it
        return jim(it-1)
    } else {
        return jimmy(it)
    }
}

if (argc > 1) {
    print jim(10)
} else {
    print jimmy(10)
}

print (1 && jim(23))
print(0 && jimmy(8))
print jimmy(12)