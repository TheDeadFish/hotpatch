setlocal
call egcc64.bat 
gcc hotpatch.cc %CCFLAGS2% -m32 -c 
ar rcs %PROGRAMS%\local\lib32\libexshit.a hotpatch.o
gcc hotpatch.cc %CCFLAGS2% -c
ar rcs %PROGRAMS%\local\lib64\libexshit.a hotpatch.o
del hotpatch.o