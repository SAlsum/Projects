src = $(wildcard *.cc) $(wildcard */*.cc)
head = $(wildcard *.h) $(wildcard */*.h)
obj = $(src:.cc=.o)

INCS = $(shell root-config --cflags)
LIBS = $(shell root-config --glibs)

CPPFLAGS = $(INCS) $(LIBS)

ApplyCuts.exe: $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(obj) ApplyCuts