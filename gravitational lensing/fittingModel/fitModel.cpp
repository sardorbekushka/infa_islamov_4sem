#include <iostream>
#include "../lensSolver.hpp"
#include "../renderer.hpp"

sf::Image createSource(int width, int height, int x, int y, int r) {
    sf::RenderWindow window(sf::VideoMode(width, height), "source");
    sf::Texture texture;
    texture.create(width, height); 

    sf::CircleShape source(r);
    source.setFillColor(sf::Color::White);
    source.setPosition(sf::Vector2f(x-r, y-r));

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
			if (event.type == sf::Event::Closed)
				window.close();
        window.clear();
        window.draw(source);
        window.display();
        texture.update(window); 
    }
    sf::Image background = texture.copyToImage();
    return background;
}

class FitRenderer: public Renderer {
    sf::Image background;
	bool showBackground;
public:
    FitRenderer(LensSolver *solver, sf::Image source, float realWidth, int dx, int dy, std::string backgroundImage, std::string title): Renderer(solver, source, realWidth, dx, dy, title), 
																														showBackground(true)
    {
        if (!background.loadFromFile(backgroundImage))
    		throw std::runtime_error("Failed to open file.");
    }

	void keyboardHandle(sf::Event event, double maxMass, double step) {
		Renderer::keyboardHandle(event);

		if (event.key.code == sf::Keyboard::Tab){
			if (solver->getMass() > maxMass)
				return;	
			saveImageInfo(saveImagesDirectory);
			solver->updateMass(step);
			}

		if (event.key.code == sf::Keyboard::K)
			showBackground = !showBackground; 
	}

    int poll(double maxMass, double step) {
        sf::Font font;
		font.loadFromFile(fontName);

		sf::Text zoomText, precText;
		precText.setFont(font);
		precText.setFillColor(sf::Color::White);
		precText.setCharacterSize(fontSize);

        sf::Texture texture;
        texture.create(width, height);
        sf::Sprite sprite;
 
        sf::Texture backTexture;
        backTexture.create(width, height);
        sf::Sprite backSprite;
        backSprite.setScale(width / background.getSize().x * 1.04, height / background.getSize().y * 1.04);
        backSprite.setColor(sf::Color(255, 255, 255, 150));
		sf::Image image;

		sf::Clock clock;
		float currentTime;
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
			currentTime = clock.restart().asSeconds();
			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				else if (event.type == sf::Event::KeyPressed)
				{
					keyboardHandle(event, maxMass, step);
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

			image.create(width, height, pixels);
			image.createMaskFromColor(sf::Color::Black);
			texture.update(image);
            sprite.setTexture(texture);

            backTexture.update(background);
            backSprite.setTexture(backTexture);
           
			if (showBackground)
            	window.draw(backSprite);

			window.draw(sprite);

			precText.setString(modelInfo +  '\n' + \
							   "lens mass: " + mass.str() + " kg" + '\n' + \
							   "einstain angle: " + einstAngle_.str() + " rad\n" + \
							   "FPS: " + std::to_string(1 / currentTime) + '\n' \
							    );
			if (hideInfo) precText.setString("");
			precText.setPosition(sf::Vector2f(10, 0));
			window.draw(precText);

			window.display();
    	}
		return EXIT_SUCCESS;
    }
};

int main() {
    float z1 = 0.227;
    float z2 = 0.9313;
    float width = 2.4241e-5;
    float height = 2.4241e-5;
	float realWidth = 5;
    unsigned heightPix = 500;
    unsigned widthPix = 500;
	double lensX = 1.149e-05;
	double lensY = 1.35262e-05;
	int sourceX = -11;
	int sourceY = 24;
	double minMass = 2.2e41;
	double maxMass = 2.8e41;
	double step = 4e-3;
	std::string background = "output_images/mask.png";
	
	auto solver = new LensSolver(minMass, z1, z2, lensX, lensY);
	FitRenderer renderer(solver, createSource(widthPix, heightPix, widthPix/2, heightPix/2, 15), realWidth, sourceX, sourceY, background, "");
	int code = renderer.poll(maxMass, step);
	delete solver;
    return code;
}
