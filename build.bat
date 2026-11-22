@echo off
REM ---------------------------------------------------------------
REM  Windows build script (needs g++ from MinGW-w64 or MSYS2 on PATH)
REM ---------------------------------------------------------------
set SRC=src\Types.cpp src\Piece.cpp src\PieceFactory.cpp src\King.cpp src\Queen.cpp src\Rook.cpp src\Bishop.cpp src\Knight.cpp src\Pawn.cpp src\Board.cpp src\BoardRules.cpp src\Game.cpp src\Demo.cpp

echo Building chess.exe ...
g++ -std=c++17 -O2 -Wall -Iinclude %SRC% src\main.cpp -o chess.exe
if errorlevel 1 goto fail

echo Building test_chess.exe ...
g++ -std=c++17 -O2 -Wall -Iinclude %SRC% tests\test_chess.cpp -o test_chess.exe
if errorlevel 1 goto fail

echo.
echo Done!  Play with:  chess.exe     Tests:  test_chess.exe     Tour:  chess.exe --demo
goto end
:fail
echo Build failed.
:end
