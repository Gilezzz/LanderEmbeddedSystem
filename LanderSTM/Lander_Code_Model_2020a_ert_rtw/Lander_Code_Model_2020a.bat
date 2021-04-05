cd .

if "%1"=="" ("C:\PROGRA~1\MATLAB~1\bin\win64\gmake"  -f Lander_Code_Model_2020a.mk all) else ("C:\PROGRA~1\MATLAB~1\bin\win64\gmake"  -f Lander_Code_Model_2020a.mk %1)
@if errorlevel 1 goto error_exit

exit 0

:error_exit
echo The make command returned an error of %errorlevel%
exit 1
