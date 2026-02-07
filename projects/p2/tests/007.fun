this = will - test - your - ability - to
perform = recursion
divider = 69
a = fun {
   if(it == 0) return 20
   ret = 0
   while (it > 0){
       ret = ret + a(it - 1)
       it = it - 1
   }
   return ret
}
print divider
print a(1)
print ret
print divider

next = we - are - going - to - test - fucntions - chained - within - each - other

b = fun return fun return fun return fun return fun return a
c = b(10)(10)(10)(10)(10)

print c == a
print divider

then = lets + test - if + statements + with + no + brackets

d = fun if it == 20 return 20 else return it(3)

print d(20)

print divider

finally = lets + test + early + returns
lets = also + test + fucntion + chaining + for + fucn

e = fun {

    if(it == 20)
        return 15

    return it(3)

    return 0
}

print e(20)
print e(a(1))
