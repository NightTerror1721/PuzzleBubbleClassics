#include "game_controller.h"

#include "globals.h"

GameController globals::game{ "!Kram Tetris" };

GameController::GameController(const std::string& name) :
	GameObjectContainer{},
	_close{ true },
	_window{},
	_deltaClock{},
	_phClock{},
	_phTimeCurrent{},
	_phTimeUp{ sf::seconds(1.f / max_physics_fps) },
	_name{ name },
	_vmode{ 640, 480 },
	_wstyle{ WindowStyle::Default },
	_virtualCanvas{},
	_virtualWindow{},
	_view{},
	_fps{}
{
	_virtualCanvas.create(canvas_width, canvas_height);

	_virtualWindow.setSize({ static_cast<float>(canvas_width), static_cast<float>(canvas_height) });
	_virtualWindow.setPosition(0, 0);
	_virtualWindow.setTexture(&_virtualCanvas.getTexture());

	_view.setSize({ static_cast<float>(canvas_width), static_cast<float>(canvas_height) });
	_view.setCenter({ static_cast<float>(canvas_width / 2), static_cast<float>(canvas_height / 2) });
}
GameController::~GameController() {}

void GameController::start()
{
	if (_close)
	{
		_close = false;
		init();
		loop();
	}
}

void GameController::close()
{
	if (!_close)
	{
		_close = true;
		_window.close();
	}
}

void GameController::videoMode(sf::VideoMode mode, bool apply)
{
	_vmode = mode;
	if (apply)
		resetWindow();
}
void GameController::style(WindowStyle style, bool apply)
{
	_wstyle = style;
	if (apply)
		resetWindow();
}
void GameController::resetWindow()
{
	if (_close)
		return;

	if (_window.isOpen())
		close();
	_window.create(_vmode, _name.c_str(), static_cast<UInt32>(_wstyle));
	//_window.setVerticalSyncEnabled(true);
	_window.setFramerateLimit(0);
	_window.setActive(true);
}

bool GameController::fullscreen() const { return (_wstyle & WindowStyle::Fullscreen) == WindowStyle::None; }

void GameController::loop()
{
	while (!_close)
	{
		processEvents();
		update();
		render();
	}
}

void GameController::init()
{
	_fps.init();
	_fps.enabled(true);
	resetWindow();
}

void GameController::update()
{
	static sf::Time currentTime = sf::Time::Zero;
	static unsigned int currentFps = 0;

	if (!_close)
	{
		_phTimeCurrent += _phClock.restart();

		if (_phTimeCurrent >= _phTimeUp)
		{
			sf::Time delta = _deltaClock.restart();

			currentTime += delta;
			currentFps += 1;
			if (currentTime >= sf::seconds(1))
			{
				currentTime = sf::Time::Zero;
				unsigned int fps = currentFps;
				currentFps = 0;
				std::cout << "Physics fps: " << fps << std::endl;
			}

			Int64 temp = static_cast<Int64>(_phTimeCurrent / _phTimeUp);
			_phTimeCurrent -= _phTimeUp * temp;
			GameObjectContainer::update(delta);
		}

		_fps.update();
	}
}

void GameController::render()
{
	if (!_close)
	{
		_window.clear();
		_virtualCanvas.clear();

		GameObjectContainer::render(_virtualCanvas);

		_virtualCanvas.display();

		_window.setView(_view);
		_window.draw(_virtualWindow);
		_window.setView(_window.getDefaultView());

		_fps.render(_window);

		_window.display();
	}
}

void GameController::processEvents()
{
	if (!_close)
	{
		sf::Event event;
		while (_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				_close = true;
				return;
			}
			GameObjectContainer::dispatchEvent(event);
		}
	}
}

void GameController::onCreate(GameObject& obj)
{
}



void FPSMonitor::init()
{
	_clock.restart();

	_text.setFillColor(sf::Color::Green);
	_text.setCharacterSize(24);

	_text.setFont(globals::fonts["arial"]);

	_text.setPosition(10, 10);

	_text.setString("0 fps");
}

void FPSMonitor::update()
{
	sf::Time delta = _clock.restart();

	_remaining -= static_cast<Int64>(delta.asMicroseconds());
	_current++;

	while (_remaining < 0)
	{
		_remaining += identity;
		_last = _current;
		_current = 0;

		_text.setString(std::to_string(_last) + " fps");
	}
}
void FPSMonitor::render(sf::RenderTarget& canvas)
{
	if (_enabled)
	{
		canvas.draw(_text);
	}
}