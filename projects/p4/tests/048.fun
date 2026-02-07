TEST = ANONYMOUS - FUNCTIONS - AND - IMPLICIT - RETURN

print fun {
    x = 0
    while (x < it) {
        if x % 2 == 0
            print x
        x = x + 1
    }
}(7)

TEST = SHORT - CIRCUIT

f = fun {
    print it
}

x = 1 && (1 || f(2))

print x

TEST = FUNCTIONS - INSIDE - FUNCTIONS

g = fun {
    h = fun {
        return it + 2
    }
    return h(it)
}

print g(2)

TEST = MAKE - SURE - IMPLICIT - RETURN - IS - PRESENT - EVEN - IF - THERE - ARE - BRANCHING - CODE - PATHS

k = fun {
    if (it == 2) {
        print it
        return it
    }
}

print f(1)

TEST = RECURSIVE - CALLS - MAKE - SURE - IT - IS - SAVED

fib = fun {
    if (it <= 1) return 1
    return fib(it - 1) + fib(it - 2) 
}

print fib(8)

TEST = PARSING - FUNCTIONS - WITH - NO - BRACKETS

print (fun if it == 0 return 5)(0)
