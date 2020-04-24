set SRC=hotpatch.cc mempatch.cc xheap.cc apihook.cc callpatch.cc
set OBJ=hotpatch.o mempatch.o xheap.o apihook.o callpatch.o

:: build
call :build 32
call :build 64
del *.o

:: copy include files
copy /Y hotpatch.h %PROGRAMS%\local\include
copy /Y apihook.h %PROGRAMS%\local\include
copy /Y callpatch.h %PROGRAMS%\local\include
exit /b

:build
	setlocal
	call egccx.bat x%1
	gcc %SRC% %CCFLAGS2% -c
	ar rcs %PROGRAMS%\local\lib%1\libexshit.a %OBJ%
	endlocal
	exit /b
