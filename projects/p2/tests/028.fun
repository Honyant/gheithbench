DELIMITER = 1000000000000
print DELIMITER + 1

test1 = is - testing - effects - and - infinite - looping - and - recursion
- in - FUNctions- without - being - called

infiniteLoop = fun 
  while 1 
    count = count + 1

infiniteRecursion = fun return b(it + 1)

print DELIMITER + 2

test2 = is - all - about - short - circuting
in = case - the - sevens - do - output, you - got - it - wrong

sideEffect = fun {
  print 77777777777777
}

print 1 || sideEffect(0)
print 0 && sideEffect(0)

print DELIMITER + 3

anonymousFunctionCall = fun {
    print it
}(20)

print DELIMITER + 4

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

print DELIMITER + 5

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

print DELIMITER + 6

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

print DELIMITER + 7

lets = pass - FUNctions - as - parameters

expect50 = fun { return it == 50 }

callFunctionWith50 = fun {
  return it(50)
}

print callFunctionWith50(expect50)

print DELIMITER + 8

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

print DELIMITER + 9
