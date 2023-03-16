/**
 * This header file is part of {{mandelbrot}}.
 * 
 * There are defined classes FractalRenderer for rendering fractals.
*/
#include <iostream>
#include <string.h>
#include <SFML/Graphics.hpp>
#include "lensSolver.hpp"
// #include "pointLensSolver.hpp"


template <typename T>
class Renderer final {
private:
    sf::RenderWindow window;
    // sf::Uint8 *pixels = nullptr;
    sf::Image source;
	sf::Image image;
    unsigned width, height;
    LensSolver<T> *solver = nullptr;
	double scale;

    /** 
	 * Maps some value to the RGBA colorscheme.
	 * 
	 * @param t the value which will be mapped to the color.
	 * 
	*/
    sf::Color getSourceColor(int x, int y) {
        return source.getPixel(x, y);
	}

    /**
	 * Sets the color of pixel with coordinate (x, y) associated with rgb_colorscheme.
	 * 
	 * @param x horisontal coordinate of pixel.
	 * @param y vertical coordinate of pixel.
	 * @param t the value which will be mapped to the color (depends on iterations number).
	*/
    void setPixelColor(int x, int y, sf::Color color) {
        // if (x < 0 || x > width || y < 0 || y > height)
            // return;
		// image.setPixel(x, y, color + image.getPixel(x, y));
		image.setPixel(x, y, color);
	}

	template <typename U>
	bool checkPoint(U x, U y) {
		return (x > 0 && x < width && y > 0 && y < height);
	}
	/**
	 * Handles mouse events. Can zoom and move the view to the cursor where the button was pressed.
	*/
    void mouseHandle() {
		sf::Vector2i posPixel = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(posPixel);
		solver->setLensCenter((T)pos.x * scale, (T)pos.y * scale);
		// std::cout << pos.
		// solver->setLensCenter(-(T)pos.x + (T)width / 2, -(T)pos.y + (T)height / 2);
	}

	/**
	 * Handles keyboard events. It can move the center of view, change max iterations count, rescale the view and reset settings. 
	*/
    /*void keyboardHandle(sf::Event event) {
		double k = 0.2 * fractal->getStartScale();
        
		if (event.key.code == sf::Keyboard::Right)
			fractal->updateCenterX(-k);
		else if (event.key.code == sf::Keyboard::Left)
			fractal->updateCenterX(k);
		if (event.key.code == sf::Keyboard::Up)
			fractal->updateCenterY(k);
		else if (event.key.code == sf::Keyboard::Down)
			fractal->updateCenterY(-k);

		if (event.key.code == sf::Keyboard::Equal)
			fractal->updateMaxIterations(10);
		else if (event.key.code == sf::Keyboard::Backspace)
			fractal->updateMaxIterations(-10);

		if (event.key.code == sf::Keyboard::Enter)
			fractal->rescale(scale_param);
		else if (event.key.code == sf::Keyboard::RShift)
			fractal->rescale(1 / scale_param);

		if (event.key.code == sf::Keyboard::Escape)
            fractal->reset();
	}*/

public: 
    /**
	 * Constructor of the class. 
	 * 
	 * @param fractal pointer to object of Fractal (or its subclass) type
     * @param title title of the window (unnecessary)
	*/
    Renderer(LensSolver<T> *solver , std::string filename, std::string title): solver(solver)
	//  , height(HEIGHT), width(WIDTH), 
                                                    //  window(sf::VideoMode(WIDTH, HEIGHT), title) {
                                                    //  pixels(new sf::Uint8[WIDTH * HEIGHT * 4]), scale_param(1) {
        {
			if (!source.loadFromFile(filename))
        {
            // delete [] pixels;
            throw std::runtime_error("Failed to open file.");
        }
		if (!image.loadFromFile(filename))
        {
            // delete [] pixels;
            throw std::runtime_error("Failed to open file.");
        }
		height = source.getSize().y;
		width = source.getSize().x;
		// scale = solver->getEinstainAngle() / height * 8;
		scale = 1e-7;
		solver->setLensCenter(width / 2 * scale, height / 2 * scale);
		// std::cout << scale << std::endl;
		window.create(sf::VideoMode(width, height), title);
    }


    Renderer(LensSolver<T> *solver, std::string filename): Renderer(solver, filename, "Gravitational lens model") {}

