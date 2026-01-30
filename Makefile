all: mkdir_out mongoose
	mkdir -p ./out
	cc main.c out/mongoose.o -o out/r4msg -I./3rd_party/mongoose

mkdir_out:
	mkdir -p out

mongoose:
	cc -c 3rd_party/mongoose/mongoose.c -o out/mongoose.o -DMG_TLS=MG_TLS_BUILTIN

#.PHONY: all mongoose
