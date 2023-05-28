# Strong gravitational lensing model

It is a model of strong gravitational lensing in the point source and point lens approximations. 
Each point is considered as a separate source refracted by massive object. For rendering was used SFML graphic library.

- [Video presentation of the project](https://youtu.be/nmlw9BvqGDM)
### Required
- C++ compiler (was tested on C++17 standart)
- [SFML](https://www.sfml-dev.org/) (SFML 2.5.1 stable version was used)
- [gsl](https://www.gnu.org/software/gsl/doc/html/index.html) ([gsl_integration.h](https://www.gnu.org/software/gsl/doc/html/integration.html))

There are some examples of result model

<img src="./output_images/BubbleNebula.jpeg" alt="Lensing of Bubble Nebula" width="50%" height="" title="Lensing of Bubble Nebula">

<img src="./output_images/CaliforniaNebula.jpeg" alt="Lensing of California Nebula" width="50%" height="" title="Lensing of California Nebula">

<img src="./output_images/OrionNebula.jpeg" alt="Lensing of Orion Nebula" width="50%" height="" title="Lensing of Orion Nebula">

<img src="./output_images/RunningManNebula.jpeg" alt="Lensing of Running Man Nebula" width="50%" height="" title="Lensing of Running Man Nebula">

And some tips about using. To compile and run code 
 - on macOS execute `complile.sh` file in this directory or use following expression 
    
        g++ -std=c++17 main.cpp -I/opt/local/include/ /opt/local/lib/libsfml-graphics.dylib /opt/local/lib/libsfml-audio.dylib  /opt/local/lib/libsfml-window.dylib /opt/local/lib/libsfml-system.dylib -Ofast -lgsl -lcblas -o main

        ./main

- on Linux use 
 
        g++ main.cpp -Ofast -lgsl -lblas -lsfml-system -lsfml-audio -lsfml-graphics -lsfml-window -o main

        ./main 
### Control

| Command |              Action            |
|---------|--------------------------------|
|LMB Click| Set the lens position          |
|LMB Drag | Lens is following the mouse    |
|Arrows   | Move the lens in this direction|
|WASD keys| Move the source                |
|Plus     | Increase the lens mass         |
|Backspace| Decrease the lens mass         |
|Enter    | Make screenshot                |
|Space    | Switch magnification show mode |
|Tab      | Save image and increase mass   |
|H        | Hide or show info about system |
|K        | Hide or show background image  |