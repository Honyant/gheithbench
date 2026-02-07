delimiter = 99999999999999999

test0 = lets - see - argc - in - action

print argc

print delimiter
print delimiter
print delimiter

test02 = lets - compiled - code - is - fast

while (i < 15000000) {
    if (i % 1000000 == 0) {
        print i
    }
    i = i + 1
}

print delimiter
print delimiter
print delimiter

test03 = fmt - and - mod - is - a - variable - too

print fmt
print mod

print delimiter
print delimiter
print delimiter

test04 = nested - iFelse - statements

if (argc == 0) {
    if (test0) {
        if (fmt) {
            if (mod) {
                if (delimiter) print 1
            }
        }
    }
    else {
        if (34) print 2
        print 234
    }
}
else {
    print 35
}

print delimiter
print delimiter
print delimiter

test05 = lets - call - some - potentially - bad - variable - names

fmt = 0
main = 1
mod = 2

print fmt
print main
print mod
print if
print while

print delimiter
print delimiter
print delimiter


test1 = lets - test - some - basic - FUNctions

f1 = fun {
    a = it
    return a
}

f2 = fun {
    return 1, 2, f1(it), 3, 4, f1(it - 3)
}

b = f1(23)
print a
print b
print f2(5)

print delimiter
print delimiter
print delimiter

test2 = make - sure - you - use - local - it - for - FUNctions

it = 2
b = f1(20)
print a
print b

print delimiter
print delimiter
print delimiter

test = lets - do - some - short - circuiting

f33 = fun {
    a = 23948
}

f333 = fun {
    b = 234123543
}

print 0 && f33(0)
print a
print 1 || f333(0)
print b

print delimiter
print delimiter
print delimiter

test3 = lets - do - recursion

abc = fun {
    if (it == 1) return it
    return abc(it - 1)
}

print abc(10000)

print delimiter
print delimiter
print delimiter

test4 = test + Function + calls + parsing

print (f1)(0)
print (((((((f1)))))))(0)
print ((f1)(f1(f1(f1(f1(f1(2)))))))

a = dp, (1, 2, 3, 4, (dp), f1)
print a(21)
print a

print delimiter
print delimiter
print delimiter

test5 = test + return + exits + Function

a = fun {
    while 1
        return it
}

print a(23)

print delimiter
print delimiter
print delimiter

test6 = test + some + operators

print 1 && 2
print 0 && 1
print f1 || 1
print f2 & 0

print delimiter
print delimiter
print delimiter

test7 = test - nested - fucntions

print (fun return fun return fun return fun return fun return fun return it + 1)(0)(0)(0)(0)(0)(0)

print delimiter
print delimiter
print delimiter

test8 = lets - do - some - Function - math

f232 = fun {
    return 1
}

f323 = f232 + 233
f323 = f323 - 233
f323 = f323 + f232
f323 = f323 - f232
print f323(1)