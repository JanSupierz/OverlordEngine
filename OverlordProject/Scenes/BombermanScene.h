#pragma once
class Character;
class ColorMaterial_Shadow_Skinned;

struct PlayerDesc
{
	PlayerDesc(int playerIndex, PxMaterial* pPxCharacterMaterial)
		:pPxMaterial{ pPxCharacterMaterial }, index{ playerIndex }
	{
	}

	ColorMaterial_Shadow_Skinned* pMainMaterial{};
	ColorMaterial_Shadow_Skinned* pShirtMaterial{};
	ColorMaterial_Shadow_Skinned* pBlackMaterial{};
	ColorMaterial_Shadow_Skinned* pGlovesMaterial{};
	ColorMaterial_Shadow_Skinned* pSkinMaterial{};
	ColorMaterial_Shadow_Skinned* pGoldMaterial{};
	PxMaterial* pPxMaterial;

	float spriteOffsetX{}, spriteOffsetY{};
	float spritePivotX{ 0.f }, spritePivotY{ 0.f };
	int index{ 0 };

	std::wstring spriteName{ L"Icon_White"};

	int clipId_Death{ 1 };
	int clipId_PlaceBomb{ 2 };
	int clipId_Floating{ 3 };
	int clipId_Idle{ 4 };
	int clipId_Walking{ 6 };
};

class BombermanScene final : public GameScene
{
public:
	BombermanScene();
	~BombermanScene() override;

	BombermanScene(const BombermanScene& other) = delete;
	BombermanScene(BombermanScene&& other) noexcept = delete;
	BombermanScene& operator=(const BombermanScene& other) = delete;
	BombermanScene& operator=(BombermanScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	void InitArena();

	void InitPlayer(const PlayerDesc& playerDesc);

	int ToInputId(int index, int basicInputId) const;

	FixedCamera* m_pFixedCamera{};

	const float m_MaxCameraHeight{ 200.f };

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };

	char** m_ClipNames{};
	UINT m_ClipCount{};

	const int m_NrPlayerInputs{5};

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb,
	};

	std::vector<Character*> m_pCharacters{};
};


