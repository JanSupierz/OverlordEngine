#pragma once
class Character;
class ColorMaterial_Shadow_Skinned;
class DiffuseMaterial_Shadow;
class Grid;
class PostChromaticAberration;

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

	std::string displayedText{};

	int index{ 0 };

	std::wstring spriteName{};

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

	static void AddGameObject(GameObject* pGameObject);
	static void RemoveGameObject(GameObject* pGameObject);

	static BombermanScene* GetCurrent();

	void InitPlayer(const PlayerDesc& playerDesc);
protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

	virtual void OnSceneActivated() override;
	virtual void OnSceneDeactivated() override;

private:
	void UpdateCamera();
	void InitArena();
	void CreateCube(bool isDestructible, int col, int row, int height, const std::wstring& meshFilePath, BaseMaterial* pColorMaterial, PxMaterial* pStaticMaterial, float heightOffset = 0.f, float scale = 1.f, bool disableRigid = false);

	int ToInputId(int index, int basicInputId) const;

	FixedCamera* m_pFixedCamera{};
	CameraComponent* m_pCameraComp{};

	const float m_MaxCameraHeight{ 150.f };

	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };
	int m_NrPlayers{};

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

	const float m_CubeSize;
	std::vector<Character*> m_pCharacters{};
	std::unique_ptr<Grid> m_pGrid;

	PostChromaticAberration* m_pChromatic;
	const float m_MaxScreenShakeDuration{ 0.5f };
	float m_ScreenShakeTimer{};

	std::string m_EndText{ "Draw!" };
	XMFLOAT2 m_EndTextPosition{};

	//Sounds
	FMOD::Channel* m_pChannel2D{ nullptr };
	FMOD::Channel* m_pChannel3D{ nullptr };
	FMOD_VECTOR m_PrevCamPos{};
	FMOD::Sound* m_pBombSound3D{ nullptr };

	//UI
	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };
	std::string m_TimerText{ "2:00" };
	XMFLOAT2 m_TimerTextPosition{};
	float m_TimeLeft{ 0.5f };
	int m_NrMinutes{ 3 };
	int m_NrSeconds{ 0 };
	bool m_GameEnded{false};

	//Statics
	static std::vector<GameObject*> s_pObjectsToAdd;
	static std::vector<GameObject*> s_pObjectsToRemove;
	static bool s_CheckVectors;
	static BombermanScene* s_CurrentScene;

	XMFLOAT3 m_DefaultCameraOffset;
	XMFLOAT3 m_LastCameraPosition{};
	float m_LastFov{ XM_PIDIV4 };
};


