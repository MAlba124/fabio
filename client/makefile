##
#
#
# @file
# @version 0.1
openGlStart: poker.cpp glad.o
	g++ -o poker poker.cpp glad.o imgui/*.o -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

glad.o: include/glad.c
	g++ -c include/glad.c
# end
