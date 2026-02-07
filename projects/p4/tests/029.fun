delimiter = 111111111111
test = short - circuiting - for - and
a = fun {
  print 1
}

print 0 && a(0)
print a(0) && a(0)
print delimiter


test = short - circuiting - for - or
b = fun {
  print 1
  return 1
}

print 1 || b(0)
print b(0) || b(0)


print delimiter * 2
test = that + ints + are + formatted + with + percent + ld

a = 9223372036854775807
print a
a = a + 1
print a


print delimiter * 3
test = that + arm + instructions + are + doing + unsigned + comparisons
check = if + you + use + cset + replace + le + with + ls, ge + with + hs, etc

print (0 - 5) < 0
print 0 > (0 - 5)
print (0 - 5) <= 0
print 0 >= (0 - 5)
