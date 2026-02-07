bob = fun {
    x = 5
    while (it > 5) {
        if (it % 2) {
            print it
        } else {
            print 0
        }
        it = it - 1
        x = x + 1
    }
    print x
}

if (1) {
    print bob(101)
} else {
    print bob(1)
}

bobby = fun {
    temp = bob(it)
    while(temp > 0) {
        if(temp % 3) {
            print temp
        }
        temp = temp - 1
    }
}

print(1 && bob(453))
print(0 && bob(453))
print bobby(222)