hello = lets - start - by - testing - operators

test = 1 <= 2
print test

test = 3 <= 3
print test

test = 1 < 2
print test

test = 1 < 1
print test

test = 5 >= 6
print test

test = 8 >= 5
print test

test = 4 == 4
print test

test = 3 == 4
print test

test = 4 != 4
print test

test = 3 != 4
print test

test = 1 && 0
print test

test = 1 && 1
print test

test = 1 || 0
print test

test = 0 || 0
print test

test = 1, 2, 3
print test

test = 1 & 2
print test

test = 1 & 3
print test

now = testing - IF
if (1) {
	print 3
	if (0) {
		if (1) {
			print 32
		}
	} else {
		print 4
	}
}

now = testing - WHILE
while (0) {
	print 6
}

test = 0
while (test < 5) {
	print test
	test = test + 1
}

now = testing - FUN
yay = fun {
	i = 0
	count = 0
	while (i < it) {
		i = i + 1
		count = count + 10
	}
	print count
	if (count == 10) {
		print 2
	}
}

print yay(5)

print yay(1)
