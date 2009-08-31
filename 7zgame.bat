cd axengine\data
7z a -tzip ..\..\axgame\data\data.pak *
cd ..\..

cd axgame\extract
7z a -tzip ..\..\axgame\data\extract.pak *
cd ..\..

cd axgame\main
7z a -tzip ..\..\axgame\data\main.pak *
cd ..\..

mkdir axgame\bin
xcopy /E /S axengine\bin axgame\bin
7z a -t7z axgame.7z axgame\bin axgame\data
