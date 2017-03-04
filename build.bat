set CUR_CD=%CD:\=/%
set GDK=%CUR_CD%
set GDK_WIN=%CUR_CD%
set PATH=%GDK_WIN%\bin;%PATH%
make -f makelib.gen
cd demo
make -f ..\makefile.gen
del temp*
cd ..
