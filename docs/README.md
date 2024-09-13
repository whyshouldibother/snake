# Docs
Documentation for various things

## Installing Raylib on Windows

### Recommended Method
- Install Latest Version of [MinGw](https://github.com/skeeto/w64devkit/releases/tag/v2.0.0)
Make sure to add the path/bin to path
- Download The latest MinGw release of [Raylib library](https://github.com/raysan5/raylib/releases/tag/5.0) preferably [raylib-5.0_win64_mingw-64.zip](https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip)

- Extract Raylib and copy the files under include and lib to respective folders under MinGw_path/bin

### For using other IDE
- Download the latest release of [Raylib library](https://github.com/raysan5/raylib/releases/tag/5.0) corresponding to your IDE.
- Extract Raylib and copy them to your project folder
- Modify your build command to be `g++ main.cpp -Iinclude -Llib -lraylib -lwinmm -lopengl32 -lgdi32`

If the following methods doesnt work inquire the [official site](https://github.com/raysan5/raylib/wiki/Working-on-Windows)