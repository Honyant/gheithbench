should = always - return - the - value - of - the - parameter

bitsum = fun {
    bit = 1
    sum = 0
    while (bit <= it) {
        if (bit & it) {
            sum = sum + bit
        }
        bit = bit * 2
    }
    return sum
}

checks = number - of - positive - integers - relatively - prime - to - parameter

totient = fun {
    number = 1
    counter = 0
    while (number <= it) {
        a = number
        b = it
        while (b) {
            c = a
            a = b
            b = c % b
        }
        if (a == 1) {
            counter = counter + 1
        }
        number = number + 1
    }
    return counter
}

counting = the - number - of - twos - in - the - parameter

vptwo = fun {
    curr = it
    if (curr % 2 == 0) {
        return 1 + vptwo(curr/2)
    }
}

while (it < 1024) {
    print bitsum(it)
    it = it + 1
}

print totient(100)
print totient(1000)
print totient(97)
print vptwo(64)
print vptwo(192)
