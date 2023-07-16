code_file = sutherland-hodgman.c
functions_file_clip = clip.c
functions_file_fill = polygonfilling.c
FLAGS = -lGL -lGLU -lglut 

run: compile

compile: $(code_file)
	gcc $(code_file) $(functions_file_clip) $(functions_file_fill) $(FLAGS) -o sh
