all:
	gcc raytrace.c -o raytrace
clean:
	rm -f raytrace raytrace.o
run: ./raytrace
	./raytrace