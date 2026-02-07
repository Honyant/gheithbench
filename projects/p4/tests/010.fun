description = some-edge-cases-I-encountered-while-making-my-compiler

note = starting-with-arithmetic

note = loading-large-numbers
print 10001
print 111
print 1111111
print 111111111111
print 11111111111111111

note = arithmetic-overflow
print 10002
print 11111111111111111111 + 11111111111111111111
print 11111111111111111111 * 11111111111111111111
print 11111111111111111112 / 11111111111111111111
print 0 - 1

note = div-and-mod-by-0
print 10003
print 1 / 0
print 1 % 0
print 0 / 0
print 0 % 0

note = use-unsigned-conditions
print 10004
print 11111111111111111111 > 0
print 11111111111111111111 > 11111111111111111112
print 11111111111111111111 >= 0
print 11111111111111111111 >= 11111111111111111112
print 11111111111111111111 < 0
print 11111111111111111111 < 11111111111111111110
print 11111111111111111111 <= 0
print 11111111111111111111 <= 11111111111111111110

note = naming-issues

note = possible-naming-collisions
print 10005
main = 1
print main
fmt = 2
print fmt
start = 3
print start
exit = 4
print exit

note = special-names
print 10006
it = 1
print it
argc = argc
print argc

note = moving-on-to-control-flow

note = out-of-order-declaration
print 10007
f = fun {
    print it
    return x
}
x = 2
print f(1)

note = no-ret
print 10008
h = fun {
    print it
}
print h(1)

note = short-circuiting
print 10009
print 1 && f(3)
print 0 && f(0)
print 1 || f(0)
print 0 || f(4)
x = 8
print f(5), 1 + f(6), f(7)

note = call-associativity
print 10010
g = fun {
    return f
}
print g(0)(1)

note = general-logic-test

note = prime-factorization
print 10011
print fun {
    candidate = 2
    while (candidate * candidate <= it) {
        if (it % candidate) {
            candidate = candidate + 1
        } else {
            print candidate
            it = it / candidate
        }
    }
    print it
    return 1234567890
}(argc)

note = global-it-should-remain-1
print 10012
print it

note = gcd-recursion
print 10013
p = argc
q = 168
euclid = fun {
    if q == 0 return p
    else {
        it = p
        p = q
        q = it % q
        return euclid(0)
    }
}
gcd = fun {
    if q > p {
        it = p
        p = q
        q = it
    }
    return euclid(0)
}
print gcd(0)
