FROM	fritzprix/cbuild:0.0.2

MAINTAINER	fritzprix

RUN		apt-get update
RUN		git clone https://github.com/fritzprix/cdsl.git
WORKDIR	cdsl
RUN		make config DEFCONF=x86_x86_64_hosting
RUN		make release
RUN		make all
RUN		make test 
CMD		./cdsl

