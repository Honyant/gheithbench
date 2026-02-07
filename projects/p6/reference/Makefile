CFLAGS = -Werror -Wall -O3 -g -std=c11 -I ./

TEST_DIR = ./tests
LIB_C_FILES=${wildcard src/*.c}
LIB_S_FILES=${wildcard src/*.s}
LIB_OC_FILES=${subst .c,.o,${LIB_C_FILES}}
LIB_OS_FILES=${subst .s,.o,${LIB_S_FILES}}
LIB_O_FILES=${LIB_OS_FILES} ${LIB_OC_FILES}
C_FILES=${wildcard *.c} ${wildcard ${TEST_DIR}/*.c}
O_FILES=${subst .c,.o,${C_FILES}}
TEST_NAMES=${sort ${subst .c,,${C_FILES}}}
TEST_OUTS=${addsuffix .out,${TEST_NAMES}}
TEST_DIFFS=${addsuffix .diff,${TEST_NAMES}}
TEST_RESULTS=${addsuffix .result,${TEST_NAMES}}
TEST_TESTS=${addsuffix .test,${TEST_NAMES}}
TEST_RUNS=${addsuffix .run,${TEST_NAMES}}

all : ${TEST_RUNS}

${TEST_RUNS} : %.run : Makefile %.o ${LIB_O_FILES}
	@-rm -f $*
	-gcc -MMD ${CFLAGS} -o $@ $*.o ${LIB_O_FILES}

${LIB_OC_FILES} : %.o : Makefile %.c
	-gcc -MMD ${CFLAGS} -c -o $@ $*.c

${LIB_OS_FILES} : %.o : Makefile %.s
	-gcc -MMD ${CFLAGS} -c -o $@ $*.s

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
	-rm -rf ${PROG} *.out *.diff *.result *.d *.o src/*.d src/*.o *.time *.err *.run
	-rm -rf ${TEST_DIR}/*.out ${TEST_DIR}/*.diff ${TEST_DIR}/*.result ${TEST_DIR}/*.d ${TEST_DIR}/*.o ${TEST_DIR}/*.time ${TEST_DIR}/*.err ${TEST_DIR}/*.run

-include *.d
-include src/*.d

