
VGFONTPATH=/opt/vc/src/hello_pi/libs/vgfont
HELLOFONTPATH=/opt/vc/src/hello_pi/hello_font/

CPPFLAGS+= -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux -I$(VGFONTPATH)
LDFLAGS+= -L$(VGFONTPATH) -lvgfont -lfreetype -lz
LDFLAGS+=-L/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm

all: vgfont frsky_omx_osd

vgfont:
	cp $(HELLOFONTPATH)/Vera.ttf .
	make -C $(VGFONTPATH)



%.o: %.c
	gcc -c -o $@ $< $(CPPFLAGS)

frsky_omx_osd: main.o frsky.o render.o
	gcc -o $@ $^ $(LDFLAGS)


clean:
	rm -f frsky_omx_osd *.o *~


