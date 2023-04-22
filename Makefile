all: math physics images
	g++ *.o main.cpp

math: box.o lemke_solver.o matrix.o v2.o

v2.o: src/math/v2.h src/math/v2.cpp
	g++ -c src/math/v2.cpp

box.o: src/math/box.h src/math/box.cpp v2.o
	g++ -c src/math/box.cpp

lemke_solver.o: src/math/lemke_solver.h src/math/lemke_solver.cpp matrix.o
	g++ -c src/math/lemke_solver.cpp

matrix.o: src/math/matrix.h src/math/matrix.cpp
	g++ -c src/math/matrix.cpp


physics: dynamic_box.o force.o

dynamic_box.o: src/physics/dynamic_box.h src/physics/dynamic_box.cpp box.o
	g++ -c src/physics/dynamic_box.cpp

force.o: src/physics/force.h src/physics/force.cpp v2.o
	g++ -c src/physics/force.cpp


images: adv_image.o my_image.o

adv_image.o: src/images/adv_image.h src/images/adv_image.cpp
	g++ -c src/images/adv_image.cpp

my_image.o: src/images/my_image.h src/images/my_image.cpp
	g++ -c src/images/my_image.cpp


clean:
	rm *.o