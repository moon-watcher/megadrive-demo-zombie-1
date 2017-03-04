set CUR_CD=%CD:\=/%
set GDK=%CUR_CD%
set GDK_WIN=%CUR_CD%
set PATH=%GDK_WIN%\bin;%PATH%
cd src
del /q *.o
cd ..
make -f makelib.gen
cd demo
del /s/q out
make -f ..\makefile.gen
del temp*
cd ..
