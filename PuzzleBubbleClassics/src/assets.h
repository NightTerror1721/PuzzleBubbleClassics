#pragma once

#include <random>

#include "common.h"
#include "game_basics.h"
#include "utils/SimpleElementManager.h"

class TextureManager : public SimpleElementManager<sf::Texture>
{
public:
	TextureManager(const TextureManager&) = delete;
	TextureManager(TextureManager&&) noexcept = default;
	~TextureManager() = default;

	TextureManager& operator= (const TextureManager&) = delete;
	TextureManager& operator= (TextureManager&&) = default;

public:
	TextureManager();
	TextureManager(TextureManager& parent);

	bool load(const Path& filepath, const String& tag);
	bool load(const Path& filepath, const String& tag, const sf::IntRect& dims);
	bool load(const Path& filepath, const String& tag, int x, int y, int width, int height);

	inline bool load(const String& filename, const String& tag) { return load(Path(filename), tag); }
	inline bool load(const String& filename, const String& tag, const sf::IntRect& dims) { return load(Path(filename), tag, dims); }
	inline bool load(const String& filename, const String& tag, int x, int y, int width, int height)
	{
		return load(Path(filename), tag, x, y, width, height);
	}

private:
	static TextureManager _Root;

	explicit TextureManager(int);

public:
	static inline TextureManager& root() { return _Root; }
};





class AnimatedSprite : public sf::Sprite, public Renderable, public Updatable
{
public:
	enum class Mode { Static, Sequence, Loop, Random };

private:
	UInt32 _x;
	UInt32 _y;
	UInt32 _w;
	UInt32 _h;
	UInt32 _frames;

	Mode _mode;

	float _min;
	float _max;
	float _current;

	std::minstd_rand _rand;

	float _it;
	float _oldIt;
	float _speed;

	bool _end;

public:
	AnimatedSprite();
	virtual ~AnimatedSprite();

	void setFrameDimensions(UInt32 x, UInt32 y, UInt32 w, UInt32 h);
	void setFrameCount(UInt32 frames_count);

	void setStaticMode();
	void setSequenceMode();
	void setLoopMode();
	void setRandomMode(float min, float max);

	bool isStaticMode() const;
	bool isSequenceMode() const;
	bool isLoopMode() const;
	bool isRandomMode() const;

	bool hasEnded() const;

	void start();
	void stop();

	void setCurrentFrame(UInt32 frame);
	void setExactCurrentFrame(float frame);

	UInt32 getCurrentFrame() const;
	float getExactCurrentFrame() const;

	void rewind();
	void fastForward();

	UInt32 getFrameX() const;
	UInt32 getFrameY() const;
	UInt32 getFrameWidth() const;
	UInt32 getFrameHeight() const;
	UInt32 getFrameCount() const;

	void update(const sf::Time& delta) override;
	void render(sf::RenderTarget& canvas) override;

private:
	int state() const;
	void generateCurrent();
	void updateIterator();
};
