description = separator + is + used + to + group + related + sections + for + debugging + purposes
separator = 111111111

print separator

description = undeclared - variables - are - zero
print neverDeclared

print separator
description = fnction + call + from + inside + another + fnction
description = global - it - should - not - be - modified - by - fnction
description = the + it + variable + is + local + to + fnction + scope

it = 19
x = fun{return it + 5}
y = fun{return x(it) + it}

result = y(5)
print result
print it

print separator
description = fnction - that - does - not - rtrn - should - prnt - zero
description = set - rtrn - reg - to - zero - by - default

noReturn = fun {
    print it
    this = line - does - not - do - anything
}

print noReturn(5)

print separator
description = do - not - run - code - after - rtrn - or - you - will - infinite - loop
noInfiniteLoop = fun {
    return it
    while (1) print 5
}

print noInfiniteLoop(100)

print separator
description = making - sure - fnctions - are - opaque - uint
description = this - should - subtract - two - equal - uints - and - give - zero
print y - y

print separator
description = fnction - returning - another - fnction - and - then - params - passed - to - result
f1 = fun {
    return it * 6
}

f0 = fun {
    print it
    return f1
}

description = make - sure - we - can - print - result - of - fnctions - that - return
print f0(0)(7)

print separator
description = calling - a - fnction - which - is - defined - in - later - line
f5 = fun {
    print it
    return f6
}

f6 = fun {
    return it * 6
}

print f5(0)(7)


print separator
description = make - sure - we - can - name - a - fnction - it - and - call
it = fun {
    return it
}

print it(500)

print separator
description = printing - nested - fnctions - with - anonymous - call
print fun {
    print it
    return fun {
        print it
        return fun {
            return it
        }
    }
}(3)(3)(5)


print separator
description = modifying - the - reserved - variables
it = 76
it = it + 1
print it

print argc
argc = argc + 1
print argc
