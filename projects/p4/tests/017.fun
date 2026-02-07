delimiter = 999999
print delimiter
test = operators
print 10 <= 1
print 10 <= 10
print 1 > 0 > 0
print 1 >= 1
print 0 > 1
print 5 == 1
print 5 == 5
print 5 != 1
print 5 != 5
print (1 + 2) == 3
print 2 & 3
print 2 && 3
print 1 && 0
print 0 && 1 && 1
print 1 || 0
print 0 || 0
print 5 || 0

test = basic + variables
print delimiter
x = 1
x = 2
print x

x = 1, 2 + 2
print x

test = variable + names
main = 10
print main
exit = 5
print exit
fmt = 15
print fmt

test = nested + WHILE
print delimiter
var = 1
while (var < 5) {
    var = var + 1
    var2 = var + 1
    while (var2 < 5) {
        print var2
        var2 = var2 + 1
    }
    print var
}

test = nested + IF
print delimiter
y = 5
if (y > 0) {
    print 1
    y = y - 1
    if (y == 5) {
        print 0
    } else {
        print 1
        if (y >= 5) {
            print 0
        } else {
            print 1
        }
    }
} else {
    print 0
    if (y == 5) {
        print 0
    } else {
        print 0
    }
}

test = FUNCTIONS
print delimiter
f1 = fun{
    print it
    return 70
}
print f1(50)

f2 = fun{
    while (it < 5) {
        it = it + 1
        print it
    }
}
print f2(1)

print delimiter
print argc

test = FUN + variable
fun = 10

test = anonymous + FUNCTION + and + parantheses + around + FUNCTION
print (fun{
    print 10
    return it
}) (5)

print 1 / 0
print 1 % 0