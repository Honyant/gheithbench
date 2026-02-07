delimiter = 99999999999999999

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

test3 = lets - solve - this - UTPC - problem - URL - below
URL = codeforcesDotComSlashGymSlash104931SlashProblemSlashH
here = is - the - solution

N = 2
MOD = 1000000007

print N

dp = fun {
    n = (it / 1000) % 10
    aceUsed = (it / 100) % 10
    rank = it % 100

    if (n == N) return 1

    it = 0

    tmpRank = rank + 1
    while (tmpRank < 14) {
        tmpN = n
        tmpTmpRank = tmpRank
        it = it + dp((n + 1) * 1000 + aceUsed * 100 + tmpRank)
        it = it % MOD
        tmpRank = tmpTmpRank
        n = tmpN
        tmpRank = tmpRank + 1
    }

    if (aceUsed < 4) {
        it = it + dp((n + 1) * 1000 + (aceUsed + 1) * 100 + 1)
        it = it % MOD
    }

    return it
}

answer = dp(1101)

i = 2
while (i < 14) {
    answer = answer + dp(1000 + i)
    i = i + 1
}

print answer

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