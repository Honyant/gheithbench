g = fun {
    it = f(it)
    print it
    return 50
}

f = fun {
    a = argc
    while it < 100 {
        if (a + it  < 70)
            if(a + it > 50)
                print 1
            else
                print 2
        else
            if(a + it > 80)
                print 3
            else
                print 4
        it = it + 1
    }

    return 20
}

print g(0)
