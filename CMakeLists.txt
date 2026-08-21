cmake_minimum_required(VERSION 3.24)
project(frag)

add_executable(frag
	frag.c
	laymake.h laymake.c
	drvinfo.h drvinfo.c
	dosutil.h dosutil.c
	structs.h
	util.h
)
