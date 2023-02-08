#!/bin/sh

if [ -e imgui_docking ]
then
	echo "ImGUI already installed"
else
	wget https://github.com/ocornut/imgui/archive/refs/heads/docking.zip -O \
		imgui_docking.zip
	unzip imgui_docking.zip
	rm imgui_docking.zip
fi
