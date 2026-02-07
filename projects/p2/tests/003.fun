THIS = IS - A - NESTING - LOGIC - TEST
delimiter = 999999999

TEST01 = FUNCTION - NESTING
print delimiter * 100 + 1

nest = fun {
    it = it + 1
    nest2 = fun {
        it = it + 1
        nest3 = fun {
            it = it + 1
            return it
        }
        it = nest3(it)
        return it
    }
    it = nest2(it)
    return it
}
print nest(0)

TEST02 = CONTROL - FLOW - NESTING
print delimiter * 100 + 2

gheithIsCool = 1
a = 5
if gheithIsCool == 0 {
    result = 0
} else if gheithIsCool == 2 {
    result = 2
} else if gheithIsCool == 1 {
    if a == 2 {
        result = 0
    } else result = 1
}
print result

TEST03 = LOOP - NESTING
print delimiter * 100 + 3

count = 0
i = 0
while i < 100 {
    j = 0
    while j < 100 {
        k = 0
        while k < 100 {
            count = count + 1
            k = k + 1
        }
        j = j + 1
    }
    i = i + 1
}
print count

TEST04 = COMMA - FUNCTION - NESTING
print delimiter * 100 + 4

x = 4, 2, 9, fun { return nest(1) + a }
print x(16)

TEST05 = INLINE - CONTROL - FLOW - NESTING
print delimiter * 100 + 5

b = 1
c = 2
if b == 1 if c == 1 print 3 else if c == 3 print 4 else print 5

TEST06 = CONDITION - FUNCTION - NESTING
print delimiter * 100 + 6

d = 0
yes = fun {
    d = d + 1
    return it
}
i = 5
while yes(i) i = i - 1
print d
