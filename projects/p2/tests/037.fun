description = separator + is + used + to + group + related + sections + for + debugging + purposes

separator = 111111111

description = simple + while + loop + test + that + prints + fibonocci + sequence
description = evaluate + complex + expression + for + loop + condition
x = 1
y = 1
var = 5
while (x < (((var * 4)/2)) + 10) {
    z = y + x
    print z
    x = y
    y = z
}

print separator 

description = nested + loops + and + conditionals + that + print + prime + numbers
description = incorporates + els + statement + and + newly + implemented + operators
description = els + statement + with + no + content + inside
description = varied + use + of + curly + braces
x = 2
y = 2
z = 1
while (x <= 20) {
    while (y < x) {
        if (x % y == 0) {
            z = 0
        }
        else {
            
        }
        y = y+1
    }
    if (z == 1)
        print x
    z = 1
    x = x + 1
    y = 2
}

print separator 

description = fnction + call + from + inside + another + fnction
description = the + it + variable + is + local + to + fnction + scope
x = fun{return it + 5}
y = fun{return x(it) + it}

print y(5)

description = returning + a + fnction + and + passing + argument + to + it
y = fun {return it + 5}
x = fun {return y}

print x(5)(7)
