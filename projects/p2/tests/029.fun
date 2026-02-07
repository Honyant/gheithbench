comment = simple + comparison + tests
print 0 == 0
print 5 == 10
print 4 != 3
print (0 - 100) != 4
print a <= 10
print z < 5 <= 20 > 5 >= 0
print 0    &&   0
    print 0 && 1
print 1 && 0
print 10 && 0 != (0 - 100)
     print 20 || 60
print 0 || 3
 print  5 < 10 == 7 -  (0 +100 *       (0 - 1) / (0 -1)) + 20 & 50 != 4 % 1
print (0 - 500) % (0 - 3)

comment = testing + (not + methods)

if (fun {return 5} * 0) print 19
if (fun {return 5} * 0 + 1) print 20

a = fun {return 10}
temp = a
a = a - 500

while (a != temp) a = a + 1
print (a)(1)

print (((1 * temp)))(100)

factorialrec = fun {
    if it <= 1 return 1

    return factorialrec(it - 1) * it
}

factorialnormal = fun {
    out = 1
    val = 1
    while (((val <= it))) {
        out = out * val
        val = val + 1
    }

    return out
}

start = 0
while (start < 100) {
    print factorialrec(start) == factorialnormal(start)
    start = start + 1
}

it = (0 - 7)

silly = fun {
    while fun print 5 || 1{
        sum = fun while fun return 6 if 1 print 6
        return 0 * fun {print 5} (0)
    }
}

print silly(5)

showpointer = fun {
    it = 68
    print it
    return 68
}

print 0 * (((showpointer(showpointer(showpointer(showpointer(showpointer(showpointer(showpointer(showpointer((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((showpointer))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))
print 0 && (((showpointer(showpointer(showpointer(showpointer(showpointer(showpointer(showpointer(showpointer((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((showpointer))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))
print 1 || (((showpointer(showpointer(showpointer(showpointer(showpointer(showpointer(showpointer(showpointer((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((showpointer(factorialrec(6))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))