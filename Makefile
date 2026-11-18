NAME		?=	f2p2
VERSION		?=	1.0.0
ENV			?=	
ARGS		?=	


PACKAGES	=	tbaricault_cli	\

TARGETS		=	tbaricault::cli	\

SRC			=	main	\
				Project	\


all: build

configure:
	@cmake							\
		-B build 					\
		-DNAME="$(NAME)"			\
		-DVERSION="$(VERSION)"		\
		-DENV="$(ENV)"				\
		-DPACKAGES="$(PACKAGES)"	\
		-DTARGETS="$(TARGETS)"		\
		-DSRC="$(SRC)"				\

build: configure
	@cmake --build build

clean:
	@rm -dfr build

re: clean build

run:
	"build/$(NAME)" $(ARGS)


.PHONY: all configure build clean re run
