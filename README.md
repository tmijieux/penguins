# penguins
Les pingouins ou la vie.

play the "hey that's my fish" game on a 1D, 2D or 3D platform (in a 3D view) versus AI or other players
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


# touche controle:

FLECHE GAUCHE: retour arrière
FLECHE DROITE: avancer

FLECHE HAUT: deplacer la case sélectionée pour afficher les aretes du graphes entre les cases
FLECHE BAS: deplacer la case sélectionée pour afficher les aretes du graphes entre les cases

P: Activer/Désactiver "l'autoplay"
S: abandonner (s pour surrender)
R: réinitialiser la caméra
5 (pavé numérique): switcher entre project orthogonale et perspective

# screenshot

![penguins 3d penrose](/assets/penguins3dpenrose.png)


# Requirements / Dependencies

- OpenGL
- GLU
- GLFW3
- FreeType

also relying on the following software (bundled, no install required)  
- glad
- stbi\_image (https://github.com/nothings/stb/blob/master/stb\_image.h)


# How to build
if all requirements are available just `make` should be enough
on unix

otherwise use cmake in a build directory to generate the build system:
mkdir build
cd build
cmake ..

The windows platform is supported using cmake and Visual Studio
