print 99999901
print 99999901
print 99999901
test = anonymous - fntions
help = make - sure - you - are - storing - fntions - correct

a = fun {
  return it+3
}(2)
print a

print 99999902
print 99999902
print 99999902
test = chained - equalities
help = make - sure - you - are - doing - operations - in - the - right - order

a = 1 < 2 >= 3 <= 5 > 4 <= 10
print a

print 99999903
print 99999903
print 99999903
test = logic - operators
help = make - sure - you - check - for - many - ands - in - a - row

print 1 && 0
print 0 || 1
print 1 && 2 || 0
print 2 && 0 || 0
print 1 || 2 && 0

print 99999904
print 99999904
print 99999904
test = if - statements
help = make - sure - you - check - for - many - ands - in - a - row

if(3 > 3) {
  print 0
} else if(a >= b) print 1

if(1) print 0
else print 1

print 99999905
print 99999905
print 99999905
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

print 99999906
print 99999906
print 99999906
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

print 99999907
print 99999907
print 99999907
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

print 99999908
print 99999908
print 99999908
test = global - it 
help = make - sure - it - is - dependent - on - scope - in - fntions

it = 10

test = fun {
  print it
  it = 9
}

a = test(8)
print it

