:: build library
@call cmake_gcc
@libmerge %PROGRAMS%\local\lib32\libexshit.a build\libhotpatch.a
@call cmake_gcc x64
@libmerge %PROGRAMS%\local\lib64\libexshit.a build64\libhotpatch.a

:: copy include files
copy /Y hotpatch.h %PROGRAMS%\local\include
copy /Y apihook.h %PROGRAMS%\local\include
copy /Y callpatch.h %PROGRAMS%\local\include
