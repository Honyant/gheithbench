print nonExistentVariableTest

belowZeroTest = 0 - 1
print belowZeroTest

divideByZeroTest = 0 / 0
print divideByZeroTest
modByZeroTest = 1 % 0
print modByZeroTest

maxuint64 = 18446744073709551615
overflowTest = maxuint64 + 1
print overflowTest

hashCollisionTest = belowZeroTest + overflowTest
print hashCollisionTest
hashCollisionTest = hashCollisionTest + 1
print hashCollisionTest

belowZeroTest = belowZeroTest + belowZeroTest
print belowZeroTest
belowZeroTest = belowZeroTest + belowZeroTest
print belowZeroTest

mathTest = 1
print mathTest + 1
mathTest = mathTest * 300 + 20
print mathTest
mathTest = mathTest * mathTest * mathTest
print mathTest
mathTest = mathTest - maxuint64
print mathTest

parenthesisTest = (20 + 30) * 40
print parenthesisTest
parenthesisTest = (((((((((parenthesisTest))))))+2)))
print parenthesisTest
parenthesisTest = (((((((((parenthesisTest)))*2)))+2)))
print parenthesisTest

moduloTest = 20 % 3
print moduloTest
moduloTest = (0 - 20) % 3
print moduloTest

divisionTest = 30 / 3
print divisionTest
print DivisionTest / 3
print 0 / 1

stressTest = maxuint64
stressTest = stressTest + stressTest
print stressTest
stressTest = stressTest + stressTest
print stressTest
stressTest = stressTest + stressTest
print stressTest
stressTest = stressTest + stressTest
print stressTest
stressTest = stressTest * stressTest
print stressTest
stressTest = stressTest * stressTest
print stressTest
stressTest = stressTest * stressTest
print stressTest
stressTest = stressTest * stressTest
print stressTest
stressTest = stressTest * stressTest
print stressTest