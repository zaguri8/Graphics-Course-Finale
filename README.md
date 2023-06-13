## Introduction
This project represents the culminating work for Course 20562, Computer Graphics, undertaken at the Open University. The project was diligently completed by Nadav Avnon as a requisite part of the coursework.

 **❗ Authors' Note ❗**\
 The project uses OpenGL 3.3 without fixed pipeline since i wanted to program some of the pipeline stages my self, so ImGui was needed to create the menus, as OpenGL 3.3 does not have a built in way to create menus. In addition, i wanted to use some textures so i included STBI for loading images, no external libraries were used for the rendering itself.


## Libraries used
The libraries used in this project are:
- GLFW -  For window creation and input handling
- GLM - For matrix and vector operations
- OpenGL - For rendering
- STB Image - For loading textures
- ImGUI - For the Menus

The files i created are the following:
- `main.cpp` - The main file of the project, contains the main loop and the rendering code.
- `_graphics.hpp` - A header file for all the graphics related code.
- `_camera.hpp` - A class for creating and using a camera.
- Shader files - The shader files for the project \
(**included in `shaders` folder**).



## How to run
There is just 1 adjustment needed to be made in order to run the project, and that is to change the path for GLFW, GLM and OpenGL in the Makefile, (STBI and ImGUI are already included inside the project).
The project is built using the Makefile, so just run `make` in the terminal and then `./final_project` to run the project.

## Controls
The controls are as follows:
- `W` - Move forward
- `S` - Move backwards
- `A` - Move left
- `D` - Move right
- `R` Rotate the camera to the right
- `Q` Rotate the camera to the left
- `2` Rotate the camera upwards
- `3` Rotate the camera downwards


## Authors
- **Nadav Avnon** - *Project owner* - [Nadav Avnon](https://github.com/zaguri8)    