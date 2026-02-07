NOTE = THAT-ARGC-IS-10
delimiter = 12345


TEST = 1
print delimiter
print TEST
CHECKING = NESTED-IF-ELSE-BRANCHING
OUTPUT = 7007-AND-33333

if (20 < 5) {
    print 9999
} else if (20>=5) {
    print 1001
    if (10 >= 90) {
        if (2 >= 3) {
            print 2002
        } else if (12>=11) {
            print 3003
        } else {
            print 4004
        }
    } else if (5 >= 20) {
        print 5005
    } else {
        print 6006
    }
} else if (50 >= 950) {
    print 7007
    if (10 >= 9) {
        if (2 >= 3) {
            print 8008
        } else if (1>=11) {
            print 9009
        } else {
            print 10010
        }
    }
} else if (500 >= 0) {
    print 11011 
} else {
    print 12012
    if (10 >= 9) {
        if (2 >= 3) {
            print 13013
        } else if (1>=11) {
            print 14014
        } else {
            print 15015
        }
    }
}


TEST = 2
print delimiter
print TEST
CHECKING = IF-ELSEIF-ELSE-IN-LOOP

index1 = argc
while index1 >= 1 {
    if (index1 >= 9) {
        print 99
    } else if (index1 >= 5) {
        print 55
    } else if index1 >= 2 {
        print 22
    } else {
        print 11
    }
    index1 = index1 - 1
}


TEST = 3
print delimiter
print TEST
CHECKING = MODULO-AND-DIV-BY-0

x = 0
print 3 / x
print 3 % 0
print 0 % 0 
print 3 % x
print x / x


TEST = 4
print delimiter
print TEST
CHECKING = NESTED-FUNCTION-WITH-PARENTHESIS-AND-LOCAL-IT

nestedfucntioncall = fun {
    return fun {
        return 3
    }
}

print (4*((2*(nestedfucntioncall)/2)(1234))/4)(shouldret3)

weirdfucn = fun {
    print it
    it = 8888
    return fun {
        print it
        it = 9999
        return fun {
            print it
            return it
        }
    }
}

trash = weirdfucn(1)(2)(3)
print it


TEST = 5
print delimiter
print TEST
CHECKING = REDEFINITION-OF-FUNCTIONS-AND-VARIABLES

var1 = fun {print 7777}
var2 = var1
var1 = 8008
print var1
print var2(trash)
var2 = var2
print var2(trash)
var1 = var1 * 2
print var1


TEST = 6
print delimiter
print TEST 
CHECKING = THAT-INEQUALITY-OPERATORS-WORK-OUTSIDE-OF-BRANCHING
print 3 >= 2
print 3 >= 3
print 3 >= 4

print 3 <= 2
print 3 <= 3
print 3 <= 4

print 3 > 2
print 3 > 3
print 3 > 4

print 3 < 2
print 3 < 3
print 3 < 4


