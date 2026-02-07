comment = basic - fucntion - calls

isalpha = fun {
    return (it >= 65 && it <= 90) || (it >= 97 && it <= 122)
}

isnum = fun {
    return (it >= 48 && it <= 57)
}

isalnum = fun {
    return isalpha(it) || isnum(it)
}

print isalnum(65)
print isalpha(65)
print isnum(65)


print 1000000

comment = basic - short - circuiting

if 1 && 1
    print 1
else 
    print 0

if 1 && 0
    print 0
else 
    print 1

if 0 && 1
    print 0
else 
    print 1

if 0 && 0
    print 0
else 
    print 1

if 1 || 1
    print 1
else 
    print 0

if 1 || 0
    print 1
else 
    print 0

if 0 || 1
    print 1
else 
    print 0

if 0 || 0
    print 0
else 
    print 1


print 2000000
comment = short - circuiting - with - fucntions

short = fun {
    print 666
    return 1
}


if 0 && short(0)
    print 0
else 
    print 1

if 1 || short(0)
    print 1
else 
    print 0



print 3000000
comment = basic - reccursion

triangle = fun {

    if (it > 1){
        run = triangle(it - 1)
    }
    i = 0
    temp = 0
    while i < it{
        temp = (temp * 10) + 1
        i = i + 1
    }
    print temp
}

run = triangle(5)
run = triangle(10)


print 4000000
comment = nested - while - loops - in - fucntion

pyramid = fun {

    i = 0
    while i < it {
        ones = it - i
        string = 0
        j = 0

        while j < ones {
            string = (string * 10) + 7
            j = j + 1
        }

        out = (string * 10)
        j = 0
        while j < i {
            out = (out * 100) 
            j = j + 1
        }

        j = 0

        while j < ones {
            out = out * 10
            j = j + 1
        }

        out = out + string
        print out
        i = i + 1
    }
}

run = pyramid(5)


print 5000000
comment = exiting - in - middle - of - fucntion

test = fun {
    if (it){
        print 1
        return 5
    }

    print 0
    return 8
}

print test(1)
print test(0)

