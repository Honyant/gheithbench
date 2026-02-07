a = 13
b = (a + 125) * (a - 50)
print b
c = b * (a + 5)
print c
maxuint64 = 18446744073709551615
overflowTest = maxuint64 + 1
print overflowTest
belowZeroTest = 0 - 1
print belowZeroTest
d = c / 3
print d
e = (d + maxuint64) % a
print e
f = (b - e) * (d + overflowTest)
print f
g = (f / (belowZeroTest + 2)) * ((e + 1) % (overflowTest - 1))
print g
h = i + 2
print h
j = j + 1
print j
