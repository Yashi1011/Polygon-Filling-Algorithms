code_file = scanlinepolyfill.c
functions_file = polygonfilling.c
input_file = crab.txt

run: compile execute delete

compile: $(code_file)
	gcc $(code_file) $(functions_file) -lGL -lGLU -lglut -o polyfill

execute: compile
	./polyfill $(input_file)

delete: execute
	rm polyfill
