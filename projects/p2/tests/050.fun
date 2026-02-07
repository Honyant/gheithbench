f = fun {
   if(it > 0) {
        return g(it-1) + 1
    }
    else {
        return 0
    }
}
g = fun {
    if(it > 0) {
        return f(it-1) + 1
    }
    else {
        return 0
    }
}
print f(10)
print g(10)
