# Pingouins !
(a board game about penguins)

Play the "hey that's my fish" game on a 1D, 2D or 3D platform (in a 3D view)
versus AI or other players
(it can also be ran with n dimension , n > 3, but without a 3D view)

you can also:
 - program your own AI to play on the platform
 - program a module that generates new map type

currently available map types are:
  - square
  - hexagonal
  - triangle
  - octagonal+little square
  - penrose


# Control keys:

- LEFT ARROW: go back in game history
- RIGHT ARROW: go forward in game history
- UP ARROW: in a given debug mode, red threads are displayed for all tiles reachable from a given selected tiles. The key binding change the selected tile.
- DOWN ARROW: Same as UP ARROW but in the other direction.
- P: Toggle "autoplay" (animations are playing on 3D board automatically when AI are playing their moves without the user having to pres RIGHT ARROW)
- S: [S]urrend (Human player leave the game and let the AIs finish the game on their own)
- R: Reset camera to initial position
- 5 (numeric keypad): Toggle between perspective and orthographic projection.
- LEFT MOUSE BUTTON: translate the 3D view
- RIGHT MOUSE BUTTON: rotate the 3D view
- MOUSE WHEEL: Zoom in or Zoom out the 3D view


# screenshot

![penguins 3d penrose](/assets/penguins3dpenrose.png)


# Requirements / Dependencies

- OpenGL with GLU (basically you need to have a OpenGL implementation installed, either mesa or your graphic card's driver)
- GLFW3
- FreeType

also relying on the following software (bundled, no install required)
- glad
- stbi\_image (https://github.com/nothings/stb/blob/master/stb\_image.h)


# How to build
if all requirements are available just `make` should be enough
on unix

otherwise use cmake in a build directory to generate the build system:
- mkdir build
- cd build
- cmake ..

The windows platform is supported using cmake and Visual Studio
