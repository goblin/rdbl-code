all: rdbl_enc rdbl_dec

rdbl_enc: huffs.o encode.cc
	g++ -std=c++11 -o rdbl_enc encode.cc huffs.o

rdbl_dec: huffs.o decode.cc
	g++ -std=c++11 -o rdbl_dec decode.cc huffs.o

install:
	install -d $(DESTDIR)/usr/bin
	install rdbl_enc $(DESTDIR)/usr/bin/
	install rdbl_dec $(DESTDIR)/usr/bin/

huffs.o: huffs.cc huffs.hh dict.hh
	g++ -std=c++11 -Os -c -o huffs.o huffs.cc

# don't want to clean dict.hh
clean:
	rm -f rdbl_enc rdbl_dec huffs.o

dict.hh: words_alpha.txt process_dict.pl
	./process_dict.pl < words_alpha.txt > dict.hh

