
proj07: proj07.student.o
	g++ -o proj07 proj07.student.o
proj07.student.o: proj07.student.c
	g++ -std=c++11 -c -Wall proj07.student.c


