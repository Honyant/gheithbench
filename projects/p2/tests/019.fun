testnum = 111111111111
TEST = 1 + IF
print testnum

a = 1 

if (a > 0){
    print 4
}
else 
    print 3 

if 1 > 0 {
    if 1 > 0 a = (1) else a = 3
    if 1 < 0 print a + 1 else print a
    print a + 2
}
else {}

TEST = 2 + WHILE
print testnum * 2

while(a < 100000){
    if a % 9999 == 0
        print a
    a = a + 1
}

a = 0
if (a < 0)
    while a < 9 * testnum
        a = a + 1 
else 
    print 9 

TEST = 3 + FUNCTION
print 3 * testnum

prit = fun print it

loop = fun {
    a = 0
    while(a < 1)
    b = a
}

unusedvar = loop 
unusedvar = unusedvar + 1

callwhencreated = fun{print it(5)}(prit)

callself = fun {
    numCalls = numCalls + 1
    if(numCalls < 1000){
        call = it(it)
    }
    else{
        print 314
    }
    return prit
}

call = callself(callself)

itMaskingTest = fun {
    print it
    it = it + 1
}   

maskTest = fun {
    apple = fun {
        it = it + 1
        call = itMaskingTest(it)
    }
    print it 
    call = apple(it)
    print it
}

call = maskTest(10)
globalVariableOnceDefinedTest = apple(8)

outerfun = fun {
    innerfun = fun {
        while(it < 30){
            it = it + 1
            it = outerfun(it)
            if(it == 10){
                return it
            }
        }
        return 99
    }

    if(it == 0){
        return innerfun(it + 1)
    }
    else if(it <= 10){
        return it
    }
    else{
        return innerfun(it + 1)
    }
}

print outerfun(0)
print outerfun(1)
print outerfun(11)
print innerfun(7)

while(1 < 0){
    return loop(1)
}

retInFunc = fun {
    while(it < 100){
        it = it + 1
        if(it == 50){
            return it
        }
    }
    print 666777
}

test = retInFunc(0)

multipleCalls = callself(callself)(99)

TEST = MISC + 4
print testnum * 4

if(3 || loop(0)){
    shortCircuit = prit(9)
}
else{
    shortCircuit = prit(0)
}

if(0 && loop(0)){
    shortCircuit = prit(9)
}
else{
    shortCircuit = prit(0)
}

if((3 || loop(0)) && (0 && loop(0))){
    shortCircuit = prit(9)
}
else{
    shortCircuit = prit(0)
}

commaTest = 8888, prit(77), 909
print commaTest

nestedCommaTest = (8888, prit(77), 909), 54
print nestedCommaTest

pemdas = (6 + 8 * 9 / 2 + 4 - 1 - 4 ) % 8 + 2 * (5 - 3) / 2 / 1 + 99 * 2
print pemdas

print 9



