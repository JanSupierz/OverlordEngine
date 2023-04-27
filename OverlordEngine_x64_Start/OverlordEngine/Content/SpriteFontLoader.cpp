#include "stdafx.h"
#include "SpriteFontLoader.h"

SpriteFont* SpriteFontLoader::LoadContent(const ContentLoadInfo& loadInfo)
{
	const auto pReader = new BinaryReader();
	pReader->Open(loadInfo.assetFullPath);

	if (!pReader->Exists())
	{
		Logger::LogError(L"Failed to read the assetFile!\nPath: \'{}\'", loadInfo.assetSubPath);
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	const char bytes[]{ pReader->Read<char>(), pReader->Read<char>(), pReader->Read<char>() };

	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	
	if (bytes[0] != 'B' || bytes[1] != 'M' || bytes[2] != 'F')
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid.fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	const char version{ pReader->Read<char>() };

	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr

	if (version != 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file >> Start Parsing!
	//use this SpriteFontDesc to store all relevant information (used to initialize a SpriteFont object)
	SpriteFontDesc fontDesc{};

	//**********
	// BLOCK 0 *
	//**********
	//Skip the blockId and blockSize
	pReader->MoveBufferPosition(7);

	//Retrieve the FontSize [fontDesc.fontSize]
	fontDesc.fontSize = pReader->Read<short>();

	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pReader->MoveBufferPosition(12);

	//Retrieve the FontName [fontDesc.fontName]
	fontDesc.fontName = pReader->ReadNullString();

	//**********
	// BLOCK 1 *
	//**********
	//Skip the blockId and blockSize
	pReader->MoveBufferPosition(9);

	//Retrieve Texture Width & Height [fontDesc.textureWidth/textureHeight]
	fontDesc.textureWidth = pReader->Read<short>();
	fontDesc.textureHeight = pReader->Read<short>();

	//Retrieve PageCount
	const short nrPages{ pReader->Read<short>() };

	//> if pagecount > 1
	//	> Log Error (Only one texture per font is allowed!)
	if (nrPages > 1)
	{
		Logger::LogError(L"Only one texture per font is allowed!");
		return nullptr;
	}

	//Advance to Block2 (Move Reader)
	pReader->MoveBufferPosition(5);

	//**********
	// BLOCK 2 *
	//**********
	//Skip the blockId and blockSize
	pReader->MoveBufferPosition(5);

	//Retrieve the PageName (BinaryReader::ReadNullString)
	const std::wstring pageName{ pReader->ReadNullString() };

	//Construct the full path to the page texture file
	//	>> page texture should be stored next to the .fnt file, pageName contains the name of the texture file
	//	>> full texture path = asset parent_path of .fnt file (see loadInfo.assetFullPath > get parent_path) + pageName (filesystem::path::append)
	//	>> Load the texture (ContentManager::Load<TextureData>) & Store [fontDesc.pTexture]
	std::filesystem::path texturePath{ loadInfo.assetFullPath.parent_path().append(pageName) };
	fontDesc.pTexture = ContentManager::Load<TextureData>(texturePath);

	//**********
	// BLOCK 3 *
	//**********

	//Convert BRGA to RGBA
	std::unordered_map<char, unsigned char> channels
	{
		{ 1, 2 }, { 2, 1 }, { 4, 0 }, { 8, 3 }, { 16, 4 }
	};
	
	//Skip the blockId
	pReader->MoveBufferPosition(1);

	//Retrieve blockSize
	const int blockSize{ pReader->Read<int>() };

	//Retrieve Character Count (see documentation)
	const int nrCharacters{ blockSize / 20 };

	//Create loop for Character Count, and:
	for (int index{}; index < nrCharacters; ++index)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		const wchar_t id{ static_cast<wchar_t>(pReader->Read<int>()) };

		//> Create instance of FontMetric (struct)
		FontMetric metric{};

		//	> Set Character (CharacterId) [FontMetric::character]
		metric.character = id;

		//	> Retrieve Xposition (store Local)
		const short xPos{ pReader->Read<short>() };

		//	> Retrieve Yposition (store Local)
		const short yPos{ pReader->Read<short>() };

		//	> Retrieve & Set Width [FontMetric::width]
		metric.width = pReader->Read<short>();

		//	> Retrieve & Set Height [FontMetric::height]
		metric.height = pReader->Read<short>();

		//	> Retrieve & Set OffsetX [FontMetric::offsetX]
		metric.offsetX = pReader->Read<short>();

		//	> Retrieve & Set OffsetY [FontMetric::offsetY]
		metric.offsetY = pReader->Read<short>();

		//	> Retrieve & Set AdvanceX [FontMetric::advanceX]
		metric.advanceX = pReader->Read<short>();

		//	> Retrieve & Set Page [FontMetric::page]
		metric.page = pReader->Read<char>();

		//	> Retrieve Channel (BITFIELD!!!) 
		const char channel{ pReader->Read<char>() };

		//	> See documentation for BitField meaning [FontMetrix::channel]
		metric.channel = channels[channel];

		//	> Calculate Texture Coordinates using Xposition, Yposition, fontDesc.TextureWidth & fontDesc.TextureHeight [FontMetric::texCoord]
		metric.texCoord.x = xPos / static_cast<float>(fontDesc.textureWidth);
		metric.texCoord.y = yPos / static_cast<float>(fontDesc.textureHeight);

		//> Insert new FontMetric to the metrics [font.metrics] map
		//	> key = (wchar_t) charId
		//	> value = new FontMetric
		fontDesc.metrics[id] = metric;

		//(loop restarts till all metrics are parsed)
	}

	//Done!
	delete pReader;
	return new SpriteFont(fontDesc);
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
