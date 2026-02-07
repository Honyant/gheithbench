this = is - an - index - counter
y = 1
this = is - the - number - of - divisors
z = 0

print 111111
this = is-a-little-stress-test-of-recursive-fucntions-without-retunr
test1 = divisor-problem
computes = number - of - divisors - without - retunr
countDivisors = fun {
    if (y <= it) {
        if it % y == 0 {
            z=z+1
        }
        if (z <= 2) {
            y = y + 1
            trash = countDivisors(it)
        }
    }
}
start = 2
findAllPrimesUpToIt = fun {
    y = 1
    z = 0
    if (start <= it) {
        trash = countDivisors(start)
        if (z == 2) {
            print start
        }
        start = start + 1
        p = findAllPrimesUpToIt(it)
    }
}
print findAllPrimesUpToIt(1500)


print 222222
this = is-a-test-of-its
it = fun {
    it = it
    return it
}
print fun{ 
    it = it, 0 
    return 56
}(it)
trash = it(8008,it)
it = 9009
print it


print 333333
this = is-a-test-of-fucntion-parsing-and-assignmnet
test3 = fucnfcun, f3fufn, f2u3n
print test3
myfucntion = fun {
    return 3
}
print myfucntion(myfucntion)
print myfucntion(myfucntion(myfucntion(myfucntion))) + myfucntion - myfucntion
a = myfucntion
a = a * 2
print (a/2)(this-should-parse)

weirdfucn = fun {
    return fun {
        return 3
    }
}

print (4*((2*(weirdfucn)/2)(987654321))/4)(shouldret3)

print 444444
this = is-a-test-of-brackets
curindex = 0
while curindex < 10
    curindex = curindex + 1
print curindex
while (curindex < 20)
    curindex = curindex + 1
print curindex
while (curindex < 30)
    {curindex = curindex + 1}
print curindex
while curindex < 40
    {curindex = curindex + 1}
print curindex

newvar = 0
if newvar
    print this-shouldnt-run
else if (newvar == 0)
    print 100001
else if (newvar == 0)
    {print 8008}
else if newvar == 0
    {print 9009}
else 
    print 7007

if 1 & 1 && 2
    print 6006
else {
    print 5005
}

weird = fun {
    return fun {
        return 3
    }
}
weird2 = fun {return fun { return fun {return fucn }}}
trash = weird2(0)(0)

