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

bool isMouseInsideWindow(const sf::Window* window);
void pointZoom(const int, sf::Vector2f*, sf::Vector2f*, const sf::Window*);

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
        pointZoom(
          event.mouseWheelScroll.delta,
          &minDisplayedValues,
          &maxDisplayedValues,
          &window
        );
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

/**
 * Updates minDisplayedValues and maxDisplayedValues
 */
void pointZoom(
  // can be -1 or 1
  const int scrollDelta,
  sf::Vector2f* minDisplayedValues,
  sf::Vector2f* maxDisplayedValues,
  const sf::Window* window
)
{
  if (!isMouseInsideWindow(window)) return;

  sf::Vector2i mousePosition = sf::Mouse::getPosition(*window);
  sf::Vector2u windowSize = window->getSize();

  double mousePercentLeft = double(mousePosition.x) / double(windowSize.x);
  double mousePercentTop = double(mousePosition.y) / double(windowSize.y);

  double scaleFactor = 0.1;
  // add or subtract scaleFactor, depending on scroll direction
  double rangeScale = 1 + scaleFactor * scrollDelta;

  double oldXRange = maxDisplayedValues->x - minDisplayedValues->x;
  double oldYRange = maxDisplayedValues->y - minDisplayedValues->y;

  double newXRange = oldXRange * rangeScale;
  double newYRange = oldYRange * rangeScale;

  double newMinX =
    minDisplayedValues->x + (oldXRange - newXRange) * mousePercentLeft;
  double newMinY =
    minDisplayedValues->y + (oldYRange - newYRange) * (mousePercentTop / 1);

  minDisplayedValues->x = newMinX;
  minDisplayedValues->y = newMinY;
  maxDisplayedValues->x = newMinX + newXRange;
  maxDisplayedValues->y = newMinY + newYRange;
}

bool isMouseInsideWindow(const sf::Window* window)
{
  sf::Vector2i position = sf::Mouse::getPosition(*window);
  sf::Vector2u windowSize = window->getSize();

  if (position.x < 0 || position.y < 0) return false;
  if (position.x > windowSize.x || position.y > windowSize.y) return false;

  return true;
}
