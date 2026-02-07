test=anonymous-FUNctions
exarg=is-an-external-or-extra-argument

deriv1 = fun return (exarg(it + 1) - exarg(it - 1)) / 2

deriv2 = fun return (it(exarg + 1) - it(exarg - 1)) / 2

exarg=calculates-the-exponential-FUNction-with-arguments-up-to-6
exarg = fun {
	denom = 1
	num = 1
	i = 1
	pow = 1
	while (i <= 16) {
		pow = pow * it
		num = i * num + pow
		denom = i * denom
		i = i + 1	
	}
	return num / denom	
}

derivative=estimate-is-18-percent-larger-than-actual-derivative-for-purely-mathematical-reasons
print exarg(5)
print deriv1(fun{
	return it
	test=that-the-FUNction-RETURNs-properly-by-corrupting-exarg-otherwise
	you=might-get-a-segfault-here-and-that-would-be-bad
	exarg = 2989
}(5))

derivative=estimate-is-exact
exarg = 5
print deriv2(fun return it * it)
print 2 * 5
