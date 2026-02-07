c = basic - logic - test
print 1<1
print 1==1
print 1>1
print 1!=1
print 1<=1
print 1>=1
print 2<=1
print 2>=1
c = logic - and - operation - order
print 1&&0
print 1&&1||0
print 1||1&&0
print 1&&(1||0)
print 1&&1&&1&&1||0&&1
c = if - and - while - loop - basic - test
if 1 == 0 {
    print 10
}
else {
    print 11
}
x = 0
while x < 100 {
    print x
    x = x + 1
}
y = 0
while y < 100 {
    if y % 2 == 1 {
        print y
    }
    y = y + 1
}

c = normal - fnctions
randomArithmatic = fun {
    print 1
    it = it + 3
    return it
}
print(randomArithmatic(10))

c = recursive - fnctions
toStart = 1


twoToThePower = fun {
    if it == 0 {
        return toStart
    }
    else {
        return 2 * twoToThePower(it - 1)
    }
}

print twoToThePower(5)

c = interrelated - fnctions

SomeArithmatic = fun {
    it = it + 1
    it = it * 2
    it = it + 1
    it = it + 3
    return it
}
SomeMoreArithmatic = fun {
    it = it + 2
    it = it * 4
    it = it - 3
    it = SomeArithmatic(it)
    return it
}
print SomeMoreArithmatic(10)

c = combining - fnctions - while - loops - and - if - statements
holder = 0
counter = 0

EvenDigitSum = fun {
    while it != 0 {
        holder = it % 10
        if holder % 2 == 0 {
            counter = counter + holder
        }
        it = it - holder
        it = it / 10   
    }
    return counter
}

print EvenDigitSum(444555666)
