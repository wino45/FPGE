set VERSION=070

del fpge%VERSION%.zip

copy /y upx.exe release\
copy /y test\mapfrg.txt release\
copy /y test\countries.txt release\
copy /y test\icons.txt release\
copy /y test\0str.bmp release\

rem PzC conversion files examples
copy /y test\pgu2pzcu.txt release\
copy /y test\pgc2pzcc.txt release\
copy /y test\pgtt2pzctt.txt release\

rem PG2 conversion files examples
copy /y test\pg2tt2pgtr.txt release\
copy /y test\pg2tt2pgtt.txt release\
copy /y test\pg2cl2pgcl.txt release\
copy /y test\pg2c2pgc.txt release\

rem Pacific General conversion files examples
copy /y test\tt2tiles.txt release\
copy /y test\paccl2pgcl.txt release\
copy /y test\pacmt2pgmt.txt release\

cd release
upx fpge.exe
cd ..

rem Pack sources
zip src.zip src\*.Manifest src\*.rc src\*.ico src\*.h src\*.c .project .cproject make_release.bat CMakeLists.txt

rem Make zip
zip fpge%VERSION%.zip release\FPGE.exe release\*.txt doc\*.* src.zip release\0str.bmp

del src.zip