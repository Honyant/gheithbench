test=that-argc-has-been-implemented
print argc

test=that-short-circuiting-occurs-for-ands-and-ors-but-not-for-multiplication-and-division
s = 10000
f = fun {
	s = s + it
	return 1
}
q = f(1) / 0
r = f(10) % 0
a = 0 && f(100)
b = 1 || f(1000)
print q
print r
print a
print b
print s

test=global-it
it = 814
print it

test=that-logical-operators-evaluate-to-either-0-or-1
print 5 < 4
print 4 < 5
print 5 <= 4
print 4 <= 5
print 5 > 4
print 4 > 5
print 5 >= 4
print 4 >= 5
print 5 == 5
print 5 == 4
print 5 != 5
print 5 != 4
print 4 && 4
print 4 && 0
print 0 && 4
print 0 && 0
print 4 || 4
print 4 || 0
print 0 || 4
print 0 || 0

test=moving-large-immediates-and-sign-issues-with-division-and-modulo-and-printing
verylargenumber = 18446744073709551579
print verylargenumber
print verylargenumber / 2
print verylargenumber % 2

same=number-but-in-unsimplified-form-to-test-correctness-of-constant-folding
verylargenumber = 0 - 37
print verylargenumber
print verylargenumber / 2
print verylargenumber % 2
