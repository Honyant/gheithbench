strategy = several + other + people + already + wrote + stress + tests + so + I + will + focus + on + syntax

note = testing + printing + literal + values
print 0
print 256
print 18446744073709551615
print 18446744073709551617

note = printing + results + of + operations
print 101 + 9
print 101 - 9
print 101 * 9
print 101 / 9
print 101 % 9
print 0 - 1

note = testing + assignments + and + reassignments
print variable
variable = 3
print variable
variable = 5
print variable
variable = variable + 7
print variable

note = illegal-ish + operations
print variable/0
print variable%0
print 1/0
print 1%0
print 1/(1-1)
print 1%(1-1)
print 0/0
print 0%0

note = testing + parentheses
print 6 * 5 % 4 + 3 / 2 - 1
print (6 * (5 % 4) + 3) / (2 - 1)

note = defining + some + variables
a = 1
b = 4
c = 7
d = 10
e = 13
f = 16
print a
print b
print c
print d
print e
print f

note = mixing + assignments + variables + literals + parentheses
print ((((c+e)/b)-a)*f)%d
result = (3 * (b + 5) - a)/e + f
print result
print (result * 5381 + 33) / 256

note = testing + variable + differentiation
print thisisquitealongvariablename
thisisquitealongvariablename = 1
print thisisquitealongvariablename
print thisisquitealongvariableqqqq
thisisquitealongvariablename = 2
print thisisquitealongvariableqqqq

note = strange + variable + name
Id3nT1f13r = 78
print Id3nT1f13r

note = extraneous + stuff
print (5)
print ((((((((((((((((((((((5))))))))))))))))))))))
print 000000005
print                          5 *          (((              c)+b)   )
print (((((((((((a * b * c * d * e * f  
print (f *
(e +
    d)/
c) -    a %
b



note = should + error
I + can + only + test + one + error = sad