this = test + is + focused + on + fuunctions

first = lets + just + do + a + simple + one 
f1 = fun {
    print it
}
execute = f1(1)

now = lets + check + that + we + can + not + use + brackets
f2 = fun
    print it + 1
execute = f2(1)

notably = if + the + first + statement + is + an + if
we = should + also + include + a + subsequent + else
f3 = fun   
    if(it)
        print 4
    else
        print 3
execute = f3(0)

since = fuunctions + are + uints + we + can + assign + other + variables + to + them
fClone = f3
execute = fClone(1)

even = better + fuunctions + can + be + passed + into + other + fuunctions
runTenTimes = fun{
    num = 5
    while (num < 16){
        execute = it(num)
        num = num + 1
    }
}
execute = runTenTimes(f1)

its = kind - of - gross - but - we - should - also - be - able - to - put - the - name - of - the - fuunction - in -its -own -parentheses
output16 = fun{
    print 16
}
execute = (output16)(f1)

for = our - finale - lets - have - a - fuunction - define - another -fuunction -and - output - it
giveFuunction = fun{
    interiorFuunction = fun{
        print it
    }
    return interiorFuunction
}
execute = (giveFuunction(0))(17)