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


class Renderer final {
private:
    sf::RenderWindow window;
    // sf::Uint8 *pixels = nullptr;
    sf::Image source;
	// sf::Image image;
    unsigned width, height;
    LensSolver *solver = nullptr;
	sf::Uint8 *pixels = nullptr;
	double scale;
	float *magn;
	// bool showMagnification;
	// sf::Color (*colorMap)(sf::Color, double);

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

	static sf::Color magnificateColor(sf::Color color, double m) {
		if (m > 4) m = 4;

		color.r = (color.r * m > 255) ? 255 : color.r * m;
		color.g = (color.g * m > 255) ? 255 : color.g * m;
		color.b = (color.b * m > 255) ? 255 : color.b * m;

		return color;
	}

	static sf::Color notMafnificateColor(sf::Color color, double m) {
		return color;
	}

    void setPixelColor(unsigned x, unsigned y, sf::Color& color, Point p) {
        // if (x < 0 || x > width || y < 0 || y > height)
            // return;
		// image.setPixel(x, y, color + image.getPixel(x, y));

		// auto r = (color.r * m > 255) ? 255 : color.r * m;
		// auto g = (color.g * m > 255) ? 255 : color.g * m;
		// auto b = (color.b * m > 255) ? 255 : color.b * m;

		// color.r = (color.r *= m > 255) ? 255 : 100;
		// color.g = (color.g *= m > 255) ? 255 : 50;
		// color.b = (color.b *= m > 255) ? 255 : 50;


		// if (m > 4) m = 4;
		// else if (m < 0.25) m = 0.25;
		
		 // короче попробуй сделать как в мандельброте типа хранить массив с пикселями и обновлять там данные, мб ускорит код

		// color.r = (color.r * m > 255) ? 255 : color.r * m;
		// color.g = (color.g * m > 255) ? 255 : color.g * m;
		// color.b = (color.b * m > 255) ? 255 : color.b * m;

		// pixels[4 * (width * y + x)] = (color.r * m > 255) ? 255 : color.r * m;
		// pixels[4 * (width * y + x) + 1] = (color.g * m > 255) ? 255 : color.g * m;
		// pixels[4 * (width * y + x) + 2] = (color.b * m > 255) ? 255 : color.b * m;
		// pixels[4 * (width * y + x) + 3] = 255;
		int r = color.r;
		int g = color.g;
		int b = color.b;
		
		// if (m > 1)
		// r =  (r * m > 255) ? 255 : r * m;
		// if (b > 300) 
			// std::cout << "ffr" << std::endl;
		auto m = magn[(unsigned)std::round(std::sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)))];
		r = (r * m > 255) ? 255 : r * m;
		g = (g * m > 255) ? 255 : g * m;
		b = (b * m > 255) ? 255 : b * m;
		// g *= m;
		// b *= m;

		pixels[4 * (width * y + x)] = r;
		pixels[4 * (width * y + x) + 1] = g;
		pixels[4 * (width * y + x) + 2] = b;
		
		pixels[4 * (width * y + x) + 3] = 255;
		
		// image.setPixel(x, y, color);
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
		solver->setLensCenter(pos.x * scale, pos.y * scale);
		// std::cout << pos.
		// solver->setLensCenter(-(T)pos.x + (T)width / 2, -(T)pos.y + (T)height / 2);
	}

	/**
	 * Handles keyboard events. It can move the center of view, change max iterations count, rescale the view and reset settings. 
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
	// 		fractal->updateMaxIterations(10);
		else if (event.key.code == sf::Keyboard::BackSpace)
			solver->updateMass(-k);

		// if (event.key.code == sf::Keyboard::Enter) {
		// 	showMagnification = !showMagnification;
		// 	colorMap = notMafnificateColor;
		// 	if (showMagnification) 
		// 		colorMap = magnificateColor;
		// }

		// std::cout << event.key.code << std::endl;
	// 		fractal->updateMaxIterations(-10);

	// 	if (event.key.code == sf::Keyboard::Enter)
	// 		fractal->rescale(scale_param);
	// 	else if (event.key.code == sf::Keyboard::RShift)
	// 		fractal->rescale(1 / scale_param);

	// 	if (event.key.code == sf::Keyboard::Escape)
    //         fractal->reset();
	}

	void setMagnification() {
		for (unsigned i = 0; i < std::sqrt(width * width + height * height) + 1; i++) {
			magn[i] = solver->magnification(i * scale);
			// magn[i] = 2;
		}
	
	}

public: 
    /**
	 * Constructor of the class. 
	 * 
	 * @param fractal pointer to object of Fractal (or its subclass) type
     * @param title title of the window (unnecessary)
	*/
    Renderer(LensSolver *solver , std::string filename, std::string title): solver(solver)//, showMagnification(true), colorMap(magnificateColor)
	//  , height(HEIGHT), width(WIDTH), 
                                                    //  window(sf::VideoMode(WIDTH, HEIGHT), title) {
                                                    //  pixels(new sf::Uint8[WIDTH * HEIGHT * 4]), scale_param(1) {
        {
			if (!source.loadFromFile(filename))
        {
            // delete [] pixels;
            throw std::runtime_error("Failed to open file.");
        }
		// if (!image.loadFromFile(filename))
        // {
            // delete [] pixels;
            // throw std::runtime_error("Failed to open file.");
        // }
		height = source.getSize().y;
		width = source.getSize().x;
		// scale = solver->getEinstainAngle() / height * 8;
		scale = 3e-2;
		solver->setLensCenter(width / 2 * scale, height / 2 * scale);
		pixels = new sf::Uint8[width * height * 4];
		// std::cout << scale << std::endl;
		window.create(sf::VideoMode(width, height), title);
		magn = new float[std::sqrt(width * width + height * height) + 1];
		setMagnification();
    }


    Renderer(LensSolver *solver, std::string filename): Renderer(solver, filename, "Gravitational lens model") {}

    /**
     * Sets all pixels' colors.
    */
    void processImage() {
		// image.copy(sf::Image(image.getSize().x, image.getSize().y, sf::Color::Black));
		// image.create(image.getSize().x, image.getSize().y);

		// image.loadFromFile("resources/galaxy.jpg");
		for (unsigned y = 0; y < height; y++)
            for (unsigned x = 0; x < width; x++)
                processPoint(x, y);
				

		// auto e = solver->getEinstainAngle() / scale;
		// auto center = solver->getLensCenter() / scale;
		// for 

		// for 
		// std::cout << e << ' ' << center.x << std::endl;
		// for (double y = -e-10 ; y < e+10; y++)
		// 	for (double x = -e-10 ; x < e+10; x++)
		// 		if (std::abs(std::pow(x, 2) + std::pow(y, 2) - std::pow(e, 2)) < 1000)
		// 			if (checkPoint(center.x + x, center.y + y)){
		// 				auto k = 255-std::abs(std::pow(x, 2) + std::pow(y, 2) - std::pow(e, 2)) / 4;
		// 										image.setPixel(center.x + x, center.y + y, image.getPixel(center.x + x, center.y + y) + sf::Color(k, k, k));

		// 			}
					// if (image.getPixel(center.x + x, center.y + y) == sf::Color::Black)
						// image.setPixel(x, y, sf::Color::White);
    }

	void reverseProcessImage() {
		// image.create(image.getSize().x, image.getSize().y);
		auto p = solver->getLensCenter();
		for (unsigned y = 0; y < height; y++)
			for (unsigned x = 0; x < width; x++) 
				reverseProcessPoint(x, y, p);
		
	}

	void reverseProcessPoint(unsigned x, unsigned y, Point center) {

		float magnification = 1.0;
		// m = magnification;
		auto p = solver->reverseProcessPoint(x * scale, y * scale, magnification) / scale;
		// if ((p - Point(500, 200)) * (p - Point(500, 200)) < 10)
			// std::cout << p.x << ' ' << x << std::endl;
		if (!checkPoint(p.x, p.y))
			return;
		auto color = getSourceColor(p.x, p.y);
		
		
		setPixelColor(x, y, color, center);

		// sf::Color deltaColor(color.r * (m - 1), color.g * (m - 1), color.b * (m - 1));
			// unsigned x_ = (unsigned)std::ceil(p.x);
			// unsigned y_ = (unsigned)std::ceil(p.y);
			// setPixelColor(x_, y_, color + deltaColor);
		// setPixelColor(x, y, color + deltaColor);
		// sf::Color newColor(color.r,)
		// sf::Color deltaColor(color.r * (m - 1), color.g * (m - 1), color.b * (m - 1));
		// std::cout << color.toInteger() << ' ' << deltaColor.toInteger() << std::endl;
		// setPixelColor(x, y, color + deltaColor);
		// auto col = ((int)color.toInteger() * m > 4294967295) ? 4294967295 : color.toInteger() * m;
		// setPixelColor(x, y, sf::Color(color.toInteger()));
		// setPixelColor(x, y, sf::Color(color.r * m, color.g * m, color.b * m));
	}
    /**
     * Sets color of pixel with (x, y) coordinates.
    */
    void processPoint(unsigned x, unsigned y) {
		// std::cout << x << std::endl;
		// std::array<double, 2> &magnification = {1, 1};
		double magnification[2] {1, 1};
        std::array<Point, 2> imagePositions = solver->processPoint(x * scale, y * scale, magnification);
        // auto imagePos1 = imagePositions[0];
        // auto imagePos2 = imagePositions[1];
		// for (auto p = imagePositions.begin(); p != imagePositions.end(), ++p) {
		// std::cout << "ok" << std::endl;
		auto color = getSourceColor(x, y);
		for (int i = 0; i < 2; i++) {
			auto p = imagePositions[i] / scale;
			auto m = magnification[i];
			if (!checkPoint(p.x, p.y)) 							
				return;
			// sf::Color deltaColor(color.r * (m - 1), color.g * (m - 1), color.b * (m - 1));
			// unsigned x_ = (unsigned)std::ceil(p.x);
			// unsigned y_ = (unsigned)std::ceil(p.y);
			// setPixelColor(x_, y_, color + deltaColor);
			setPixelColor(p.x, p.y, color, p);
			// setPixelColor(p.x, p.y + 1, color + deltaColor);
			// setPixelColor(p.x, p.y - 1, color + deltaColor);
			// setPixelColor(p.x + 1, p.y, color + deltaColor);
			// setPixelColor(p.x - 1, p.y, color + deltaColor);

			// x_ += 1;
			// y_ += 1;
			// setPixelColor(x_, y_, color + deltaColor);
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
		texture.setSmooth(true);
		// texture.setRepeated(true);
        sf::Sprite sprite;
		// sprite.setScale
		// processImage();

		// auto startTime = std::time(nullptr);
		// auto currentTime = time(nullptr);
		sf::Clock clock;
		float currentTtime;
		// window.setFramerateLimit(120);
		while (window.isOpen())
    	{
			// startTime = std::time(nullptr);
			currentTtime = clock.restart().asSeconds();
			// startTime = clock.getElapsedTime().asSeconds();
			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				else if (event.type == sf::Event::KeyPressed)
				{
					keyboardHandle(event);
                    reverseProcessImage();
                    // processImage();
				}

                else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                // else if (event.type == sf::Event::MouseMoved){
					mouseHandle();
					reverseProcessImage();
                    // processImage();
					
				}
			}

            window.clear();

            // processImage();
            // texture.loadFromImage(image);
			texture.update(pixels);

			// texture.update(image);

			// texture.setSmooth(true);

            sprite.setTexture(texture);
            window.draw(sprite);

			// zoomText.setString("Zoom: " + std::to_string(fractal->getScale() / fractal->getStartScale()));
			// sf::Time elapsed1 = clock.getElapsedTime
			precText.setString("FPS: " + std::to_string(1 / currentTtime));
			// precText.setPosition(sf::Vector2f(0, 32));
			// window.draw(zoomText);
			window.draw(precText);	
            
			window.display();
    	}
		return EXIT_SUCCESS;
	}

    ~Renderer() {
        window.close();
        delete [] pixels;
		delete [] magn;
    }
};
