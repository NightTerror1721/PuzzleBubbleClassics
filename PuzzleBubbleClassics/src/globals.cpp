#include "globals.h"

namespace globals
{
	FontManager fonts;
	TextureManager& textures = TextureManager::root();
	BubbleModelManager& bubblesManager = BubbleModelManager::instance;
}
