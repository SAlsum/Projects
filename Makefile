src = $(wildcard *.cc) $(wildcard */*.cc)
obj = $(src:.cc=.o)

INCS = $(shell root-config --cflags)
LIBS = $(shell root-config --glibs)

CPPFLAGS = $(INCS) $(LIBS)

ApplyCuts.exe: $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(obj) ApplyCuts