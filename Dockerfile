FROM	fritzprix/cbuild:0.0.2

MAINTAINER	fritzprix

RUN		apt-get update
RUN		git clone https://github.com/fritzprix/cdsl.git
WORKDIR	cdsl
RUN		make config ARCH=x86 DEFCONF=x86_64
RUN		make release
RUN		make all
RUN		make test 
CMD		./cdsl

