#pragma once 

#include <iostream>
#include <string.h>
#include <SFML/Graphics.hpp>
#include "lensSolver.hpp"
#include <sstream>


class Renderer final {
private:
    sf::RenderWindow window;			// the window where render is going
    sf::Image source;					// source image that will be refracted
    unsigned width, height;				// width and height of the window
    LensSolver *solver = nullptr;		// pointer the LensSolver object will be used in calculations
	sf::Uint8 *pixels = nullptr;		// array with information about pixels color
	double scale;						// scale param (ratio of real size to the number of pixels in window)
	bool showMagnification;				// flag shows if magnification will be shown


	/**
	 * checks if the point belongs to the window
	 * 
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * 
	 * @return is the pixel in window
	*/
	template <typename U>
	bool checkPoint(U x, U y) {
		return (x > 0 && x < width && y > 0 && y < height);
	}

	/** gets the color in source image at the specified point
	 * 
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * 
	 * @return received color
	*/
    sf::Color getSourceColor(int x, int y) {
		if (!checkPoint(x, y))
			return sf::Color::Black;
        return source.getPixel(x, y);
	}

	/**
	 * sets the magnificated color of the pixel in array
	 * 
	 * @overload
	*/
    void setPixelColor(unsigned x, unsigned y, sf::Color& color, Point p) {
		for (int i = 0; i < 3; i++)
			pixels[4 * (width * y + x) + i] = 0;
        		
		float m = 1;
		// m = magn[(unsigned)std::round((Point(x, y) - p / scale).norm())];
		float r = color.r * m;
		float g = color.g * m;
		float b = color.b * m;

		int newColor[4] = {r > 255 ? 255 : (int)r, g > 255 ? 255 : (int)g, b > 255 ? 255 : (int)b, 255};

		for (int i = 0; i < 4; i++)
			pixels[4 * (width * y + x) + i] = newColor[i];
	}

	/**
	 * sets the magnificated color of the pixel in array
	 * 
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @param color the color which will be set
	 * @param m the magnification at this point
	*/
	void setPixelColor(unsigned x, unsigned y, sf::Color& color, float m) {
		int newColor[4] = {color.r, color.g, color.b, 255};

		// if (showMagnification)
		for (int i = 0; i < 3 * showMagnification; i++) {
			int colorComponent = newColor[i] * m;
			newColor[i] = colorComponent > 255 ? 255 : colorComponent;
		}

		// int r = color.r * m;
		// int g = color.g * m;
		// int b = color.b * m;

		// int newColor[4] = {r > 255 ? 255 : r, g > 255 ? 255 : g, b > 255 ? 255 : b, 255};
		// int newColor[4] = {r, g, b, 255};
		
		for (int i = 0; i < 4; i++)
			pixels[4 * (width * y + x) + i] = newColor[i];
	}

	/**
	 * Handles mouse events. Moves lens center to the mouse position.
	*/
    void mouseHandle() {
		sf::Vector2i posPixel = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(posPixel);
		solver->setLensCenter(pos.x * scale, pos.y * scale);
	}

	/**
	 * Handles keyboard events. Can move the lens, change lens mass and switch the magnication mode
	*/
    void keyboardHandle(sf::Event event) {
		auto delta = 5 * scale;

		if (event.key.code == sf::Keyboard::Right)
			solver->moveLens(delta, 0);
		else if (event.key.code == sf::Keyboard::Left)
			solver->moveLens(-delta, 0);

		if (event.key.code == sf::Keyboard::Up)
			solver->moveLens(0, -delta);
		else if (event.key.code == sf::Keyboard::Down)
			solver->moveLens(0, delta);

		double k = 0.1;
		if (event.key.code == sf::Keyboard::Equal)
			solver->updateMass(k);
			
		else if (event.key.code == sf::Keyboard::BackSpace)
			solver->updateMass(-k);

		if (event.key.code == sf::Keyboard::Enter) 
			showMagnification = !showMagnification;
	}

	/**
	 * initialize the magnifaction array
	 * 
	 * @param magn the array will be initialized
	*/
	void setMagnification(float *magn) {
		for (unsigned i = 0; i < std::sqrt(width * width + height * height) + 1; i++) {
			magn[i] = solver->magnification(i * scale);
			if (magn[i] > 2) magn[i] = 2;
			else if (magn[i] < 0.25) magn[i] = 0.25; 
		}
	}

public: 
	/**
	 * @param solver pointer to the LensSolver object
	 * @param filename the path to the file with image for background (source)
	 * @param realWidth real width of the object on image in arcminutes
	 * @param title the title of the window
	 * 
	 * @throw std::runtime_error is thrown if the 'filename' couldn't be open
	*/
    Renderer(LensSolver *solver , std::string filename, float realWidth, std::string title): solver(solver), showMagnification(true)
    {
		if (!source.loadFromFile(filename))
    		throw std::runtime_error("Failed to open file.");

		height = source.getSize().y;
		width = source.getSize().x;
		scale = realWidth * 29e-5 / width;

		if (solver->getEinstainAngle() / scale > std::min(height, width)) 
			std::cerr << "Warning! The lens too big for the image." << std::endl;

		solver->setLensCenter(width / 2 * scale, height / 2 * scale);
		pixels = new sf::Uint8[width * height * 4];
		window.create(sf::VideoMode(width, height), title);
    }

