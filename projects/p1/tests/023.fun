testing = basicAssignment
a = 3
print a

test = arithmeticOps
b = 3 + 5
print b
b = a * 2
print b
b = a % 7
print b
b = a / 2
print b

test = selfUpdate
c = 7
c = c * 2
print c
c = c - 3
print c
c = c / 5
print c

test = expressions
d = (a + 7) * 2
print d
d = (17 + 3) / 2 * a + (d - 64) + 40 % 6
print d

test = negative
e = 0
f = e - 49 - 20
print e
print f

test = largeVals
maxInt = 18446744073709551615
g = maxInt + 1
print g

test = undefinedVars
print thisVarDoesNotExist
print thisVarDoesNotExist + anotherVarThatDoesntExist * thirdVar

test = invalidArithmeticDivideByZero
h = h / 0
print h