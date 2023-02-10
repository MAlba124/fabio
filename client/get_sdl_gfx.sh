#!/bin/sh

if [ -e SDL2_gfx ]
then
	echo "SDL2_gfx already installed"
else
	wget http://www.ferzkopp.net/Software/SDL2_gfx/SDL2_gfx-1.0.4.tar.gz -O \
		SDL2_gfx.tar.gz
	tar -xvf SDL2_gfx.tar.gz
	rm SDL2_gfx.tar.gz
	mv SDL2_gfx-1.0.4 SDL2_gfx
fi
