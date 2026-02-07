delim = 9999999999999999
smalldelim = 99999

print delim

can = you, resize, your, call, stack
uselessrecurse = fun {
    if(it == 1)
        return 1
    how = about, redefining, a, variable, over, and, over, it
    return uselessrecurse(it - 1) + 1
}
print uselessrecurse(1000)

print delim

what = about, branching, if, statements, and, f0nctions
fizzbuzz = fun {
    if (it % 5 == 0 || it % 3 == 0)
        if (it % 3 == 0 && it % 5 != 0)
            print 1
        else 
            if (it % 5 == 0 && it % 3 != 0)
                print 2
            else 
                print 12
    else
        return 0
}
execute = fizzbuzz(1)
execute = fizzbuzz(3)
execute = fizzbuzz(5)
execute = fizzbuzz(15)

print delim

can = you, take, f0nctions, as, parameters

i = 0
runtill30 = fun
    while i <= 30{
        dont = forget, about, running, while, loops
        print i
        execute = it(i)
        i = i + 1
        print smalldelim
    }
execute = runtill30(fizzbuzz)

print delim

dont = run, code, inside, uncalled, f0nctions
forever = fun
    while 1
        print 1

make = sure, you, short, circuit, properly
if((0 && forever(1)) || (1 || forever(1)))
    print 1

print delim

lets = finish, things, off, with, an, some, anonymous, f0nction, calls
print fun {
            return it % 2 == 0
        }(3)

print delim

can = a, f0nction, return, itself
f0nctionreturner = fun {
    if it % 2 == 0
        return fun
            return it % 2 == 0
    else        
        return f0nctionreturner
} 
print f0nctionreturner(3)(7)(11)(17)(2)(2)
print f0nctionreturner(3)(7)(11)(17)(2)(3)

print delim

can = you, pass, a, f0nction, as, a, parameter, inline
i = 25
execute = runtill30(fun print it * 10)
print delim
same = thing, but, with, commas
i = 25
f0nction = just, some, random, junk, fun print it * 10
execute = runtill30(f0nction)

print delim