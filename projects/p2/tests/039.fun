a = testing-some-weekOne-OrderOfOperations-to-ensure-still-working

overFlowNUMbER = 18446744073709551615
a = 3458983245978
b = 2029390844
c = 330
d = 493009234
e = 444444
f = 1705435134005137014081755710450751408705807154
g = 17253993279492378
h = overFlowNUMbER

print  f % ((a / (b/ c / d/ e/g - h) * c) * 3) % (a / (b/ c / d/ e/g - h)   * c)
print  f % (((a / (b/ c / d/ e/g - h) * c) * 3) % (a / (b/ c / d/ e/g - h)   * c) + 4)

print  ((a / (b/ c / d/ e/g - h) * c) * 3) % (a / (b/ c / d/ e/g - h)   * c)
print (a / (b/ c / d/ e/g - h) * c) * 3 % (a / (b/ c / d/ e/g - h) * c)
print overFlowNUMbER

LineBreak = 3333333
print LineBreak

a = testing-newly-added-order-of-operations-plus-short-circuiting

a = basic-recursion-test
pow2 = fun {
    if it == 0 return 1
    return 2 * pow2(it-1) + 0
}

zero = 0
a = 123
b = 456
c = 789
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

a = testing-short-circuiting-and-nested-boolean-operators

print pow2(1) || (fun print 33) (5)
print 0 && (fun print 42) (5)

print 1 || (0 || (1 && (fun print 48) (5)))

a = this-should-call-fuunction
print 0 || (0 || (1 && (fun print 54) (5)))

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

a = now-fuunction-tests-first-with-other-statements

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

f = fun {
    while it < 20 * it {
        while it < it * 4 {
            if it % 4 == 0 return it
            it = it + 1
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

print LineBreak
a = FINISHED-TEST-CASE