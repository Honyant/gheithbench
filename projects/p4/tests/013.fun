first = lets + test + that + you + read + args + correctly
note = ever + outputting - 1 + means + something + has + gone + wrong

print argc

delimiter = 9999999999
print delimiter

now = we + test + that + modulus + works + as + intended + and + that + divide + by + 0 + and + mod + by + 0 + is + handled

print 17%2
print 19952%50
a = 0
print 9%a 
print a%a
print 123/a 
print a/a

print delimiter


now = lets + test + that + loops + and + conditionals + can + be + nested

x = 1
y = 1
while x < 3{
    while y < 3 {
        print x + y
        y = y + 1
    }
    y = 0
    x = x + 1
}
z = 0
w = 0
if z == 0
    if w == 1
        print 0-1
    else
        print 1
else
    print 0-1

print delimiter

next = we + test + weird + parantheses + in + FUNction + calls
and = as + we + do + that + we + will + also + confirm + that + it + is + local

it = 0-1
f1 = fun{
    print it
    return 2
}

print (((f1)-1)+1)(((1)))

print delimiter
excellent = now + lets + check + that + nested + FUNctions + work
in = case + this + breaks + for + you + check + that + you + are + properly + branching + and + linking
another = way + to + fail + this + test + is + having + it + not + be + properly + stored
f2 = fun{
    print it
    it = 0-1
    return 4
}
f3 = fun{
    notLocal = f2(it)
    print it + 1
    return 3
}
print f3(1)
print notLocal

print delimiter
another = important + thing + to + check + is + nested + definitions
in = my + first + draft + of + this + prog + all + variables + declared + in + FUNctions + were + zeroed + out + local

f4 = fun{
    variableInFunction = 100
    f5 = fun{
        f6 = fun{
            print variableInFunction
        }
    }
}
print variableInFunction
declare1 = f4(0)
declare2 = f5(0)
declare3 = f6(0)

print delimiter
for = our + final + test + lets + confirm + that + a + mix + of + differnt + conditionals + FUNctions + and + loops + works
and = that + we + can + pass + in + FUNctions + as + parameters

counter = 0
f7 = fun{
    while counter < 10{
        if counter % 2 == 0
            execute = it(counter)
        counter = counter + 1
    }
    return counter + 1
}

f8 = fun{
    print it
}

print f7(f8)