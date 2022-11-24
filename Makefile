mandelplot: mandelplot.cpp
	g++ -std=c++17 -c mandelplot.cpp
	g++ -std=c++17 mandelplot.o -o mandelplot -lsfml-graphics -lsfml-window -lsfml-system
	rm mandelplot.o
