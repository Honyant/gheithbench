
note = variable-assignments
print 1111111100

a = 2
b = 1
c = 1
d = 0

note = operators
print 1111111101

print a + a
print a + b
print a + c
print a + d
print b + a
print b + b
print b + c
print b + d
print c + a
print c + b
print c + c
print c + d
print d + a
print d + b
print d + c
print d + d

print a - a
print a - b
print a - c
print a - d
print b - a
print b - b
print b - c
print b - d
print c - a
print c - b
print c - c
print c - d
print d - a
print d - b
print d - c
print d - d

print a * a
print a * b
print a * c
print a * d
print b * a
print b * b
print b * c
print b * d
print c * a
print c * b
print c * c
print c * d
print d * a
print d * b
print d * c
print d * d

print a / a
print a / b
print a / c
print a / d
print b / a
print b / b
print b / c
print b / d
print c / a
print c / b
print c / c
print c / d
print d / a
print d / b
print d / c
print d / d

print a % a
print a % b
print a % c
print a % d
print b % a
print b % b
print b % c
print b % d
print c % a
print c % b
print c % c
print c % d
print d % a
print d % b
print d % c
print d % d

print a <= a
print a <= b
print a <= c
print a <= d
print b <= a
print b <= b
print b <= c
print b <= d
print c <= a
print c <= b
print c <= c
print c <= d
print d <= a
print d <= b
print d <= c
print d <= d

print a < a
print a < b
print a < c
print a < d
print b < a
print b < b
print b < c
print b < d
print c < a
print c < b
print c < c
print c < d
print d < a
print d < b
print d < c
print d < d

print a >= a
print a >= b
print a >= c
print a >= d
print b >= a
print b >= b
print b >= c
print b >= d
print c >= a
print c >= b
print c >= c
print c >= d
print d >= a
print d >= b
print d >= c
print d >= d

print a > a
print a > b
print a > c
print a > d
print b > a
print b > b
print b > c
print b > d
print c > a
print c > b
print c > c
print c > d
print d > a
print d > b
print d > c
print d > d

print a == a
print a == b
print a == c
print a == d
print b == a
print b == b
print b == c
print b == d
print c == a
print c == b
print c == c
print c == d
print d == a
print d == b
print d == c
print d == d

print a != a
print a != b
print a != c
print a != d
print b != a
print b != b
print b != c
print b != d
print c != a
print c != b
print c != c
print c != d
print d != a
print d != b
print d != c
print d != d

print a && a
print a && b
print a && c
print a && d
print b && a
print b && b
print b && c
print b && d
print c && a
print c && b
print c && c
print c && d
print d && a
print d && b
print d && c
print d && d

print a || a
print a || b
print a || c
print a || d
print b || a
print b || b
print b || c
print b || d
print c || a
print c || b
print c || c
print c || d
print d || a
print d || b
print d || c
print d || d

print a , a
print a , b
print a , c
print a , d
print b , a
print b , b
print b , c
print b , d
print c , a
print c , b
print c , c
print c , d
print d , a
print d , b
print d , c
print d , d

print a & a
print a & b
print a & c
print a & d
print b & a
print b & b
print b & c
print b & d
print c & a
print c & b
print c & c
print c & d
print d & a
print d & b
print d & c
print d & d


note = conditionals
print 1111111102

if a print 111111111
if b print 111111111
if c print 111111111
if d print 111111111
if a print 123456789 else print 987654321
if b print 123456789 else print 987654321
if c print 123456789 else print 987654321
if d print 123456789 else print 987654321

note = loops
print 1111111103

while a {print a a = a - 1}
while b {print b b = b - 1}
while c {print c c = c - 1}
while d {print d d = d - 1}

note = reassignment
print 1111111104

a = 5381
b = 33
c = 16
d = 0

note = simple-routine
print 1111111105

square = fun{it = it * it print it return it}
print square(a)
print square(b)
print square(c)
print square(d)

note = routine-called-via-expression
print 1111111106

print (square + 1 - 1)(a)
print (square % 0 + square)(a)

note = nested-function-declarations
print 1111111107

x = fun{
	y = fun{
		print it
		return it
	}
		return y + it
}

note = returning-anonymous-function
print 1111111108

w = fun{
	return fun{
		print it
		return square(it)
	}
}

note = routine-representation-as-unsigned-integer
print 1111111109

print x/0
print w/0

note = sequential-routine-calls
print 11111111010

print w(0)(b)
print (x(a) - a)(b)

note = using-parameter-to-call-routine
print 11111111011

z = fun return it(a)
print z(square)

note = premature-return
print 11111111012

f = fun {
	it = it + 1
	return it
	it = it - 1
}
print f(a)
print f(b)
print f(c)
print f(d)

note = routine-without-return
print 11111111013

g = fun {
	print square(it)
}
print g(c)
note = reassignment
print 11111111014

a = 192
b = 168
c = 24
d = 11
x = 0
y = 0

note = complex-routines-that-exit-from-inside-conditionals-and-loops
print 11111111015

NuMPr1m3FacT0rs = fun {
	count = 0
	factor = 1
	while (1) {
		if (it == 1) return count
		factor = factor + 1
		while (it % factor == 0) {
			it = it/factor
			count = count + 1
		}
	}
}
print NuMPr1m3FacT0rs(a)
print NuMPr1m3FacT0rs(b)
print NuMPr1m3FacT0rs(c)
print NuMPr1m3FacT0rs(d)

note = recursion
print 11111111016

gcd = fun {
	if (x == y) return y
	if x > y x = x % y
	temp = x
	x = y
	y = temp
	return gcd(0)
}
x = a
y = a
print gcd(0)
x = a
y = b
print gcd(0)
x = a
y = c
print gcd(0)
x = a
y = d
print gcd(0)
x = b
y = a
print gcd(0)
x = b
y = b
print gcd(0)
x = b
y = c
print gcd(0)
x = b
y = d
print gcd(0)
x = c
y = a
print gcd(0)
x = c
y = b
print gcd(0)
x = c
y = c
print gcd(0)
x = c
y = d
print gcd(0)
x = d
y = a
print gcd(0)
x = d
y = b
print gcd(0)
x = d
y = c
print gcd(0)
x = d
y = d
print gcd(0)

note = minimal-spacing
print 11111111017

print fun{while it<10if it>0it=it+1}(8)
