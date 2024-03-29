#pragma once 

#include <iostream>
#include <string.h>
#include <SFML/Graphics.hpp>
#include "lensSolver.hpp"
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string lastFile(std::string directory) {
	std::string name;
	for (const auto & entry : fs::directory_iterator(directory))
		if (name == "") name = entry.path();
	return name;
}

class Renderer {
protected:
    sf::RenderWindow window;			// the window where render is going
    sf::Image source;					// source image that will be refracted
    unsigned width, height;				// width and height of the window
    LensSolver *solver = nullptr;		// pointer the LensSolver object will be used in calculations
	sf::Uint8 *pixels = nullptr;		// array with information about pixels color
	double scale;						// scale param (ratio of real size to the number of pixels in window)
	bool showMagnification;				// flag shows if magnification will be shown
	bool hideInfo; 						// flag shows if model info will be hidden
	int dx, dy;

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
	 * @param x the horizontal coordinate of the pixel
	 * @param y the vertical coordinate of the pixel
	 * @param color the color which will be set
	 * @param m the magnification at this point
	*/
	void setPixelColor(unsigned x, unsigned y, sf::Color& color, float m) {
		int newColor[3] = {color.r, color.g, color.b};

		for (int i = 0; i < 3; i++) {
			int colorComponent = newColor[i] * m;
			newColor[i] = colorComponent > 255 ? 255 : colorComponent;
			pixels[4 * (width * (y-dy) + x-dx) + i] = newColor[i];
		}
	}

	/**
	 * Handles mouse events. Moves lens center to the mouse position.
	*/
    void mouseHandle() {
		sf::Vector2i posPixel = sf::Mouse::getPosition(window);
		sf::Vector2f pos = window.mapPixelToCoords(posPixel);
		solver->setLensCenter(pixToRad(pos.x), pixToRad(pos.y));
	}

	/**
	 * Handles keyboard events. Can move the lens, change lens mass and switch the magnication mode
	*/
    void keyboardHandle(sf::Event event) {
		float delta = pixToRad(SHIFT);

		if (event.key.code == sf::Keyboard::Right)
			solver->moveLens(delta, 0);
		else if (event.key.code == sf::Keyboard::Left)
			solver->moveLens(-delta, 0);

		if (event.key.code == sf::Keyboard::Up)
			solver->moveLens(0, -delta);
		else if (event.key.code == sf::Keyboard::Down)
			solver->moveLens(0, delta);

		// float k = 0.1;
		if (event.key.code == sf::Keyboard::Equal)
			solver->updateMass(massSHIFT);
			
		else if (event.key.code == sf::Keyboard::BackSpace)
			solver->updateMass(-massSHIFT);

		int kx = 0;
		int ky = 0;
		if (event.key.code == sf::Keyboard::D)
			kx = -1;
		else if (event.key.code == sf::Keyboard::A)
			kx = 1;
		if (event.key.code == sf::Keyboard::S)
			ky = -1;
		else if (event.key.code == sf::Keyboard::W)
			ky = 1;
		dx += kx * SHIFT;
		dy += ky * SHIFT;
		solver->moveLens(kx * delta, ky * delta);

		if (event.key.code == sf::Keyboard::Space) 
			showMagnification = !showMagnification;
		if (event.key.code == sf::Keyboard::H) 
			hideInfo = !hideInfo;
		if (event.key.code == sf::Keyboard::Enter)
			saveImageInfo(saveImagesDirectory);
	}

	sf::Image saveImage(std::string directory) {
		std::stringstream ss;
		sf::Texture texture;
		texture.create(width, height);
		texture.update(window); 
    	sf::Image image = texture.copyToImage();
		std::string filename = lastFile(directory);
		int idx = filename.find("-");
    	ss << filename.substr(0, idx) + "-";
    	ss << std::stoi(filename.substr(idx + 1, filename.length() - idx - 4)) + 1;
		image.saveToFile(ss.str() + ".png");

		return image;
	}

	sf::Image saveImageInfo(std::string directory) {
		std::stringstream ss;
		sf::Texture texture;
		texture.create(width, height);
		texture.update(window); 
    	sf::Image image = texture.copyToImage();
		auto p = solver->getLensCenter();
		ss << directory << "lens_" << p.x << '_' << p.y
						<< "_source_" << dx << '_' << dy 
						<< "_mass_" << solver->getMass() << ".png";
		image.saveToFile(ss.str());

		return image;
	}

	double pixToRad(int pix) {
		return pix * scale;
	}

	double radToPix(double rad) {
		return rad / scale;
	}

	Point radToPix(Point rad) {
		return rad / scale;
	}

