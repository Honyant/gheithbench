delimiter = 10111011101

unction = calling - in - body - before - definition
fib = fun {
    trash = fibhelp(it)
    return lo
}

lo = 1
hi = 1

fibhelp = fun {
    if it == 1 {
        lo = 1
        hi = 1
        return 7
    }
    if it % 2 == 0 {
        it = it / 2
        trash = fibhelp(it)
        pairlo = lo
        pairhi = hi
        lo = pairhi * pairhi - (pairhi - pairlo) * (pairhi - pairlo)
        hi = (pairlo * pairlo) + pairhi * pairhi
    }
    else {
        it = it / 2
        trash = fibhelp(it)
        pairlo = lo
        pairhi = hi
        lo = pairlo * pairlo + (pairhi * pairhi)
        hi = (pairlo + pairhi) * (pairlo + pairhi) - pairlo * pairlo
    }
    no = r3turn - value
}

index = 5
while (index <= 100) {
    print fib(index)
    index = index + 5
}


print delimiter

testing = many - operations

print 2 + (3 + (4 + (5 + 6)))
print ((((((((((((((((16 + ((14 + ((12 + ((10 + ((8 + ((6 + ((4 + ((1 + 2) + 3)) + 5)) + 7)) + 9)) + 11)) + 13)) + 15)
    ) + 17) + 18) + 19) + 20) + 21) + 22) + 23) + 24) + 25) + 26) + 27) + 28) + 29) + 30) + 31)

print delimiter
    
sideEffect1 = fun {
    print 1984
    return 1
}

sideEffect2 = fun {
    print 2024
    return 0
}

weretesting = short - circuiting

x = sideEffect1(0) || sideEffect2(0)
print x + 110

if (sideEffect2(0) || sideEffect1(0)) {
    print 777
}
else {
    print 888
}

if (sideEffect2(0) && sideEffect1(0)) {
    print 999
}

print delimiter

bignumber = 18446744073709551614

print bignumber

otherbignumber = 18446744073709551615

print bignumber & otherbignumber
