    
# Harmony Engine

#### Cross-platform game engine and editor for making expansive 2D or 2.5D games.

### [Feature Roadmap](https://github.com/users/GameOverture/projects/2)
##### **Disclaimer:** This is a very much work in progress and not ready for public consumption.
---
The Harmony Engine consists of two main projects
- The GUI editor (HyEditor) that manages and handles all the offline work
- The engine itself (HyEngine) which is a static library that links into your application.

## Technologies Utilized

**Engine:** OpenGL, SDL2 (or GLFW), Box2d, glad, glm, stb, soil2, rapidjson

**Editor:** Qt, freetype2, freetype-gl, libogg/vorbis, QtWaitingSpinner

**Build:** CMake, Emscripten

## Editor Features (HyEditor)
* Generate and manage game projects and code builds
    - Manages all the CMake shenanigans under the hood
    - Use any major IDE
    - All library dependencies taken care of, including any specified ones you use
* A full asset pipeline from import -> optimize -> engine
    - Handle various types of assets such as tilesheets, PNG sequences, GLTF, and audio
    - Assets are packed into optimized banks with optional compression techniques
    - Quickly replace and iterate on existing assets
* Visually create every element in the game
    - Manage sprite animations with various states
    - Generate truetype fonts with optional effects such as outlines, inlines, and color blending
    - Audio can be logically grouped into cues with specified weighted randomness
    - Particle effects (TBD)
    - Visualize in-engine Spine® skeletons imports and specify runtime animation blending and mixing
* Create complex Entities using the elements above
    - Apply physics, add colliders, and setup bounding volumes

![Harmony Editor](https://private-user-images.githubusercontent.com/344924/324213397-200843ab-a37d-4cb2-8800-5bae9ce0aa4d.gif?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MTM4MTgwMTQsIm5iZiI6MTcxMzgxNzcxNCwicGF0aCI6Ii8zNDQ5MjQvMzI0MjEzMzk3LTIwMDg0M2FiLWEzN2QtNGNiMi04ODAwLTViYWU5Y2UwYWE0ZC5naWY_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjQwNDIyJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI0MDQyMlQyMDI4MzRaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1kNmRhOTMwMGVjODQ4OTViNmU3NWE4ZTg2YWM5ZGM0MWMwMGI5ZTYzYTg0MDBkNDNkMzlmMWVkY2ViODhjZDkyJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZhY3Rvcl9pZD0wJmtleV9pZD0wJnJlcG9faWQ9MCJ9.-JSPDu9zdEpueN-WugIaKqq794J6HPYaQjVRCNM-ASM)

## Engine Features (HyEngine)

- Compile and run the same game code in Windows, Mac, Linux environments. As well as target web and mobile environments by transpiling to wasm via Emscripten.
- Robust and simple programming API, learn once, apply to every type of game object
- First-class support for Spine Animation
- TBD

## Installation 

1. Clone this repository
2. Download and execute the [CMake Installer](https://cmake.org/download/). Make sure to set the PATH variable during installation
3. Run a pre-built Harmony Editor or compile it from source

### Compiling the Editor
- Compiling the editor **requires Qt - Only version 5.15.0 has been tested**, open-source version
- When installing Qt6, under the selected Qt version, you must also select "Additional Libraries" -> Qt Multimedia
- Using CMake, choose one of the two methods below:
```
-- GUI TOOL ---------------------------------------------------------------------------------------------
Open CMake-gui tool

"Where is the source code:" should be:    <path to repo>
"Where to build the binaries:" should be: <path to repo>\build

Press "Configure" button and confirm creating the build directory
In the next dialog, choose which IDE to generate in first combo box

Fill in the value for variable Qt5_DIR. Specify cmake Qt5 directory:
-Example-  C:/Qt/5.15.0/msvc2017_64/lib/cmake/Qt5
  
Press "Configure" again until all red options disappear
Press "Generate" once
Press "Open Project" to open the generated solution (.sln) or navigate to the specified build directory

-- COMMAND LINE ----------------------------------------------------------------------------------------
  
cmake -S "<path to repo>" -B "<path to repo>\build" -DQt5_DIR=<path cmake Qt5> -G "Visual Studio 16 2019"

Fill in the value for variable Qt5_DIR. Specify cmake Qt5 directory:
-Example-  C:/Qt/5.13.0/msvc2017_64/lib/cmake/Qt5

This will use default settings to generate a build. If on Windows, CMake will detect which version of 
Visual Studio you have installed to generate if you omit the -G argument. If Visual Studio 2019 is chosen 
it will default to generating 64bit, while older versions of Visual Studio will generate 32bit. You can
explicitly set the archetecture yourself by including the -G and -A arguments. Examples below:
... -G "Visual Studio 16 2019" -A Win32
... -G "Visual Studio 15 2017" -A x64
... -G "Visual Studio 14 2015" -A x64

The solution (.sln) will be placed in the build directory (-B argument above)
```
  
## Usage/Examples

```C++
HySprite2d hero("Level1", "Hero");
hero.Load();
hero.pos.Set(0.0f, 0.0f);

// Move right and grow larger
hero.pos.Tween(100.0f, 0.0f, 2.5f);
hero.scale.Tween(1.5f, 1.5f, 2.5f);

// TODO: actual useful examples
```
  
## Documentation

lol

  
## License

**Engine:** [Modified zlib](https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE/)

**Editor:** [GPL/LGPL](https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE/)
