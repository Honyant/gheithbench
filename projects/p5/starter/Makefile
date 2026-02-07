CFLAGS = -Werror -Wall -O3 -std=c11 -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free
# 
HEAP_C_FILES=${wildcard heap/*.c}
HEAP_O_FILES=${subst .c,.o,${HEAP_C_FILES}}

C_FILES=${wildcard *.c}
O_FILES=${subst .c,.o,${C_FILES}}
TEST_NAMES=${subst .c,,${C_FILES}}
TEST_OUTS=${addsuffix .out,${TEST_NAMES}}
TEST_DIFFS=${addsuffix .diff,${TEST_NAMES}}
TEST_RESULTS=${addsuffix .result,${TEST_NAMES}}
TEST_TESTS=${addsuffix .test,${TEST_NAMES}}
TEST_RUNS=${addsuffix .run,${TEST_NAMES}}

all : ${TEST_RUNS}

${TEST_RUNS} : %.run : Makefile %.o ${HEAP_O_FILES}
	@-rm -f $*
	-gcc -MMD ${CFLAGS} -o $@ $*.o ${HEAP_O_FILES}

${HEAP_O_FILES} : %.o : Makefile %.c
	-gcc -MMD ${CFLAGS} -c -o $@ $*.c

${O_FILES} : %.o : Makefile %.c
	-gcc -MMD ${CFLAGS} -c -o $@ $*.c

${TEST_OUTS} : %.out : Makefile %.run
	@echo "failed to run" > $*.out
	@rm -f $*.err
	(/usr/bin/time --quiet -o $*.time -f "%E" timeout 7 ./$*.run  > $*.out 2> $*.err); if [ $$? -eq 124 ]; then echo "timeout" > $*.time; fi

${TEST_DIFFS} : %.diff : Makefile %.out %.ok
	@echo "failed to diff" > $*.diff
	-diff -a $*.out $*.ok > $*.diff 2>&1 || true

${TEST_RESULTS} : %.result : Makefile %.diff
	@echo "fail" > $*.result
	(test \! -s $*.diff && echo "pass" > $*.result) || true

${TEST_TESTS} : %.test : Makefile %.result
	@echo "$* ... `cat $*.result` [`cat $*.time`]"

test : ${TEST_TESTS};

clean:
	-rm -rf ${PROG} *.run *.out *.diff *.result *.d *.o heap/*.d heap/*.o *.time *.err

-include *.d
-include heap/*.d

