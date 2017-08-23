all: graceful-terminating-gui-application-windows.zip

graceful-terminating-gui-application-windows.exe: main.c
	gcc -o $@ -std=gnu99 -pedantic -Os -Wall -m64 -municode -mwindows main.c
	strip $@

graceful-terminating-gui-application-windows.zip: graceful-terminating-gui-application-windows.exe
	zip -9 $@ $<

clean:
	rm -f graceful-terminating-gui-application-windows.*

.PHONY: all clean
