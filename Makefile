CC=gcc
CFLAGS=-g

SRCDIR=src
INCLDIR=include
LIBDIR=lib

all: mapreduce mapper reducer

mapreduce: $(SRCDIR)/mapreduce.c $(LIBDIR)/utils.o mapper reducer
	@$(CC) $(CFLAGS) -I$(INCLDIR) $(LIBDIR)/utils.o $(SRCDIR)/mapreduce.c -o mapreduce

mapper: $(SRCDIR)/mapper.c $(LIBDIR)/utils.o
	@$(CC) $(CFLAGS) -I$(INCLDIR) $(LIBDIR)/utils.o $(SRCDIR)/mapper.c -o mapper

reducer: $(SRCDIR)/reducer.c $(LIBDIR)/utils.o
	@$(CC) $(CFLAGS) -I$(INCLDIR)  $(LIBDIR)/utils.o $(SRCDIR)/reducer.c -o reducer

.PHONY: clean t1 setup_test_env test_mapper test_reducer test_master verify_test_result

setup_test_env:
	@yes | cp ./solutionexe/mapper ./testing/
	@yes | cp ./solutionexe/reducer ./testing/
	@yes | cp ./solutionexe/mapreduce ./testing/
	@rm -rfd ./testing/output

test_mapper: mapper setup_test_env
	@mv ./mapper ./testing/mapper
	@cd ./testing/ && make compile
	@make run_all_testcase

test_reducer: reducer setup_test_env
	@mv ./reducer ./testing/reducer
	@cd ./testing/ && make compile
	@make run_all_testcase

test_mapreduce: mapreduce setup_test_env
	@mv ./mapreduce ./testing/mapreduce
	@cd ./testing/ && make compile
	@make run_all_testcase

test_integration: mapreduce reducer mapper setup_test_env
	@mv ./mapreduce ./testing/mapreduce
	@mv ./reducer ./testing/reducer
	@mv ./mapper ./testing/mapper
	@cd ./testing/ && make compile
	@make run_all_testcase

run_all_testcase:
	@find ./test -name '*.txt' -exec echo Running test on test file {} \; -exec cp {} ./solutionexe/test/T1/F1.txt \; -exec cp {} ./testing/test/T1/F1.txt \; -exec make verify_test_result \;

verify_test_result:
	@cd ./solutionexe && make run
	@rm -f sample_output.txt 
	@find ./solutionexe/output/ReduceOut/ -name '*.txt' -exec cat {} >> sample_output.txt \;
	@cd ./testing && make run
	@rm -f test_program_output.txt
	@find ./testing/output/ReduceOut/ -name '*.txt' -exec cat {} >> test_program_output.txt \;
	@sort ./sample_output.txt > sorted_sample_output.txt
	@sort ./test_program_output.txt > sorted_test_program_output.txt
	@if diff sorted_sample_output.txt sorted_test_program_output.txt; then echo "\033[1;32m *** Test case passed ***\033[0m"; make clean; else echo "\033[1;31m *** Did not pass testcase *** \033[0m"; fi
	
#500KB
t1:
# 	make -i clean
	make
	./mapreduce 5 2 test/T1/F1.txt

clean:
	rm -f mapreduce mapper reducer
	rm -rf output
	rm -f sample_output.txt sorted_sample_output.txt test_program_output.txt sorted_test_program_output.txt 
