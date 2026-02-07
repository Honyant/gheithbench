delimiter = 999999900

relational = operators

print 3 >= 2
print 3 >= 3
print 3 >= 6

print 1 <= 0
print 1 <= 1
print 1 <= 9

print 9999 > 7
print 9999 > 9999
print 9999 > 99999

print 9999 < 0
print 9999 < 9999
print 9999 < 999999

print 1 == 0
print 1 == 1
print 1 == 9

print 1 != 0
print 1 != 1
print 1 != 9

print 3 >= 2 >= 1
print 3 == 3 == 1
print 3 == 3 < 1
print 0 != 0 == 0


print delimiter + 1
logical = operators

print 3 && 1
print 3 && 0
print 0 && 1
print 0 && 0

print 3 || 1
print 3 || 0
print 0 || 1
print 0 || 0

print 51 & 1059


print delimiter + 2
test = spacing + for + bitwise + and
make = sure + that + your + bitwise + and + and + logical + and + are + detected + properly

print 51&1059
print 51& 1059
print 51 &1059

print 51&&1059
print 51&& 1059
print 51 &&1059


print delimiter + 3
test = comma

print 0,2
print 0, 3*3,5
print 0, 4, 4, 0, 3*5/2


print delimiter + 4
test = if + statements

if 3 > 2 print 5
else print 4

if (3 > 2) {print 5}

if (3 < 2) {print 5}

if 1
{
  if 0 {
    print 1
  }
  else {
    if 0 print 2
    else print 3
  }
}


print delimiter + 5
test = that + code + that + does + not + run + does + not + produce + side + effects

a = 3
if (0) {
  a = 6
}
print a


print delimiter + 6
test = while + loops

a = 1
i = 10
while i > 0 {
  a = a * 2
  i = i - 1
}
print a

i = 5
a = 0
while i > 0 {
  a = a + 1
  i = i / 2
}
print a


print delimiter + 7
test = fnctions

a = fun {
  print it
}

print a(5)

b = fun {
  print a(it)
}

x = b(4)

a = fun {
  return 1
}

print a(2)


print delimiter + 8
test = a + recursive + fnction

c = fun {
  if (it == 0) return 1
  else return c(it - 1) * 2
}

print c(20)


print delimiter + 9
test = thatIt + still + works + at + the + global + scope

it = 5
print it


print delimiter + 10
test = thatIt + does + not + interfere + with + local + it

b = fun {
  it = it + 7
  print it
}

x = b(4)
