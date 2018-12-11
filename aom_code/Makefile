
ARDUINOPATH = /Applications/Arduino.app/Contents/Java/

ARDUINOBUILDER = $(ARDUINOPATH)/arduino-builder

TOOLS = $(ARDUINOPATH)/hardware/tools/


SKETCH = $(wildcard *.ino)

GITVERS = $(shell git describe --match NeVeRmATcH --always --abbrev=7 --dirty)
#CFLAGS =

#src/gitversion.h: .git/head .git/index
#	echo "const char * gitversion = \"$(shell git describe --match=NeVeRmAtCh --always --abbrev=7 --dirty)\";" > $@

upload: compile
	$(TOOLS)/teensy_post_compile -file=$(SKETCH) -path=$(CURDIR)/build -tools=$(TOOLS) -board=TEENSY31 -reboot

compile:
	mkdir -p build
	mkdir -p bin

	# this is bad: need to modify platform.txt for teensy to include build.flags.extras in the g++ compile command.
	$(ARDUINOBUILDER) -compile -verbose  -prefs="build.flags.extras=-DGITVERSION=\"$(GITVERS)\"" -build-options-file build.options.json -build-path $(CURDIR)/build $(SKETCH)
	cp $(CURDIR)/build/$(SKETCH).hex $(CURDIR)/bin/aom.ino.$(GITVERS).hex
	cp $(CURDIR)/build/$(SKETCH).elf $(CURDIR)/bin/aom.ino.$(GITVERS).elf


clean:
	rm -rf build/
	rm -rf bin/
