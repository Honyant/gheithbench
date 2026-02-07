testing = fnctions - and - operators
op = fun {
   print it <= 10
   print it >= 10
   print it < 30
   print it > 30
   print it == 15
   print it != 15
   print it == 15 && it != 15
   print it == 15 || it != 15
   print it, 12, 13, 14 + 10
   return it + 25
}
print op(15)
print op(50)

testing = modanddiv
notes = both - should - be - zro
print 5 % 0
print 5 / 0

testing = argsee
print argc

testing = recrsion
recur = fun {
   print it
   if it <= 0 {
      return 0
   } else {
      return recur(it - 1)
   }
}
ert = recur(5)

testing = anonfnc
anon = fun {
   print it
}(1234567)

testing = rturn
notes = should - not - prnt - second - statement
rtrn = fun {
   return 12
   print 3030
}
print rtrn(0)


testing = short - circuiting
circuit = fun {
   print(it)
   return 1
}
notes = should - short - circui
if (1 || circuit(18)) {
   print 20
}
notes = should - not - short - circui
if (1 && circuit(18)) {
   print 20
}


testing = f + lse - nested
notes = make - sure - the - right - lse - executes
x = 10
if x < 15 {
   print x
   y = 20
   if y > 35 {
      print y
   } else {
      print 7
      z = 50
      if z > 40 {
         print z
      } else {
         print 90
      }
   }
   print x
   w = 0
   while (w < 3) {
      w = w + 1
      print w
   }
} else {
   print 1000
}


testing = whle - nested
notes = the - inner - one - should - not - execute
x = 10
while (x < 12) {
    x = x + 1
    print x
    y = x * 2
    if (y > x) {
        print y
    }
    z = 0
    while (z > 3) {
        z = z + 1
        print z
    }
}


testing = extra - statement - at - the - end
y = 34567
print y