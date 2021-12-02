all:
	gcc ./ORP/aspl.c -c -O2
	gcc ./ORP/utils.c -c -O2
	g++ main.cpp -c -O2
	g++ aspl.o utils.o main.o -o ORP.exe -O2

clean:
	rm -f *.o *.a *.exe