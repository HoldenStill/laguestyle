#include <iostream>
#include <SFML/Graphics.hpp>

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

	sf::CircleShape redCircle(20);
	redCircle.setPosition({ 100, 100 });
	redCircle.setFillColor(sf::Color::Red);
	canvas.draw(redCircle);
	canvas.display();
	
	sf::Color paintColor = sf::Color::White;

	while (window.isOpen()) {
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

		// random
		

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			// Determine if the user clicked within the bounds of redCircle.
			sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

			if (redCircle.getGlobalBounds().contains(mousePosition)) {
				toChangeColor = true;
				paintColor = redCircle.getFillColor();
			}

			if (!toChangeColor) {
				sf::CircleShape brushCircle(10);
				brushCircle.setFillColor(paintColor);
				brushCircle.setPosition({ mousePosition.x - 5.0f, mousePosition.y - 5.0f });
				canvas.draw(brushCircle);
			}
			toChangeColor = false;
		}

		canvas.display();

		// Render
		window.clear();
		window.draw(sf::Sprite(canvas.getTexture()));
		window.display();

	}
	return 0;
}