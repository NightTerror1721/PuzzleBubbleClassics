#pragma once

#include "object_basics.h"
#include "utils/reference.h"

#include <list>
#include <memory>


class GameController;

enum class WindowStyle : Uint32
{
	None = 0,      ///< No border / title bar (this flag and all others are mutually exclusive)
	Titlebar = 1 << 0, ///< Title bar + fixed border
	Resize = 1 << 1, ///< Title bar + resizable border + maximize button
	Close = 1 << 2, ///< Title bar + close button
	Fullscreen = 1 << 3, ///< Fullscreen mode (this flag and all others are mutually exclusive)

	Default = Titlebar | Resize | Close ///< Default window style
};

constexpr WindowStyle operator& (WindowStyle left, WindowStyle right)
{
	return static_cast<WindowStyle>(static_cast<std::uint32_t>(left) & static_cast<std::uint32_t>(right));
}

constexpr WindowStyle operator| (WindowStyle left, WindowStyle right)
{
	return static_cast<WindowStyle>(static_cast<std::uint32_t>(left) | static_cast<std::uint32_t>(right));
}

constexpr WindowStyle operator~ (WindowStyle right)
{
	return static_cast<WindowStyle>(~static_cast<std::uint32_t>(right));
}

constexpr WindowStyle& operator&= (WindowStyle& left, WindowStyle right)
{
	return left = static_cast<WindowStyle>(static_cast<std::uint32_t>(left) & static_cast<std::uint32_t>(right));
}

constexpr WindowStyle& operator|= (WindowStyle& left, WindowStyle right)
{
	return left = static_cast<WindowStyle>(static_cast<std::uint32_t>(left) | static_cast<std::uint32_t>(right));
}


class FPSMonitor
{
private:
	static constexpr Int64 identity = 1000000;

	sf::Clock _clock;
	Int64 _remaining = identity;
	unsigned int _current = 0;
	unsigned int _last = 0;
	bool _enabled = false;

	sf::Text _text;

public:
	void init();

	void update();
	void render(sf::RenderTarget& canvas);

	inline bool enabled() const { return _enabled; }
	inline void enabled(bool flag) { _enabled = flag; }
};


class GameActivity : public GameObject
{
public:
	friend GameController;

private:
	bool _disposed = false;
	bool _initiated = false;

public:
	GameActivity() = default;
	GameActivity(const GameActivity&) = delete;
	GameActivity(GameActivity&&) noexcept = default;
	virtual ~GameActivity() = default;

	GameActivity& operator= (const GameActivity&) = delete;
	GameActivity& operator= (GameActivity&&) noexcept = delete;

public:
	constexpr bool isDisposed() const { return _disposed; }
	constexpr void dispose() { _disposed = true; }

	constexpr bool isInitiated() const { return _initiated; }

private:
	constexpr void markAsInitiated() { _initiated = true; }
	constexpr bool isValid() const { return _initiated && !_disposed; }

public:
	virtual constexpr void init() {}
};


class GameController
{
public:
	static constexpr int CanvasWidth = 1280;
	static constexpr int CanvasHeight = 720;

	static constexpr unsigned int MaxPhysicsFPS = 240;

private:
	static GameController Instance;

private:
	bool _close;
	sf::RenderWindow _window;

	sf::Clock _deltaClock;
	sf::Clock _phClock;
	sf::Time _phTimeCurrent;
	sf::Time _phTimeUp;

	std::string _name;
	sf::VideoMode _vmode;
	WindowStyle _wstyle;

	sf::RenderTexture _virtualCanvas;
	sf::RectangleShape _virtualWindow;
	sf::View _view;

	std::list<std::unique_ptr<GameActivity>> _activities;

	FPSMonitor _fps;

public:
	GameController(const GameController&) = delete;
	GameController(GameController&&) noexcept = delete;

	GameController& operator= (const GameController&) = delete;
	GameController& operator= (GameController&&) noexcept = delete;

private:
	GameController(const std::string& name);
	~GameController();

public:
	constexpr const std::string& getName() const { return _name; }
	constexpr const sf::VideoMode& getVideoMode() const { return _vmode; }
	constexpr WindowStyle getWindowStyle() const { return _wstyle; }
	constexpr bool isFullscreen() const { return (_wstyle & WindowStyle::Fullscreen) == WindowStyle::None; }

	inline sf::Vector2u getSize() const { return { _vmode.width, _vmode.height }; }

public:
	void start();

	void close();

	void setVideoMode(sf::VideoMode mode, bool apply = true);
	void setWindowStyle(WindowStyle style, bool apply = true);
	void resetWindow();

	void addActivity(std::unique_ptr<GameActivity>&& activity);

private:
	void loop();

	void init();
	void update();
	void render();
	void processEvents();

	void updateActivities(const sf::Time& elapsedTime);
	void renderActivities(sf::RenderTarget& canvas, sf::RenderStates rs);
	void processActivitiesEvents(const sf::Event& event);

public:
	static constexpr GameController& instance() { return Instance; }

public:
	template <std::derived_from<GameActivity> _Ty, typename... _ArgsTys>
	Reference<_Ty> createActivity(_ArgsTys&&... args)
	{
		_activities.push_back(std::make_unique<_Ty>(std::forward<_ArgsTys>(args)...));
		return reinterpret_cast<_Ty*>(_activities.back().get());
	}
};
