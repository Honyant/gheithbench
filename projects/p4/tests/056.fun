print first - we - will - test - conditional - statements - with - complex - syntax
if 1 print 1 
if 0 print 2
if x print 3
x = 10
if x print 4
if (1) print 6
if (((((1))))) print 7
if 1 < 2 print 8 else print 0
if (1 < 2) print 9 else {print 0}
if (9 && 13) {if 1 print 10} else print 0
if (9 & 13) {print 11} else {print 0}
if (x) {if y print 0 else {print 12}} else print 0
y = 1
if x {if (y) print 13 else print 0} else print 0

print now - we - will - test - some - new - operators
print 0 && 0 
print 0 && 1
print 1 && (x-10) 
print 1 && 1
print 10 & 12
print x & 12
print (x-1+1) & (x+2)
print x || 0
print 1, 2, 3, x + 4

print now - we - will - quickly - test - loops
x = 2
while (x && x + 1){
	print x
	x = x - 1
	if (x != 0){
		print x + 1
	} else {
		while 0 print 1
		print 0
	}
}

print now - we - will - test - methods - and - recursion
foo = fun return it * 2
bar = fun return foo(it) / 2
foobar = fun {
	z = bar(it)
	return z + 1
}
print foo(1)
print bar(2)
print (foobar(10))

f = fun {
	if (it == 100)
		return it
	else 
		return f(it + 1)
}
print f(1)

a = argc - should - be - greater - than - zero - always
if (argc > 0)
	print 12

print i - will - now - test - reassignment - of - variables
a = 100
print a 
b = a 
print b 
argc = b 
b = 15
a = b 
a = 200
print b 
b = 10 
print b
print a 