	/**
	 * @param solver pointer to the LensSolver object
	 * @param filename the name of the file with image for background (source)
	 * @param realWidth real width of the object on image in arcminutes
	*/
    Renderer(LensSolver *solver, std::string filename, float realWidth): Renderer(solver, filename, realWidth, "Gravitational lens model") {}
    /**
	 * @param solver pointer to the LensSolver object
	 * @param filename the name of the file with image for background (source)
	*/
	Renderer(LensSolver *solver, std::string filename): Renderer(solver, filename, 3, "Gravitational lens model") {}

	/**
	 * processes an image in straight way. each point in source splits to the calculated positions
	*/
    void processImage() {
		for (unsigned y = 0; y < height; y++)
            for (unsigned x = 0; x < width; x++)
                processPoint(x, y);
    }

	/**
	 * processes an image in reverse way. for each point is calculated where is the original point in the source
	*/
	void reverseProcessImage() {
		auto p = solver->getLensCenter();
		for (unsigned y = 0; y < height; y++)
			for (unsigned x = 0; x < width; x++) 
				reverseProcessPoint(x, y);
	}

	/**
	 * processes a pixel in straight way. the point splits to the calculated positions
	*/
	void processPoint(unsigned x, unsigned y) {
		double magnification[2] {1, 1};
        std::array<Point, 2> imagePositions = solver->processPoint(x * scale, y * scale, magnification);

		auto color = getSourceColor(x, y);
		for (int i = 0; i < 2; i++) {
			auto p = imagePositions[i] / scale;
			auto m = magnification[i];
			if (!checkPoint(p.x, p.y)) 							
				return;

			setPixelColor(p.x, p.y, color, m);
		}
    }

	/**
	 * processes an image in reverse way. for the point is calculated where is the original position in the source
	*/
	void reverseProcessPoint(unsigned x, unsigned y) {
		float magnification = 1.0;
		float &m = magnification;
		auto p = solver->reverseProcessPoint(x * scale, y * scale, m) / scale;
		auto color = getSourceColor(p.x, p.y);
		
		m = (m > 2) ? 2 : (m < 0.25) ? 0.25 : m;

		setPixelColor(x, y, color, m);
	}

    /**
	 * the main method that updates an image and responds to any events
	 * 
	 * @return information about successfully ending the polling (if program was aborted it doesn't return anything)
	*/
    int poll(){
		sf::Font font;
		font.loadFromFile(fontName);

		sf::Text zoomText, precText;
		precText.setFont(font);
		precText.setFillColor(sf::Color::White);
		precText.setCharacterSize(fontSize);

        sf::Texture texture;
        texture.create(width, height);

        sf::Sprite sprite;

		sf::Clock clock;
		float currentTtime;
		window.setFramerateLimit(FPS);

		void (Renderer::*update)();
		update = &Renderer::reverseProcessImage;

		std::ostringstream mass;
		std::ostringstream sourceZ;
		std::ostringstream lensZ;
		std::ostringstream scale_;
		std::ostringstream omegaM_;
		std::ostringstream omegaL_;
		
		sourceZ << solver->getSourceRedshift();
		lensZ << solver->getLensRedshift();
		mass << solver->getMass();

		scale_ << scale;
		omegaM_ << omegaM;
		omegaL_ << omegaL;

		std::string modelInfo = "H0: " + std::to_string(H0) + \
								", omegaM: " + omegaM_.str() + \
								", omegaL: " + omegaL_.str() + '\n' + \
								"source redshift: " + sourceZ.str() + '\n' + \
							    "lens redshift: " + lensZ.str() + '\n' + \
							    "scale: " + scale_.str() + " rad/pix";

		while (window.isOpen())
    	{
			currentTtime = clock.restart().asSeconds();
			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				else if (event.type == sf::Event::KeyPressed)
				{
					keyboardHandle(event);
					(*this.*update)();
					mass.str("");
					mass << solver->getMass();
				}
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
					mouseHandle();
                    (*this.*update)();
				}
			}

            window.clear();

			texture.update(pixels);
            sprite.setTexture(texture);
            window.draw(sprite);

			precText.setString(modelInfo +  '\n' + \
							   "lens mass: " + mass.str() + " kg" + '\n' + \
							   "FPS: " + std::to_string(1 / currentTtime) + '\n'
							    );
			precText.setPosition(sf::Vector2f(10, 0));
			window.draw(precText);	
            
			window.display();
    	}
		return EXIT_SUCCESS;
	}

    ~Renderer() {
        window.close();
        delete [] pixels;
    }
};

