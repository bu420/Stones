main:
	g++ -std=c++20 -O3 -Wno-narrowing -Iinclude -Iinclude/bullet -Isrc -Llib -Llib/bullet -o bin/main src/*.cpp obj/flecs.o obj/glad.o \
	-lmingw32 -lglfw3 -lgdi32 \
	-lBulletDynamics -lBulletCollision -lBulletSoftBody -lLinearMath \
	-lwsock32 -lws2_32

libs:
	gcc -c -std=c99 -w -Iinclude -o obj/flecs.o src/flecs.c
	gcc -c -std=c99 -Iinclude -o obj/glad.o src/glad.c