FROM	fritzprix/cbuild:0.0.1

MAINTAINER	fritzprix

RUN		apt-get update
RUN		git clone https://github.com/fritzprix/cdsl.git
WORKDIR	cdsl
RUN		make config ARCH=x86 DEFCONF=x86_64
RUN		make release
RUN		make all
CMD		make test & ./cdsl

