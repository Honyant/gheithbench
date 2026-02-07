comment = basic - math
delim = 999999900
x = 5
y = 10
z = y + x
print(z)

z = y - x
print(z)

z = y / x
print(z)

z = y * x
print(z)

print(delim)

comment = and - statements

if (1 && 0) {
    print(0)
} else {
    print(1)
}

if (1 && 1) {
    print(1)
} else {
    print(0)
}

if (0 && 1) {
    print(0)
} else {
    print(1)
}

if (0 && 0) {
    print(0)
} else {
    print(1)
}

comment = or - statements
print(delim + 1)

if (1 || 0) {
    print(1)
} else {
    print(0)
}

if (1 || 1) {
    print(1)
} else {
    print(0)
}

if (0 || 1) {
    print(1)
} else {
    print(0)
}

if (0 || 0) {
    print(0)
} else {
    print(1)
}

comment = nested - loop - fucntion - calls
print(delim + 2)

factorial = fun {
    val = 1
    while(it > 1){
        val = val * it
        it = it - 1
    } 
    return val
}

print(factorial(5))
print(factorial(0))

comment = nested - fucntion - calls

factorial = fun {
    if(it < 2){
        return 1
    }
    
    return it * factorial(it - 1)
}

print(factorial(5))
print(factorial(0))

