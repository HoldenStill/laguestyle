#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

// Stamps multiple brush circles between two mouse positions to hide
// the gaps caused by only sampling the mouse once per frame. Returns
// the number of stamps through stampCount so it can be logged.
static void StampBrushStroke(sf::RenderTexture& canvas,
	sf::Vector2f from,
	sf::Vector2f to,
	sf::Color color,
	float radius,
	int& stampCount)
{
	// Overlap each stamp by half the radius so no background shows through.
	const float spacing = radius * 0.5f;
	const sf::Vector2f delta = to - from;
	const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
	const int steps = std::max(1, static_cast<int>(distance / spacing));

	// Debug: show how far the mouse moved between frames and how many
	// stamps were needed to cover the gap.
	std::cout << "stroke from (" << from.x << ", " << from.y << ") to ("
		<< to.x << ", " << to.y << ") distance=" << distance
		<< " steps=" << steps << '\n';

	// Draw equally spaced circles from the previous frame's position to
	// the current one; this produces a continuous-looking line.
	for (int i = 0; i <= steps; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(steps);
		const sf::Vector2f p = from + delta * t;

		sf::CircleShape brushCircle(radius);
		brushCircle.setFillColor(color);
		brushCircle.setPosition({ p.x - radius, p.y - radius });
		canvas.draw(brushCircle);
		++stampCount;
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode({1280, 720}), "Simulation Render");
	window.setFramerateLimit(144);

	sf::Clock clock;
	float accumulator = 0.0f;
	const float dt = 1.0f / 60.0f; // Fixed time step of 60 FPS

	sf::RenderTexture canvas({1280, 720});
	canvas.clear(sf::Color::Black);
	canvas.display();

	bool toChangeColor = false;
	bool isPainting = false;
	sf::Vector2f previousMousePosition{ 0.0f, 0.0f };
	int frameStampCount = 0;

	sf::CircleShape redCircle(20);
	redCircle.setPosition({ 100, 100 });
	redCircle.setFillColor(sf::Color::Red);
	canvas.draw(redCircle);
	canvas.display();

	sf::Color paintColor = sf::Color::White;

	while (window.isOpen()) {
		frameStampCount = 0;

		// Process window events
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}

		// Accumulate dt
		float frameTime = clock.restart().asSeconds();
		if (frameTime > 0.25f) {
			frameTime = 0.25f; // Avoid spiral of death
		}
		accumulator += frameTime;

		// Fixed Step Physics update
		while (accumulator >= dt) {
			// Update your simulation here with a fixed time step of dt
			// e.g., updatePhysics(dt);
			accumulator -= dt;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

			// First frame the button is held; record the starting point so
			// the next frame can interpolate from here.
			if (!isPainting) {
				isPainting = true;
				previousMousePosition = mousePosition;
				std::cout << "painting started at (" << mousePosition.x << ", " << mousePosition.y << ")\n";
			}

			// Pick a new color when clicking inside the palette circle.
			if (redCircle.getGlobalBounds().contains(mousePosition)) {
				toChangeColor = true;
				paintColor = redCircle.getFillColor();
				std::cout << "picked color from red circle\n";
			}
			// Otherwise draw an interpolated stroke from the last known
			// position. previousMousePosition is updated to the current
			// position after stamping so the next frame continues from here.
			else if (!toChangeColor) {
				StampBrushStroke(canvas, previousMousePosition, mousePosition, paintColor, 10.0f, frameStampCount);
				previousMousePosition = mousePosition;
			}

			toChangeColor = false;
		}
		else {
			// Button released: reset state so the next click starts fresh.
			if (isPainting) {
				std::cout << "painting ended\n";
			}
			isPainting = false;
		}

		// Debug: total number of brush circles drawn this frame.
		std::cout << "frame stamps=" << frameStampCount << '\n';

		canvas.display();

		// Render
		window.clear();
		window.draw(sf::Sprite(canvas.getTexture()));
		window.display();

	}
	return 0;
}
