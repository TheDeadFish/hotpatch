project(hotpatch)
cmake_minimum_required(VERSION 3.12)

add_library( hotpatch STATIC hotpatch.cc 
	mempatch.cc xheap.cc apihook.cc callpatch.cc)
