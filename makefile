all: build
	./Executables/Differentiator

src_list = ./src/differentiator_funcs.cpp ./src/RD_funcs.cpp ./src/Differentiator.cpp ./src/debug_funcs.cpp ./src/root_funcs.cpp

build:
	g++ -D _DEBUG=1 -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations								\
	-Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body									                    \
	-Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor						                \
	-Wopenmp -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion				    \
	-Wstrict-overflow=2	-Wconversion -Werror																			\
	-Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused										    \
	-Wvariadic-macros  -Wno-reserved-user-defined-literal -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast	    \
	-Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow 						    \
	-fno-omit-frame-pointer -Wlarger-than=8192 -Wframe-larger-than=8192 -fPIE													    \
	-fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,				            \
	-I include $(src_list) -o ./Executables/Differentiator -fsanitize=address
launch:
	./Executables/Differentiator
dot:
	dot ./dots/dotfile.dot -Tpng -O ./Pics/
