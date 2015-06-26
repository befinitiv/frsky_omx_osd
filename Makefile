all: frsky_omx_osd



frsky_omx_osd: main.c
	gcc -Wall main.c -o frsky_omx_osd


clean:
	rm -f frsky_omx_osd *~


