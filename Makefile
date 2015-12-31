# -------------------------------------------------------------------------
# Sample makefile for GNU/Linux system
# To compile and test interp module
# -------------------------------------------------------------------------
CC = avr-gcc

bl: 
	@make analogIn.o
	@make servoControl.o
	@make servoControl
	@make load

analogIn.o: analogIn.c analogIn.h
	$(CC) -std=c99 -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o analogIn.o analogIn.c

servoControl.o: servoControl.c
	$(CC) -std=c99 -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o servoControl.o servoControl.c

servoControl:
	$(CC) -std=c99 -mmcu=atmega328p servoControl.o analogIn.o -o servoControl

load:
	avr-objcopy -O ihex -R .eeprom servoControl servoControl.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:servoControl.hex

clean:
	rm -f *.o
	rm -f *.hex
	rm servoControl