	Point pixToRad(Point pix) {
		return pix * scale;
	}
public: 
	Renderer(LensSolver *solver, sf::Image source, float realWidth, int dx=0, int dy=0, std::string title="gravitation lensing"): solver(solver), source(source), showMagnification(true),
																						height(source.getSize().y), width(source.getSize().x),
																						scale(realWidth * 4.8481e-6 / source.getSize().x), dx(dx), dy(dy)
	{
		if (radToPix(solver->getEinstainAngle()) > std::min(height, width)) 
			std::cerr << "Warning! The lens too big for the image." << std::endl;

		pixels = new sf::Uint8[width * height * 4];
		window.create(sf::VideoMode(width, height), title);																							
	}
	/**
	 * @param solver pointer to the LensSolver object
	 * @param filename the path to the file with image for background (source)
	 * @param realWidth real width of the object on image in arseconds
	 * @param title the title of the window
	 * 
	 * @throw std::runtime_error is thrown if the 'filename' couldn't be open
	*/

    Renderer(LensSolver *solver, std::string filename, float realWidth, std::string title): solver(solver), showMagnification(true), dx(0), dy(0)
    {
		if (!source.loadFromFile(filename))
    		throw std::runtime_error("Failed to open file.");

		height = source.getSize().y;
		width = source.getSize().x;
		scale = realWidth * 4.8481e-6 / width;

		if (radToPix(solver->getEinstainAngle()) > std::min(height, width)) 
			std::cerr << "Warning! The lens too big for the image." << std::endl;

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
	Renderer(LensSolver *solver, std::string filename): Renderer(solver, filename, 180, "Gravitational lens model") {}
	
	/**
	 * processes an image in straight way. each point in source splits to the calculated positions
	*/
    void processImage() {
		if (showMagnification)
			for (unsigned y = 0; y < height; y++)
				for (unsigned x = 0; x < width; x++) 
					processPoint(x, y);
		else
			for (unsigned y = 0; y < height; y++)
				for (unsigned x = 0; x < width; x++) 
					processPointWithoutMagn(x, y);
    }

	/**
	 * processes an image in reverse way. for each point is calculated where is the original point in the source
	*/
	void reverseProcessImage() {
		if (showMagnification)
			for (unsigned y = 0; y < height; y++)
				for (unsigned x = 0; x < width; x++) 
					reverseProcessPoint(x + dx, y + dy);
		else
			for (unsigned y = 0; y < height; y++)
				for (unsigned x = 0; x < width; x++) 
					reverseProcessPointWithoutMagn(x + dx, y + dy);
	}

	/**
	 * processes a pixel in straight way. the point splits to the calculated positions
	*/
	void processPoint(unsigned x, unsigned y) {
		float magnification[2] {1, 1};
        std::array<Point, 2> imagePositions = solver->processPoint(pixToRad(x), pixToRad(y), magnification);

		auto color = getSourceColor(x, y);
		for (int i = 0; i < 2; i++) {
			auto p = radToPix(imagePositions[i]);
			auto m = magnification[i];
			if (!checkPoint(p.x, p.y)) 							
				return;

			setPixelColor(p.x, p.y, color, m);
		}
    }
	
	/**
	 * processes a pixel in straight way without magnification
	*/
	void processPointWithoutMagn(unsigned x, unsigned y) {
		float magnification[2] {1, 1};
        std::array<Point, 2> imagePositions = solver->processPoint(pixToRad(x), pixToRad(y), magnification);

		auto color = getSourceColor(x, y);
		for (int i = 0; i < 2; i++) {
			auto p = radToPix(imagePositions[i]);
			if (!checkPoint(p.x, p.y)) 							
				return;

			setPixelColor(p.x, p.y, color, 1);
		}
    }

	/**
	 * processes an image in reverse way. for the point is calculated where is the original position in the source
	*/
	void reverseProcessPoint(unsigned x, unsigned y) {
		float magnification = 1.0;
		float &m = magnification;
		auto p = radToPix(solver->reverseProcessPoint(pixToRad(x), pixToRad(y), m));
		auto color = getSourceColor(p.x, p.y);
		
		m = (m > 2) ? 2 : (m < 0.25) ? 0.25 : m;

		setPixelColor(x, y, color, m);
	}

	/**
	 * processes an image in reverse way without magnification
	*/
	void reverseProcessPointWithoutMagn(unsigned x, unsigned y) {
		float m = 1.0;
		auto p = radToPix(solver->reverseProcessPoint(pixToRad(x), pixToRad(y), m));
		auto color = getSourceColor(p.x, p.y);
	
		setPixelColor(x, y, color, 1);
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
		std::ostringstream einstAngle_;
		sourceZ << solver->getSourceRedshift();
		lensZ << solver->getLensRedshift();
		mass << solver->getMass();
		einstAngle_ << solver->getEinstainAngle();

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
					einstAngle_.str("");
					einstAngle_ << solver->getEinstainAngle();
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
							   "einstain angle: " + einstAngle_.str() + " rad\n" + \
							   "FPS: " + std::to_string(1 / currentTtime) + '\n' \
							    );
			if (hideInfo) precText.setString("");
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

