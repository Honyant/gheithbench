TEST_DIR=./tests
V_FILES=${wildcard *.v} ${wildcard ${TEST_DIR}/*.v}
OK_FILES=${wildcard *.ok} ${wildcard ${TEST_DIR}/*.ok}
TEST_NAMES=${sort ${subst .ok,,${OK_FILES}}}
TEST_RAWS=${addsuffix .raw,${TEST_NAMES}}
TEST_OUTS=${addsuffix .out,${TEST_NAMES}}
TEST_DIFFS=${addsuffix .diff,${TEST_NAMES}}
TEST_RESULTS=${addsuffix .result,${TEST_NAMES}}
TEST_TESTS=${addsuffix .test,${TEST_NAMES}}
TEST_VCDS=${addsuffix .vcd,${TEST_NAMES}}


all : cpu

cpu : Makefile ${V_FILES}
	iverilog -o cpu ${V_FILES}

${TEST_RAWS} : %.raw : Makefile cpu %.hex
	@echo "failed to run" > $*.raw
	@rm -f $*.cycles
	-cp $*.hex mem.hex
	(/usr/bin/time --quiet -o $*.time -f "%E" timeout 10 ./cpu > $*.raw 2> $*.cycles); if [ $$? -eq 124 ]; then echo "timeout" > $*.time; fi
	-cp cpu.vcd $*.vcd

${TEST_OUTS} : %.out : Makefile %.raw
	@echo "no output" > $*.out
	-grep -v "VCD info: dumpfile cpu.vcd opened for output" $*.raw > $*.out

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
	-rm -rf cpu *.out *.diff *.raw *.out *.result *.time *.cycles
	-rm -rf ${TEST_DIR}/*.out ${TEST_DIR}/*.diff ${TEST_DIR}/*.result ${TEST_DIR}/*.d ${TEST_DIR}/*.raw ${TEST_DIR}/*.time ${TEST_DIR}/*.cycles ${TEST_DIR}/*.vcd 
