# Strong gravitational lensing model

It is a model of strong gravitational lensing in the point source and point lens approximation. 
Each point is considered as a separate source refracted by massive object. For renderin is using SFML graphic library.

### Required
- C++ compiler (C++17 standart was used)
- [SFML](https://www.sfml-dev.org/) 
- [gsl](https://www.gnu.org/software/gsl/doc/html/index.html) ([gsl_integration.h](https://www.gnu.org/software/gsl/doc/html/integration.html))

There are some examples of result model

![Lensing of Bubble Nebula](https://github.com/meIonmusk/infa_islamov_4sem/tree/main/gravitational%20lensing/output_images/BubbleNebula.jpeg "Lensing of Bubble Nebula")
![Lensing of California Nebula](https://github.com/meIonmusk/infa_islamov_4sem/tree/main/gravitational%20lensing/output_images/CaliforniaNebula.jpeg "Lensing of Bubble Nebula")
![Lensing of Orion Nebula](https://github.com/meIonmusk/infa_islamov_4sem/tree/main/gravitational%20lensing/output_images/OrionNebula.jpeg "Lensing of Orion Nebula")
![Lensing of Running Man Nebula](https://github.com/meIonmusk/infa_islamov_4sem/tree/main/gravitational%20lensing/output_images/RunningManNebula.jpeg "Lensing of Running man Nebula")

And some tips about using. To compile and run code execute `complile.sh` file in this directory or use following expression 
    
    g++ -std=c++17 main.cpp -I/opt/local/include/ /opt/local/lib/libsfml-graphics.dylib /opt/local/lib/libsfml-audio.dylib  /opt/local/lib/libsfml-window.dylib /opt/local/lib/libsfml-system.dylib -Ofast -lgsl -lcblas -o main

    ./main

### Controls

| Command |              Action            |
|---------|--------------------------------|
|LMB Click|  Set the lens position         |
|LMB Drag | Lens is following the mouse    |
|Arrows   | Move the lens in this direction|
|Plus     | Increase the lens mass         |
|Backspace| Decrease the lens mass         |
|Enter    | Switch magnification show mode |