# make 1: make day 1 executable
# make 1p2: make day 1 executable, part 2
# make debug1: make day 1 debug executable
# make debug1p2: make day 1 debug executable, part 2
# make run1: make and run day 1 executable
# make run1p2: make and run day 1 executable, part 2
# make test1: make and run day 1 executable over test input
# make test1p2: make and run day 1 part 2 executable over test input
# make dbg1: make and run debug day 1 executable
# make dbg1p2: make and run debug day 1 executable, part 2
# make dbgtest1: make and run debug day 1 executable over test input
# make dbgtest1p2: make and run debug day 1 executable, part 2 over test input
# make clean

CC = gcc
_CFLAGS = $(CFLAGS) -Wall
YEAR=2023

all: 1

debug%:
	make CFLAGS="-DDEBUG $(CFLAGS)" $*
%p2:
	make CFLAGS="-DPART2 $(CFLAGS)" $*

$(YEAR)_%: $(YEAR)_%.c grid.o chargrid.o get_next_line.o
	$(CC) $(DEBUG) -Iinclude $(_CFLAGS) -o $@ $^

run%: %
	./$(YEAR)_`printf %02d $*` input/input_$(YEAR)_`printf %02d $*`
dbg%: debug%
	./$(YEAR)_`printf %02d $*` input/input_$(YEAR)_`printf %02d $*`
test%: %
	./$(YEAR)_`printf %02d $*` test/test_$(YEAR)_`printf %02d $*`
dbgtest%: debug%
	./$(YEAR)_`printf %02d $*` test/test_$(YEAR)_`printf %02d $*`

1 2 3 4 5 6 7 8 9: %: $(YEAR)_0%
10 11 12 13 14 15 16 17 18 19: %: $(YEAR)_%
20 21 22 23 24 25 26 27 28 29: %: $(YEAR)_%
30 31: %: $(YEAR)_%

test%: $(YEAR)_%
	cat test/${^}_test | ./$^

test1 test2 test3 test4 test5 test6 test7 test8 test9: test%: test0%
test10 test11 test12 test13 test14 test15 test16 test17 test18 test19: test%: $(YEAR)_%
test20 test21 test22 test23 test24 test25 test26 test27 test28 test29: test%: $(YEAR)_%
test30 test31: test%: $(YEAR)_%

%rid.o: lib/%rid.c include/%rid.h
	$(CC) -c $(_CFLAGS) -Iinclude -o $@ $<
get_next_line.o: lib/get_next_line.c include/get_next_line.h
	$(CC) -c $(_CFLAGS) -Iinclude -o $@ $<

$(YEAR)_%.o: $(YEAR)_%.c
	$(CC) -c $(_CFLAGS) -Iinclude -o $@ $<



clean:
	rm -f *.o
	rm -f $(YEAR)_[0-3][0-9]
	rm -f $(YEAR)_[0-3][0-9]p2

