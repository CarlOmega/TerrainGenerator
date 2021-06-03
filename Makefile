all: main.o ./common/shader.o ./common/controls.o
	g++ -lGlfw -lGLEW -framework OpenGL main.o ./common/shader.o ./common/controls.o -g -o ./terrain.out
main: main.cpp
	g++ -c main.cpp -g
shader: ./common/shader.cpp
	g++ -c shader.cpp -g
controls: ./common/controls.cpp
	g++ -c controls.cpp -g