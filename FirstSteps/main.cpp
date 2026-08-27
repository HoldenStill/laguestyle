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

void InstantiatePaletteCircles(int numCircles, float radius, float spacing, sf::RenderTexture& canvas, std::vector<sf::CircleShape>& paletteCircles);

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

	std::vector<sf::CircleShape> paletteCircles;
	InstantiatePaletteCircles(5, 20.0f, 30.0f, canvas, paletteCircles);

	/*
	sf::CircleShape redCircle(20);
	redCircle.setPosition({ 100, 100 });
	redCircle.setFillColor(sf::Color::Red);
	canvas.draw(redCircle);
	canvas.display();
	*/

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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Backspace)) {
			paintColor = sf::Color::White;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
			paletteCircles.clear();
			canvas.clear(sf::Color::Black);
			InstantiatePaletteCircles(5, 20.0f, 30.0f, canvas, paletteCircles);
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
			for (const auto& paletteCircle : paletteCircles) {
				if (paletteCircle.getGlobalBounds().contains(mousePosition)) {
					isPainting = false; // Stop painting when picking a color
					toChangeColor = true;
					paintColor = paletteCircle.getFillColor();
					std::cout << "picked color from palette circle\n";
					break; // Exit the loop once a color is picked
				}
				// Otherwise draw an interpolated stroke from the last known
				// position. previousMousePosition is updated to the current
				// position after stamping so the next frame continues from here.
				else if (!toChangeColor) {
					StampBrushStroke(canvas, previousMousePosition, mousePosition, paintColor, 10.0f, frameStampCount);
					previousMousePosition = mousePosition;
				}
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




void InstantiatePaletteCircles(int numCircles, float radius, float spacing, sf::RenderTexture& canvas, std::vector<sf::CircleShape>& paletteCircles) {
	// Implementation for creating palette circles
	// Going to attempt adding a certain number of color selection circles, and each having a random color
	// add for loop for creating circles, figure out how to do random color.
	
	for (int i = 0; i < numCircles; ++i) {
		sf::CircleShape paletteCircle(radius);
		paletteCircle.setPosition({ static_cast<float>(50 + i * (radius * 2 + spacing)), 50.0f });
		paletteCircle.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
		paletteCircles.push_back(paletteCircle);
		canvas.draw(paletteCircle);

	}
}