#pragma once
class Character;
class ColorMaterial_Shadow_Skinned;

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

	void InitPlayer(int index, 
		ColorMaterial_Shadow_Skinned* pCharacterMaterial, 
		ColorMaterial_Shadow_Skinned* pBodyMaterial, 
		ColorMaterial_Shadow_Skinned* pDetailMaterial, 
		ColorMaterial_Shadow_Skinned* pHandsMaterial,
		ColorMaterial_Shadow_Skinned* pSkinMaterial,
		ColorMaterial_Shadow_Skinned* pGoldMaterial,
		PxMaterial* pDefaultMaterial);

	int ToInputId(int index, int basicInputId) const;

	FixedCamera* m_pFixedCamera{};

	const float m_MaxCameraHeight{ 200.f };

	ModelAnimator* pAnimator{};

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


