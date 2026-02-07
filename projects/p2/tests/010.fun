c = for -easy -identification, each -test -will -print -out -a -unique -number -if -it -fails

z=zzzzzzzzzzzzzzzzzzzzzz +test+ 1 zzzzzzzzzzzzzzzzzzzzzzzzz=z

c = recursive - Function - calls

sumDigits = fun {
    if (it < 10) {
        return it
    } else {
        return sumDigits(it / 10) + it % 10
    }
}

if (sumDigits(12345) != 15) print 1

z=zzzzzzzzzzzzzzzzzzzzz +test+ 2 - 4 zzzzzzzzzzzzzzzzzzzzzz=z

c = inner - Functions + effecting - Functions

f1 = fun {
    f2 = fun {
        return it * 2
    }

    return f2(it + 2) * f2(it)
}

if (f1(123) != 61500) print 2

f1 = fun {
    undefFunc = fun {
        return 404
    }
    return 200
}

if (undefFunc != 0) print 3
a = f1(0)
if (undefFunc(0) != 404) print 4

zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz=z

c = scoping - curlies

c = (I -forgot -what -I -wanted -to -test -here)

z=zzzzzzzzzzzzzzzzzzzzzz +test+ 5 zzzzzzzzzzzzzzzzzzzzzzzzz=z

c = Function - chains

f1 = fun {
    return fun {
        return fun {
            return fun {
                return 200
            }
        }
    }
}

if (f1(0)(0)(0)(0) != 200) print 5

z=zzzzzzzzzzzzzzzzzzzzz +test+ 6 - 7 zzzzzzzzzzzzzzzzzzzzzz=z

c = weird - anonymous - Functions

a = (fun { return 100 + it })(123)
if (a != 223) print 6
a = (fun return it + (fun return it)(12) + it)(15)
if (a != 42) print 7

z=zzzzzzzzzzzzzzzzzzzzz +test+ 8 - 11 zzzzzzzzzzzzzzzzzzzzz=z

c = short - circuit - testing

sideEffects = fun {
    print 54321
    print it
}

z = 0 && sideEffects(8)
z = 1 || sideEffects(9)
z = 2 || sideEffects(10)

if (0) a = sideEffects(11)
while (0) a = sideEffects(12)

z=zzzzzzzzzzzzzzzzzzzz +test+ 13 - 17 zzzzzzzzzzzzzzzzzzzzz=z

c = implicit - operator - precedence - testing
c = example - 1 < 2 > 0 - should - be - (1 < 2) > 0

if (1 < 2 > 0) != 1 print 13
if ((1 < 2) > 0) != 1 print 14
if (1 < (2 > 0)) != 0 print 15
if (2 == 2 == 1) != 1 print 16
if (2 == (2 == 1)) != 0 print 17