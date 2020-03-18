src = $(wildcard cuts/*.cc) $(wildcard utility/*.cc)
head = $(wildcard cuts.h) $(wildcard utility/*.h)
obj = $(src:.cc=.o)
executables = $(wildcard *.exe)

INCS = $(shell root-config --cflags)
LIBS = $(shell root-config --glibs)
MATS = -I${MATLABPATH}

CPPFLAGS = $(INCS) $(LIBS)


.PHONY: all


ApplyCuts.exe: ApplyCuts.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

BaseCuts.exe: BaseCuts.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

BaseCutsAdjacent.exe: BaseCutsAdjacent.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

BaseCuts_noKr.exe: BaseCuts_noKr.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

BaseCutsExtended.exe: BaseCutsExtended.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

MedCuts.exe: MedCuts.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

MaxPeakAreaStudy.exe: MaxPeakAreaStudy.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

MaxPeakAreaStudy_Run03.exe: MaxPeakAreaStudy_Run03.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

MaxPeakAreaStudy_R3R4.exe: MaxPeakAreaStudy_R3R4.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

MaxPeakAreaStudy_Revamped.exe: MaxPeakAreaStudy_Revamped.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

PromptFractionStudy.exe: PromptFractionStudy.o $(obj) $(head)
	g++ -o $@ PromptFractionStudy.o $(obj) $(INCS) $(LIBS)

PromptFractionStudy_Run03.exe: PromptFractionStudy_Run03.o $(obj) $(head)
	g++ -o $@ PromptFractionStudy_Run03.o $(obj) $(INCS) $(LIBS)

PromptFractionStudy_R3R4_Compare.exe: PromptFractionStudy_R3R4_Compare.o $(obj) $(head)
	g++ -o $@ $^ $(INCS) $(LIBS)

S2ShapeStudy.exe: S2ShapeStudy.o $(obj) $(head)
	g++ -o $@ S2ShapeStudy.o $(obj) $(INCS) $(LIBS)

S2WidthStudy.exe: S2WidthStudy.o $(obj) $(head)
	g++ -o $@ S2WidthStudy.o $(obj) $(INCS) $(LIBS)

BadAreaStudy.exe: BadAreaStudy.o $(obj) $(head)
	g++ -o $@ BadAreaStudy.o $(obj) $(INCS) $(LIBS)

Chi2Study.exe: Chi2Study.o $(obj) $(head)
	g++ -o $@ Chi2Study.o $(obj) $(INCS) $(LIBS)

CalculateEfficiency.exe: CalculateEfficiency.cc
	g++ -o $@ $^ $(INCS) $(LIBS)

EventSelector.exe: EventSelector.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

ApplySelection.exe: ApplySelection.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

LuxstampCut.exe: LuxstampCut.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

ElseCut.exe: ElseCut.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

KrCut.exe: KrCut.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

PlotFinals.exe: PlotFinals.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

FiducialCut.exe: FiducialCut.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

BinTest.exe: BinTest.cc utility/HistInfo.o
	g++ -o $@ $^

MPACut.exe: MPACut.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

PFCut.exe: PFCut.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

MPADD.exe: MPADD.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

SelectDD.exe: SelectDD.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

PFDD.exe: PFDD.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

BaseCuts_noThresholds.exe: BaseCuts_noThresholds.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

KillRepeats.exe: KillRepeats.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

PlotPositions.exe: PlotPositions.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

SelectSpecific.exe: SelectSpecific.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)

PrintS1Centers.exe: PrintS1Centers.cc $(obj)
	g++ -o $@ $^ $(INCS) $(LIBS)


.PHONY: clean
clean:
	rm -f $(obj) $(executables) ApplyCuts.o BaseCuts.exe BaseCuts.o MaxPeakAreaStudy.exe MaxPeakAreaStudy.o PromptFractionStudy.o S2ShapeStudy.o
