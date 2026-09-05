define getlib
	export CPLUS_INCLUDE_PATH=$$CPLUS_INCLUDE_PATH:$$(pwd)/.env/include/; \
	echo $$CPLUS_INCLUDE_PATH; \
	cd .env/tmp; \
	git clone git@github.com:Thomas-Baricault/tbaricault_$(1).git; \
	cd tbaricault_$(1); \
	sed -i 's/ENV			=	/ENV			=	..\/..\//g' Makefile; \
	make install;
endef

define getutf8
	cd .env/tmp; \
	wget https://github.com/nemtrif/utfcpp/archive/refs/tags/v4.2.0.zip; \
	unzip v4.2.0.zip; \
	cp -r utfcpp-4.2.0/source/* ../include/; \
	rm -dfr utfcpp-4.2.0; \
	rm -f v4.2.0.zip;
endef

init:
	rm -dfr .env
	mkdir -p .env/include
	mkdir -p .env/lib
	mkdir -p .env/tmp
	$(call getutf8)
	$(call getlib,uniconvert)
	$(call getlib,str)
	$(call getlib,cli)
	rm -dfr .env/tmp

test:
	cd test && php -S 0.0.0.0:8080 index.php

.PHONY: init test
