sum = 0

f = fun {

    sum = sum * it

    if it == 1
      return 1
    else
      return f(it-1)
}

why = f(2500000)

print sum
