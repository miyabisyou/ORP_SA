ifeq ($(ENV), intel)
  CC=icc
  CXX=icpc
else ifeq ($(ENV), fujitsu)
  CC=fccpx
  CXX=FCCpx
else
  CC=gcc
  CXX=g++
endif

all:
	$(CC) ./ORP/aspl.c -c -O2
	$(CC) ./ORP/utils.c -c -O2
	$(CXX) main.cpp -c -O2
	$(CXX) aspl.o utils.o main.o -o ORP.exe -O2

clean:
	rm -f *.o *.a *.exe
	rm ./../CSV -f -r
	rm ./../DoNS -f -r
	rm ./../graph -f -r
	rm ./../log -f -r
	rm ./../result -f -r
	rm ./../Accept -f -r
	rm ./../SLME -f -r
