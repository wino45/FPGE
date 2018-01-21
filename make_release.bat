set VERSION=075
set BUILD_DIR=release48

del fpge%VERSION%.zip

copy /y upx.exe %BUILD_DIR%
copy /y test\mapfrg.txt %BUILD_DIR%
copy /y test\countries.txt %BUILD_DIR%
copy /y test\icons.txt %BUILD_DIR%
copy /y test\tiles.txt %BUILD_DIR%
copy /y test\0str.bmp %BUILD_DIR%
copy /y test\bmp2ctry.txt %BUILD_DIR%

rem PzC conversion files examples
copy /y test\pgu2pzcu.txt %BUILD_DIR%
copy /y test\pgc2pzcc.txt %BUILD_DIR%
copy /y test\pgtt2pzctt.txt %BUILD_DIR%

rem PG2 conversion files examples
copy /y test\pg2tt2pgtr.txt %BUILD_DIR%
copy /y test\pg2tt2pgtt.txt %BUILD_DIR%
copy /y test\pg2cl2pgcl.txt %BUILD_DIR%
copy /y test\pg2c2pgc.txt %BUILD_DIR%

rem Pacific General conversion files examples
copy /y test\tt2tiles.txt %BUILD_DIR%
copy /y test\paccl2pgcl.txt %BUILD_DIR%
copy /y test\pacmt2pgmt.txt %BUILD_DIR%

cd %BUILD_DIR%
upx fpge.exe
cd ..

rem Pack sources
zip src.zip src\*.Manifest src\*.rc src\*.ico src\*.h src\*.c .project .cproject make_release.bat CMakeLists.txt

rem Make zip
zip fpge%VERSION%.zip %BUILD_DIR%\FPGE.exe %BUILD_DIR%\*.txt doc\*.* src.zip %BUILD_DIR%\0str.bmp

del src.zip