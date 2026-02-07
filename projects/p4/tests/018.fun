DELIMETER = 10000000000

print argc

isArgEven = fun {
  return (argc % 2) == 0
}

print isArgEven(0)

print DELIMETER

first = lets - test - out - some - operations

ZERO = 0
ONE = 1
TWO = 2

sideEffect = fun {
  do = not - output - this
  print 123456780000 + argc
}

print ONE & ZERO
print ONE & ONE
print ZERO & ZERO

print ZERO && TWO
print ONE && TWO
print ZERO && sideEffect(1)

print ZERO || TWO
print ONE || TWO
print ONE || sideEffect(1)

print DELIMETER + 1

now = lets - test - out - math

print 10 * 3 + 2
print 5 * 4 * 3 * 2 * 1
print 5 * (4 * (3 * (2 * 1)))
print 20 / 10
print 20 / 7
print 20 / 0
print 20 % 10
print 20 % 7
print 20 % 0

print DELIMETER + 2

lets = test - some - basic - FUNctions

double = fun {
  return it * 2
}

triple = fun {
  print it * 3
}

doTwice = fun {
  return it(4) + it(4)
}

callMe = fun {
  print it
}(40)

print double(11)
print triple(22)
scratch = doTwice(triple) 

print DELIMETER + 3

swap = FUNctions

outputOne = fun {
  print 1
}

outputTwo = fun {
  print 2
}

scratch = outputOne(0)
scratch = outputTwo(0)

swap = the - FUNctions
temp = outputOne
outputOne = outputTwo
outputTwo = temp

scratch = outputOne(0)
scratch = outputTwo(0)

print DELIMETER + 4

test = is - testing - effects - and - infinite - looping - and - recursion
- in - FUNctions- without - being - called

infiniteLoop = fun 
  while 1 
    count = count + 1

infiniteRecursion = fun return b(it + 1)

print DELIMETER + 5

anonymousFunctionCall = fun {
    print it
}(20)

print DELIMETER + 6

now = lets - make - sure - we - can - ret-urn - properly

testReturn = fun {
  return 10
}
print testReturn(100)

nestedFunctionReturns = fun {
  return fun {
    return fun {
      return fun {
        return it + 1
      } (it * 4)
    }(it * 3)
  }(it * 2)
}
print nestedFunctionReturns(1)

print DELIMETER + 7

can = we - ret-urn - from - control - flow - statements

testReturnFromIf = fun {
  if 1
    return 10
}
print testReturnFromIf(100)

testReturnFromWhile = fun {
  while 1
    return 1000
}

print testReturnFromWhile(10)

print DELIMETER + 8

testing = more + loops + in + FUNctions

xToThePowerOfX = fun {
  i = 0
  answer = 1
  this = loop - runs - the - parameter - number - of - times
  while (i < it) {
    answer = answer * it
    i = i + 1
  }
  return answer
}

print xToThePowerOfX(2)
print xToThePowerOfX(3)
print xToThePowerOfX(4)
print xToThePowerOfX(5)
print xToThePowerOfX(6)
print xToThePowerOfX(7)
print xToThePowerOfX(8)
print xToThePowerOfX(9)

print DELIMETER + 9

lets = pass - FUNctions - as - parameters

expect50 = fun { return it == 50 }

callFunctionWith50 = fun {
  return it(50)
}

print callFunctionWith50(expect50)

print DELIMETER + 10

can = we - get - FUNctions - as - ret-urn - types

FUNctionA = fun {
   print it 
   return 9
}

FUNctionB = fun { 
  print 10 * it 
  return 99 
}
FUNctionC = fun { 
  print 100 * it 
  return 999
}

pickFunction = fun {
  if (it == 1) {
    return FUNctionA
  } else {
    if (it == 2) {
      return FUNctionB
    } else {
      return FUNctionC
    }
  }
}

print pickFunction(1)(7)
print pickFunction(2)(7)
print pickFunction(3)(7)

print DELIMETER + 11
