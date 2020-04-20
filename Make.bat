setlocal
call egcc64.bat 
set SRC=hotpatch.cc mempatch.cc xheap.cc apihook.cc
set OBJ=hotpatch.o mempatch.o xheap.o apihook.o

gcc %SRC% %CCFLAGS2% -m32 -c 
ar rcs %PROGRAMS%\local\lib32\libexshit.a %OBJ%
gcc %SRC% %CCFLAGS2% -c
ar rcs %PROGRAMS%\local\lib64\libexshit.a %OBJ%
del *.o