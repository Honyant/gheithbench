TEST = FUNCTION - LITERAL - PARAMETERS

f = fun return it(10)
print f(fun return it + 10)

TEST = NO - BRACKETS

g = fun if it == 5 return 1 else return 0
print g(5)
print g(6)

TEST = RECURSION

k = fun {
    if (it <= 2) return 1
    return k(it - 1) + k(it - 2)
}

print k(1)
print k(8)

TEST = FUNCTION - ARITHMETIC

j = 1 + fun { return it } - 1
print j(10)

j = fun { return it } + 1
print (j - 1)(10)

TEST = FUNCTIONS - THAT - RETURN - FUNCTIONS

j = fun { return it(0)(10) }
print j(fun { return fun { return it }})