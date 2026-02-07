a = testing-basic-OrderOfOperations-and-overflow-handling
b = btw-also-testing-that-no-duplicate-labels-occur-when-variable-is-seen-more-than-once-btw
header = incorporating-p2-tc-with-alot-of-ARGC-tests-and-more-short-circuiting-overflow-nestedFction-tests

overFlowNUMbER = 18446744073709551615
a = 3458983245978
b = 2029390844
c = 330
d = 493009234
e = 444444
f = 1705435134005137014081755710450751408705807154
g = 17253993279492378
h = overFlowNUMbER

argc = argc + testing-to-ensure-ARGC-is-correctly-stored-and-can-be-changed-and-used-normally
print argc
argc = argc * 5 % 4 - 3 + (argc + argc) / 3
print argc

format = testing-to-ensure-common-label-names-dont-corrupt-program-intended-output
fmt = another-one + format + 3
loop = another-one + fmt + 45 + label - text

print format - fmt * loop

print  f % ((a / (b/ c * argc / d/ e/g - h) * c) * 3) % (a / (b/ c / d/ e/g - h)   * c)
print  f % (((a / (b/ c / d/ e/g - h) * c) * 3) % (a / (b/ c / d/ e/g - h)   * c) + 4)

print  ((a / (b % argc/ c / d/ e/g - h) * c) * 3) % (a % argc / (b/ c / d/ e/g - h)   * c)
print (a * argc / (b/ c / d/ e/g - h) * c) * 3 % (a / (b - argc / c / d/ e/g - h) * c)
print overFlowNUMbER
print g * g * g * g * g * g * g * g * g * g* g * g *g

LineBreak = 3333333
print LineBreak

a = testing-logical-operators-and-short-circuiting

a = basic-recursion-test
pow2 = fun {
    if it == 0 return 1
    return 2 * pow2(it-1) + 0
}

zero = 0
a = 123 & argc
b = 456 & argc
c = 789 & (argc * 42)
d = pow2(5)
e = pow2(7)
f = pow2(10)

print a & (b && c) || c && d || ((e || f) & d & f)
print a && zero == (b && c) || c && d || ((e || f) & d & f) && zero
print a && zero != (b && c) || c && d || ((e || f) & d & f) && zero

print b <= c
print c > f

print b< c, c >=f, ((b != b) && b && c || (c && d)) || ((e || f) & d & f) && zero
print b< c, c >=f, ((b != b) && b && c || (c && d)) || ((e || f) & d & f) && zero, f * pow2(8)

print LineBreak

a = testing-short-circuiting-and-basic-anonymous-fctions

temp = argc
argc = 0
print argc * pow2(1) || (fun print 33 - argc - it) (5)
print 0 && (fun print 42) (5)

argc = temp
it = should-not-call-prnt-fortyEight
it = 1 - it-should-be-changeable-and-can-be-passed-as-parameter
print pow2(it) + should-res-in-two

print (argc && fun {print 4}(1) || 1) || (0 || (1 && (fun print 48) (5)))

a = this-should-call-fuunction
print 0 || (0 || (1 && (fun print 54 * it) (5)))

a = this-should-call-fuunctionOne-butNotTwo
print (1 && (0 || (0 || (fun print 60) (((5)))) && ((fun print 50)) (5)))

print LineBreak

a = testing-nested-statements-with-weird-spacing
x = 0
while x < 30 {
    while x < 5 {
    print x
    x = x + 1 while x < 7 x = x + 1
    while x < 22 {
    while x < 9 {print x + 1 x = x + 1}
        print x
        x = x + 3
        if x % 2 == 0
            x = x - 2
                        else x = x - 1
    }
    }
    print x
    x = x + 1
}
print LineBreak

a = now-fuunction-tests-first-with-other-statements-and-fuunction-calls-before-definition

f1 = fun {
x = 0
   while (it > 7) {
       x = (x + it) * it
       it = it - 3
   }
   print x
   print overrideX(1000)
   return x
}

overrideX = fun {
x = 0
while x < 5 {
    print x +    5

    x = x + 1
}
   return x
}

print f1(1000)
print f1(0)

f1 = overrideX + ensuring-fnctions-can-be-reassigned-to-other-variables + also-big-parameters-do-not-overflow
print f1(10000000000) == overrideX(10000000000)
print it-should-be-one

print LineBreak

a = testing-early-rets

x = 0
f = fun { while x < 5 {
    while x < 5 {
    print x
    x = x + 1 while x < 7 x = x + 1
    while x < 22 {
    while x < 9 {print x + 1 print x + 2 x = x + 1}
    print x
    return x
    x = x + 3
    if x % 2 == 0
        x = x - 2
        else x = x - 1
    }
    }
    print x + 5
    print x
    x = x + 1
}
}

print ((f(21)))

a = ensuring-argc-can-be-used-and-changed-in-fctions
f = fun {
    argc = 1
    while it < 20 * it {
        while it < it * 4 {
            if it % 4 == 0 return it
            it = it + 1
            print argc * it
        }
    }
}

print f(9)

print LineBreak

a = now-testing-mini-recursion-problems

f = fun {
    if it < 3 || it %3 == 0 return it 
    return f(it - 1) * f(it - 2)
}

argc = f(67)
print argc
print f(13)

optimizedPow2 = fun {
    if it == 0 return 1
    if it % 2 == 0 {
        return optimizedPow2(it / 2) * optimizedPow2 (it/2)
    } else {
        return optimizedPow2(it / 2) * optimizedPow2 (it/2) * 2
    }
}
print (optimizedPow2(5))

a = also-testing-weird-nested-parenthesis

tribonaci = fun {
    if it == 2 || it == 1 return 1
    if it == 0 return 0
    return ((tribonaci) ((it - 3))) + (tribonaci(it - 2)) + tribonaci(it - 1)
}
print tribonaci(6)

print LineBreak

a = testing-anonymous-fctions-and-composing-fctions-within-each-other

f = ((fun {
    a = 3
    while a < 1000 {
        a = a + 1
        if a % 2 == 0 {
            while (a < it) {
                print a 
                a = a + 1
            }
        }
    }
})) (9)
print f

f = ((fun {
    a = 3
    while a < 1000 {
        a = a + 1
        if a % 2 == 0 {
            while (a < it) {
                return fun return a * it
            }
        }
    }
})) (9)(8)

print f

nestedRet = (fun return it(tribonaci(5))(optimizedPow2(6)))
print (nestedRet(fun return fun return it) + f)

a = more-overflow-testing

print LineBreak * argc * argc * argc * argc * argc - argc / 13
print argc % it
a = FINISHED-TEST-CASE
