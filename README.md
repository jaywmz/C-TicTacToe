## Note

Our application was programmed, built, and tested on Windows. The following instructions are based on the user being on a Windows platform.

## Requirements

1. **RayLib (Graphics library for C)**
	- Our application uses RayLib for its GUI portion. [RayLib GitHub](https://github.com/raysan5/raylib)
	- To install on Windows:
	  - Open [MSYS2](https://www.msys2.org/)
	  - Run: `pacman -S mingw-w64-x86_64-raylib`
	  - Alternatively, you can see [Working on Windows](https://github.com/raysan5/raylib/wiki/Working-on-Windows)
	- To install on Mac: [Working on macOS](https://github.com/raysan5/raylib/wiki/Working-on-macOS)

2. **A working C compiler (e.g., gcc for Windows)**
	- You can install gcc using [MSYS2](https://www.msys2.org/)
	- Run: `pacman -S mingw-w64-ucrt-x86_64-gcc`

## Building and Running the Application

1. Open a terminal.
2. Build the app using:
	```sh
	gcc -fcommon -o tictactoe tictactoeMain.c tictactoeGUI.c tictactoeLogic.c -lraylib -lopengl32 -lgdi32 -lwinmm
	```
3. Run the generated exe file using:
	```sh
	./tictactoe
	```
