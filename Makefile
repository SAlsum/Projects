src = $(wildcard */*.cc)
head = $(wildcard *.h) $(wildcard */*.h)
obj = $(src:.cc=.o)

INCS = $(shell root-config --cflags)
LIBS = $(shell root-config --glibs)

CPPFLAGS = $(INCS) $(LIBS)

ApplyCuts.exe: ApplyCuts.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

BaseCuts.exe: BaseCuts.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(obj) ApplyCuts.exe ApplyCuts.o BaseCuts.exe BaseCuts.o