#include "JellyCore.h"

#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Graphics/ShaderManager.h>
#include <Andromeda/Graphics/Sprite.h>

JellyCore::JellyCore()
{
	_menuAtlas = 0;
	_menuFont = 0;
	_smallFont = 0;
	_titleFont = 0;
	_spriteShader = 0;
}

JellyCore::~JellyCore()
{
	delete _smallFont;
	delete _menuFont;
	delete _titleFont;

	delete _menuAtlas;
}

void JellyCore::Init()
{
	//load basic shader
	_spriteShader = ShaderManager::Instance()->LoadFromFile("font", "Assets/Shaders/font", "Assets/Shaders/font", TextureColor);

	//create atlas
	_menuAtlas = new TextureAtlas(512, 512);

	//load fonts

	_smallFont = new TexturedFont(_menuAtlas, 16, "Assets/Fonts/JellyFont.ttf");
	_smallFont->SetShader(_spriteShader);
	_smallFont->CacheGlyphs(_cache);

	_menuFont = new TexturedFont(_menuAtlas, 32, "Assets/Fonts/JellyFont.ttf");
	_menuFont->SetShader(_spriteShader);
	_menuFont->CacheGlyphs(_cache);

	_titleFont = new TexturedFont(_menuAtlas, 64, "Assets/Fonts/JellyFont.ttf");
	_titleFont->SetShader(_spriteShader);
	_titleFont->CacheGlyphs(_cache);

	//upload texture
	_menuAtlas->CreateTexture();
}