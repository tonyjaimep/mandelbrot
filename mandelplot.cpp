#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <complex>
#include <iostream>

using namespace std;

#define ITERATION_LIMIT 2 * 0xff - 2

#define DEFAULT_MIN_DISPLAYED_VALUE_X -2.0f
#define DEFAULT_MAX_DISPLAYED_VALUE_X 0.47f

#define DEFAULT_MIN_DISPLAYED_VALUE_Y -1.12f
#define DEFAULT_MAX_DISPLAYED_VALUE_Y 1.12f

#define DEFAULT_WINDOW_WIDTH 300 * (DEFAULT_MAX_DISPLAYED_VALUE_X - DEFAULT_MIN_DISPLAYED_VALUE_X)
#define DEFAULT_WINDOW_HEIGHT 300 * (DEFAULT_MAX_DISPLAYED_VALUE_Y - DEFAULT_MIN_DISPLAYED_VALUE_Y)

sf::Color mandelplot(complex<double>);
complex<double> toSpacePoint(sf::Vector2f, sf::Vector2u, sf::Rect<double>);
void drawMandelplot(sf::Rect<double>, sf::RenderTarget*);

int main()
{
  // initialize window
  sf::RenderWindow window(
    sf::VideoMode(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT),
    "Mandelbrot",
    sf::Style::None
  );

  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  sf::Event event;
  sf::Texture texture;
  sf::Shader shader;
  sf::Sprite sprite;

  // check if shaders are supported
  if (!sf::Shader::isAvailable()) {
    cerr << "ERROR: Shaders are not available in your machine." << endl;
    return 1;
  }

  // try initializing shader
  shader.loadFromFile("shader.frag", sf::Shader::Fragment);

  // share variables with shader
  shader.setUniform("resolution", sf::Vector2f(window.getSize()));
  shader.setUniform(
    "min_displayed",
    sf::Vector2f(
      DEFAULT_MIN_DISPLAYED_VALUE_X,
      DEFAULT_MIN_DISPLAYED_VALUE_Y
    )
  );
  shader.setUniform(
    "max_displayed",
    sf::Vector2f(
      DEFAULT_MAX_DISPLAYED_VALUE_X,
      DEFAULT_MAX_DISPLAYED_VALUE_Y
    )
  );

  // initialize sprite that covers entire screen
  // our shader will shade this empty sprite
  texture.create(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
  sprite.setTexture(texture);

  while (window.isOpen()) {
    while (window.pollEvent(event)) {
      // allow quitting with q or ESC
      if (event.type == sf::Event::Closed
          || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)
          || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        window.close();
      }
    }

    window.clear();
    window.draw(sprite, &shader);
    window.display();
  }

  return 0;
}
