testing = invalidVars  - should - be - zero
print testing
print invalid

testing = simpleArithmetic
print 100
x = 30
print x + 30
a = 15
b = 5
c = a + b
print c
e = a - b
print e
testing = division
d = a/b
print d
testing = divisionNotInt
d = a/6
print d
testing = mulitiplication
f = a*b
print f
testing = negativeOutput
e = b - a
print e

testing = mod
print x%14
print x%15
print 19%b

testing = orderOfOperations
x = 2
y = 3
z = 4
w = x - y*z
print w
w = x + z/x
print w
w = x*w + z/x
print w

testing = parentheses
w = x + (12/5)
print w
w = 2 * (x + y) / 4
print w
w = (2-x-z/4)
print w

testing = overflow  - should - be - zero
overflow = 18446744073709551615
print overflow + 1

testing = divisonByZero  - should - be - zero
print y/0
print 0/y

testing = divisionByUndeclared - should - be - zero
print y/undec
print undec/undec2

testing = string - should - be - failure
string = "str"
print str

testing = textAfterFailure
extraVar = 47
print extraVar

this is just extra text that's printed after the fail