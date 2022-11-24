#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <array>
#include <complex>
#include <iostream>

using namespace std;

#define ITERATION_LIMIT 2 * 0xff - 2

sf::Color mandelplot(complex<double>);
complex<double> toSpacePoint(sf::Vector2f, sf::Vector2u, sf::Rect<double>);
void drawMandelplot(sf::Rect<double>, sf::RenderTarget*);

sf::RenderWindow* initializeWindow();

int main()
{
	bool mouseClicked = false;

	sf::RenderWindow* window = initializeWindow();
	sf::Event event;

	sf::RenderTexture mandelplotRenderTexture;
	sf::Rect<double> currentView(
			0,
			0,
			window->getSize().x,
			window->getSize().y
	);
	mandelplotRenderTexture.create(window->getSize().x, window->getSize().y);
	drawMandelplot(currentView, &mandelplotRenderTexture);

	mandelplotRenderTexture.display();
	sf::Sprite mandelbrotSet(mandelplotRenderTexture.getTexture());

	while (window->isOpen()) {
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed
					|| sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window->close();
			}

			if (event.type == sf::Event::MouseButtonPressed && !mouseClicked)
				mouseClicked = true;

			// ensure this code is run only once on each click
			if (event.type == sf::Event::MouseButtonReleased && mouseClicked) {
				if (event.mouseButton.button == sf::Mouse::Button::Left) {
					currentView.width *= 0.5;
					currentView.height *= 0.5;
				}
				if (event.mouseButton.button == sf::Mouse::Button::Right) {
					currentView.width *= 2;
					currentView.height *= 2;
				}

				mouseClicked = false;

				cout
					<< sf::Mouse::getPosition(*window).x
					<< ", " << sf::Mouse::getPosition(*window).y
					<< endl;

				currentView.left =
					sf::Mouse::getPosition(*window).x - (currentView.width / 2);
				currentView.top =
					sf::Mouse::getPosition(*window).y - (currentView.height / 2);

				if (currentView.top < 0) {
					currentView.top = 0;
				}
				if (currentView.left < 0) {
					currentView.left = 0;
				}

				drawMandelplot(currentView, &mandelplotRenderTexture);
			}
		}

		window->draw(mandelbrotSet);
		window->display();
	}

	return 0;
}

sf::Color mandelplot(complex<double> complexPoint) {
	int iterations = 0;
	complex<double> z = 0;
	float tempReal;

	while (iterations < ITERATION_LIMIT){
		if (z.real() * z.real() + z.imag() + z.imag() > 4)
			break;

		tempReal = z.real() * z.real() - z.imag() * z.imag() + complexPoint.real();
		z.imag(2 * z.real() * z.imag() + complexPoint.imag());
		z.real(tempReal);

		iterations++;
	}

	return sf::Color(iterations % 255, iterations % 255, iterations % 255);
}

complex<double> toSpacePoint(sf::Vector2f pointInWindow, sf::Vector2u windowSize, sf::Rect<double> viewChunk)
{
	// complex point to return
	complex<double> pointInSpace;

	pointInWindow.x += viewChunk.left;
	pointInWindow.y += viewChunk.top;

	pointInSpace.real(
		(pointInWindow.x - windowSize.x / 2.0) / (windowSize.x / 4.0)
	);
	pointInSpace.imag(
		(pointInWindow.y - windowSize.y / 2.0) / (windowSize.x / 4.0)
	);

	pointInSpace.real(pointInSpace.real() * (viewChunk.width / windowSize.x));
	pointInSpace.imag(pointInSpace.imag() * (viewChunk.height / windowSize.y));

	return pointInSpace;
}

/* draws given viewChunk of the mandelbrot set in the given renderTarget */
void drawMandelplot(sf::Rect<double> viewChunk, sf::RenderTarget* renderTarget)
{
	// raster position in the render target
	sf::Vector2f currentRasterPoint(1, 1);
	// this rectangle will draw every pixel as a point in the set
	sf::RectangleShape rasterPoint(sf::Vector2f(1, 1));

	// move the rasterpoint to the beginning
	rasterPoint.setPosition(currentRasterPoint);

	// TODO: Use a shader?
	// loop through each pixel in the RenderTarget
	for (
		;
		currentRasterPoint.y <= renderTarget->getSize().y;
		currentRasterPoint.y++
	) {
		for (
			currentRasterPoint.x = 1;
			currentRasterPoint.x <= renderTarget->getSize().x;
			currentRasterPoint.x++
		) {
			rasterPoint.setPosition(currentRasterPoint);
			rasterPoint.setFillColor(
				mandelplot(
					toSpacePoint(
						currentRasterPoint,
						renderTarget->getSize(),
						viewChunk
					)
				)
			);
			renderTarget->draw(rasterPoint);
		}
	}
}

sf::RenderWindow* initializeWindow()
{
	sf::RenderWindow* window = new sf::RenderWindow(
		sf::VideoMode(700, 700),
		"Mandelbrot",
		sf::Style::None
	);
	return window;
}
