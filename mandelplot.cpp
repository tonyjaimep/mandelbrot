#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
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

  sf::Vector2f minDisplayedValues(
    DEFAULT_MIN_DISPLAYED_VALUE_X,
    DEFAULT_MIN_DISPLAYED_VALUE_Y
  );

  sf::Vector2f maxDisplayedValues(
    DEFAULT_MAX_DISPLAYED_VALUE_X,
    DEFAULT_MAX_DISPLAYED_VALUE_Y
  );

  // check if shaders are supported
  if (!sf::Shader::isAvailable()) {
    cerr << "ERROR: Shaders are not available in your machine." << endl;
    return 1;
  }

  // try initializing shader
  shader.loadFromFile("shader.frag", sf::Shader::Fragment);

  // share variables with shader
  shader.setUniform("resolution", sf::Vector2f(window.getSize()));

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

      if (event.type == sf::Event::MouseWheelScrolled) {
        float scrollDirection = event.mouseWheelScroll.delta;
        float oldRangeX = maxDisplayedValues.x - minDisplayedValues.x;
        float deltaX = oldRangeX * 0.1;
        float oldRangeY = maxDisplayedValues.y - minDisplayedValues.y;
        float deltaY = oldRangeY * 0.1;

        sf::Vector2i mousePosition = sf::Mouse::getPosition();
        sf::Vector2u windowSize = window.getSize();

        cout << "Mouse position: " << mousePosition.x << ", " << mousePosition.y << endl;
        cout << "Window size: " << windowSize.x << ", " << windowSize.y << endl;

        float mousePercentLeft = float(mousePosition.x) / float(windowSize.x);
        float mousePercentTop = float(windowSize.y - mousePosition.y) / float(windowSize.y);

        cout << "mouse percent top: " << mousePercentTop << endl;
        cout << "mouse percent left: " << mousePercentLeft << endl;

        if (
          mousePosition.x < 0
          || mousePosition.y < 0
          || mousePosition.x > windowSize.x
          || mousePosition.y > windowSize.y
        ) {
          continue;
        }

        float leftDelta = deltaX * mousePercentLeft;
        float rightDelta = deltaX - leftDelta;

        float topDelta = deltaY * mousePercentTop;
        float bottomDelta = deltaY - topDelta;

        minDisplayedValues.x += leftDelta * scrollDirection;
        minDisplayedValues.y += bottomDelta * scrollDirection;

        maxDisplayedValues.x -= rightDelta * scrollDirection;
        maxDisplayedValues.y -= bottomDelta * scrollDirection;
      }
    }

    shader.setUniform("min_displayed", minDisplayedValues);
    shader.setUniform("max_displayed", maxDisplayedValues);

    window.clear();
    window.draw(sprite, &shader);
    window.display();
  }

  return 0;
}
