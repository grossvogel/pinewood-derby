SASSC = sass --scss
SASSHOME = scss/
CSSHOME = assets/css/
ARDUINO_HOME = ../arduino/derby_timer
ARDUINO_SOURCE = derby_timer.ino

watch:
	$(SASSC) --sourcemap=none --quiet --watch $(SASSHOME):$(CSSHOME) > /dev/null 2>&1 &

build:
	$(SASSC) --sourcemap=none --trace --stop-on-error --update $(SASSHOME):$(CSSHOME)
	cp $(ARDUINO_SOURCE) $(ARDUINO_HOME)

