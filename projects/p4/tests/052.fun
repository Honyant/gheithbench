a=1
b=2
c=3
d=4
e=5
f=6
print a<=b
print a>=b
print c>=c
print d<e
print e>f
print f==7
print a==1
print h==g
print c!=d
print c!=3
print c && d
print c || a
print a && g
print a || h
print d & f

x = 3251,9238479823894823,3124982394,4
print x

if x<3 print 45
else print 46

if a >b print 3
else {
    if a==b print 2
    else print 1
}

if (f == 6){
    if (e==5){
        if (d==4)
            print 10
    }
    else print 100
}
else print 1000

while (i<3){
    if (f == 6){
        if (e==5){
            if (d==4)
                print 10
                e=e-1
        }
        else print 100
    }
    else print 1000
    i=i+1
}

x=0
y=0
while (x<5){
    y=0
    while (y<5){
        print x*y
        y=y+1
    }
    x=x+1
}

firstncubes = fun {
    i=1
    answer=0
    while (i<=it){
        answer = answer + i
        i=i+1
    }
    return (answer*answer)
}

print firstncubes(5)

print argc
print argc - it

exit = 54
main = 4
fmt=5
data = 3
text = 2

print exit
print main
print fmt
print data
print text

it = 5

x = fun {
    if it<10 return it
    else {
        it = it - 1
        return x(it)
    }
}

print x(20)

test = 5
f2 = fun {
    test = 6
}


z = 34 || f2(3)
print test

print 0 - 1

z = f2(3), 4
print test
