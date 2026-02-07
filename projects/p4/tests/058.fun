Nested = Function - Checks

Checks = Register - Overwriting

f1 = fun
	return it
f2 = fun
	return f1(it + 1)
f3 = fun
	return f2(it + 2)
f4 = fun
	return f3(it + 3)
f5 = fun
	return f4(it + 4)
f6 = fun
	return f5(it + 5)
f7 = fun
	return f6(it + 6)
f8 = fun
	return f7(it + 7)
f9 = fun
	return f8(it + 8)
f10 = fun
	return f9(it + 9)
f11 = fun
	return f10(it + 10)
f12 = fun
	return f11(it + 11)
f13 = fun
	return f12(it + 12)
f14 = fun
	return f13(it + 13)
f15 = fun
	return f14(it + 14)
f16 = fun
	return f15(it + 15)
f17 = fun
	return f16(it + 16)
f18 = fun
	return f17(it + 17)
f19 = fun
	return f18(it + 18)
f20 = fun
	return f19(it + 19)


print f1(f2(f3(f4(f5(f6(f7(f8(f9(f10(f11(f12(f13(f14(f15(0)))))))))))))))

print f1(1)
print f2(2)
print f3(3)
print f4(4)
print f5(5)
print f6(6)
print f7(7)
print f8(8)
print f9(9)
print f10(10)
print f11(11)
print f12(12)
print f13(13)
print f14(14)
print f15(15)
print f16(16)
print f17(17)
print f18(18)
print f19(19)
print f20(20)