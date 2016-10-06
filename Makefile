.PHONY: all build clean run zip

ZIPNAME = 08_xdobes13_xborek08.zip

CPPC = g++
CPPF = -O2 -std=c++11 -Wno-write-strings

SRCS = *.cpp
HDRS = *.h

all: $(SRCS) $(HDRS)
	$(CPPC) $(CPPF) -o model $(SRCS) -lsimlib -lm 

build: all
	
run:
	echo "Spoustim simulaci 1 tydne. Vysledek bude ulozen v naprahu.csv"
	./model -s naprahu.hdf -t 604800 -o naprahu.csv
	echo "Simulace dokoncena. Vysledek ulozen v souboru naprahu.csv"

clean:
	rm model || true

zip:
	rm $(ZIPNAME) || true
	zip -9 $(ZIPNAME) Makefile $(SRCS) $(HDRS) *.hdf dokumentace.pdf
