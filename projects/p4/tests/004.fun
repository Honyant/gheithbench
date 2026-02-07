SECTION = 100
print SECTION
test = argc
help = make - sure - you - dont - double - add - argc - in - identifiers

print argc
argc = 10
print argc 

SECTION = 200
print SECTION
test = assembly - keywords 
help = try - appending - underscores - to - variables 

main = 0
x0 = 0
x1 = 0
ldr = 0
LR = 0
start = 0
data = 0
global = 0
print 1


SECTION = 300
print SECTION
test = large - immediates 
help = try - storing - immediates - in - data - section 

variable = 1000000000000
print variable

SECTION = 400
print SECTION
test = nested - while - loops 
help = make - sure - your - labels - are - unique 

a = 5
out = 0
while (a > 0) {
  b = 5
  while (b > 0) {
    out = out + b
    b = b - 1
  }
  a = a - 1
}

print out

SECTION = 500
print SECTION
test = unsigned - comparison 
help = make - sure - you - use - unsigned - comparators

a = 5
b = 5 - 10

print a < b
print a <= b
print a == b
print a != b
print a > b
print a >= b

SECTION = 600
print SECTION
test = nested - conditional - statements 
help = make - sure - you - use - unique - identifiers 

a = 3
b = 3

if(a > 3) {
  print 1
} else {
  if(b > 3) {
    print 2  
  } else {
    if(a == b) {
      print 3
    }
    print 4
  }
  print 5
}

SECTION = 1000
print SECTION
test = anonymous - fntions
help = make - sure - you - are - storing - fntions - correct

a = fun {
  return it+3
}(2)
print a

SECTION = 2000
print SECTION
test = chained - equalities
help = make - sure - you - are - doing - operations - in - the - right - order

a = 1 < 2 >= 3 <= 5 > 4 <= 10
print a

SECTION = 3000
print SECTION
test = logic - operators
help = make - sure - you - check - for - many - ands - in - a - row

print 1 && 0
print 0 || 1
print 1 && 2 || 0
print 2 && 0 || 0
print 1 || 2 && 0

SECTION = 4000
print SECTION
test = i - f - statements
help = make - sure - you - check - for - correct - branching 

a = 3
b = 2
if(3 > 3) {
  print 0
} else if(a >= b) print 1

if(1) print 0
else print 1

SECTION = 5000
print SECTION
test = recursion - fntions
help = make - sure - you - keep - track - of - the - callstack

gotozero = fun {
  if(it > 1) {
    print it
    return gotozero(it-1)
  } else {
    print 1
  }
}

print gotozero(10)

SECTION = 6000
print SECTION
test = short - circuiting 
help = make - sure - you - short - circuit - when - possible

shouldntrun = fun {
  print 123123123
  return 0
}

print 0

a = 0 && shouldntrun
a = 1 || shouldntrun
a = (0 && 1) && shouldntrun

SECTION = 7000
print SECTION
test = fntions - generators 
help = make - sure - you - handle - multiple - fntions - chained

a = fun {
  return fun {
    return fun {
      return it
    }
  }
}

print a(0)(0)(1)

SECTION = 8000
print SECTION
test = global - it 
help = make - sure - it - is - dependent - on - scope - in - fntions

it = 10

test = fun {
  print it
  it = 9
}

a = test(8)
print it