    /**
     * Sets all pixels' colors.
    */
    void processImage() {
		// image.copy(sf::Image(image.getSize().x, image.getSize().y, sf::Color::Black));
		image.create(image.getSize().x, image.getSize().y);
		// image.loadFromFile("resources/galaxy.jpg");
		for (unsigned y = 0; y < height; y++)
            for (unsigned x = 0; x < width; x++)
                processPoint(x, y);
    }

    /**
     * Sets color of pixel with (x, y) coordinates.
    */
    void processPoint(unsigned x, unsigned y) {
		// std::cout << x << std::endl;
		// std::array<double, 2> &magnification = {1, 1};
		double magnification[2] {1, 1};
        std::array<Point<T>, 2> imagePositions = solver->processPoint((T)x * scale, (T)y * scale, magnification);
        // auto imagePos1 = imagePositions[0];
        // auto imagePos2 = imagePositions[1];
		// for (auto p = imagePositions.begin(); p != imagePositions.end(), ++p) {
		// std::cout << "ok" << std::endl;
		auto color = getSourceColor(x, y);
		for (int i = 0; i < 2; i++) {
			auto p = imagePositions[i] / scale;
			auto m = magnification[i];
			// получилось так что первая точка не меняется а вторая выходит за пределы. типа
			// 524 524 296 296
			// -1888 524 -984 296
			if (!checkPoint(p.x, p.y)) 							
				return;
			unsigned x_ = (unsigned)std::round(p.x);
			unsigned y_ = (unsigned)std::round(p.y);
			sf::Color deltaColor(color.r * (m - 1), color.g * (m - 1), color.b * (m - 1));
			setPixelColor(x_, y_, color + deltaColor);
			// if (std::abs((double)x - width / 2) < 2 && std::abs((double)y - height/2) < 2) 
				// std::cout << m << std::endl;
				// std::cout << x_ << ' ' << x << ' ' << y_ << ' ' << y << '\n'; 
			// if (x < 2 && y < 2)
			// setPixelColor(x_, y_, sf::Color(255, 255, 255));
		}
		
		

		// for (auto &p : imagePositions) {
		// 	if (!checkPoint(p.x, p.y)) 
		// 		return;
		// 	unsigned x_ = (unsigned)std::ceil(p.x);
		// 	unsigned y_ = (unsigned)std::ceil(p.y);
		// 	setPixelColor(x_, y_, getSourceColor(x_, y_));
		// }
    }

    /**
	 * The main method that polls the image and responds to any events.
	 * 
	 * @return information about successfully ending the polling (if program was aborted it doesn't return anything).
	*/
    int poll(){
		sf::Font font;
		font.loadFromFile(fontName);

		sf::Text zoomText, precText;
		// zoomText.setFont(font);
		precText.setFont(font);
		// zoomText.setFillColor(sf::Color::White);
		precText.setFillColor(sf::Color::White);
		// zoomText.setCharacterSize(24);
		precText.setCharacterSize(24);

        sf::Texture texture;
        texture.create(width, height);
        sf::Sprite sprite;
		// sprite.setScale
		// processImage();

		// auto startTime = std::time(nullptr);
		// auto currentTime = time(nullptr);
		sf::Clock clock;
		float startTime;
		while (window.isOpen())
    	{
			// startTime = std::time(nullptr);
			
			startTime = clock.getElapsedTime().asSeconds();
			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				// else if (event.type == sf::Event::KeyPressed)
				// {
					// keyboardHandle(event);
                    // setPixels();
				// }

                else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
					mouseHandle();
                    processImage();
				}
			}

            window.clear();

            // processImage();
            // texture.loadFromImage(image);
			texture.update(image);
            sprite.setTexture(texture);
            window.draw(sprite);

			// zoomText.setString("Zoom: " + std::to_string(fractal->getScale() / fractal->getStartScale()));
			// sf::Time elapsed1 = clock.getElapsedTime
			precText.setString("FPS: " + std::to_string(1 / (clock.getElapsedTime().asSeconds() - startTime)));
			// precText.setPosition(sf::Vector2f(0, 32));
			// window.draw(zoomText);
			window.draw(precText);	
            
			window.display();
    	}
		return EXIT_SUCCESS;
	}

    ~Renderer() {
        window.close();
        // delete [] pixels;
    }
};
