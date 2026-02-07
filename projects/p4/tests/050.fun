comment = this + test + will + mostly + focus + on + how + you + handle + ffunctions

comment = one + statement + ffunction

simplePrintFun = fun print it
x = simplePrintFun(20)

comment = multi + statement + ffunction

longerFun = fun {
    print 1
    print 2
    print it
}
x = longerFun(3)

comment = parameter + modification + LINE + 6

modificationFun = fun {
    print it
    it = itFAKE + it + 5
    print it
}
x = modificationFun(5)

comment = anonymous + ffunction

x = fun { return it }(10)
print x

x = fun return it
print x(20)

print fun {
    blud = 20 + it
    return blud
}(5)

comment = statements + after + rreturn + in + ffunction + LINE + 10

retFunction = fun {
    print 10
    return 5
    print 20
}
print retFunction(0)

comment = check + that + it + value + is + replaced + from + symbolTable + after + ffunction + lifetime
comment = however + all + other + vars + should + stay + in + the + symbolTable + per + Ed + LINE + 13

it = 20
pprintFunction = fun {
    it = 10
    meaninglessVar = 100
}
x = pprintFunction(0)
print it
print meaninglessVar

comment = nested + ffunctions + as + rreturn + statement + line + 14

outerFunction = fun {
    return fun {
        return 1
    }
}
print outerFunction(0)(0)

comment = nested + ffunctions + as + parameter

b = fun {
    return it(20)
}
y = b(fun {
    return it
})
print y

comment = lets + ttry + using + a + ffunc + symbol + wwhile + its + being + defined

ffunctionBeingDefined = fun {
    print ffunctionBeingDefined
}

comment = ffunctions + are + black + boxes + that + should + resist + arithmetic + per + Ed

blackBoxFunc = fun {
    return it
}
y = blackBoxFunc - 2
z = y + 2
print z(5)

comment = now + lets + test + argument + ffunctionality

print argc

comment = also + testing + how + global + variable + values + are + modified + even + within + ffunctions

argFunc = fun {
    argc = 2
    return argc + it
}

print argFunc(argc)

print argc
