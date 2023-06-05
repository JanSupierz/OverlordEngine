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
	void UpdateGameEnd();
	void UpdateCamera();
	void InitArena();
	void CreateCube(bool isDestructible, int col, int row, int height, const std::wstring& meshFilePath, BaseMaterial* pColorMaterial, PxMaterial* pStaticMaterial, float heightOffset = 0.f, float scale = 1.f, bool disableRigid = false);

	int ToInputId(int index, int basicInputId) const;

	const float m_MaxCameraHeight{ 150.f };
	int m_MinNrAlive{ 0 };
	int m_AnimationClipId{ 0 };
	float m_AnimationSpeed{ 1.f };
	int m_NrPlayers{};

	char** m_ClipNames{};
	UINT m_ClipCount{};

	const int m_NrPlayerInputs{ 7 };

	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterPlaceBomb,
		CharacterDetonate,
		Pause
	};

	const float m_CubeSize;
	std::vector<Character*> m_pCharacters{};
	std::unique_ptr<Grid> m_pGrid;

	PostChromaticAberration* m_pChromatic;
	const float m_MaxScreenShakeDuration{ 0.5f };
	float m_ScreenShakeTimer{};

	//Sounds
	FMOD::Channel* m_pChannel2D{ nullptr };
	FMOD::Channel* m_pChannel2DClock{ nullptr };
	FMOD::Channel* m_pChannel3D{ nullptr };

	FMOD_VECTOR m_PrevCamPos{};

	FMOD::Sound* m_pBombSound3D{ nullptr };
	FMOD::Sound* m_pPickUpSound{ nullptr };
	FMOD::Sound* m_pClockSound{ nullptr };
	bool m_ClockSoundActivated{ false };

	//UI
	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };

	XMFLOAT4 m_TimerTextColor{ 1.f,1.f,1.f,1.f };
	std::string m_TimerText{};
	XMFLOAT2 m_TimerTextPosition{};

	float m_TimeLeft{ 180.f };
	int m_NrMinutes{};
	int m_NrSeconds{};
	bool m_GameEnded{ false };

	std::string m_EndText{ "Draw!" };
	XMFLOAT2 m_EndTextPosition{};

	float m_WaitTime{ 5.f };

	//Statics
	static std::vector<GameObject*> s_pObjectsToAdd;
	static std::vector<GameObject*> s_pObjectsToRemove;
	static bool s_CheckVectors;
	static BombermanScene* s_CurrentScene;

	//Camera
	XMFLOAT3 m_DefaultCameraOffset;
	XMFLOAT3 m_LastCameraPosition{};
	float m_LastFov{ XM_PIDIV4 };

	FixedCamera* m_pFixedCamera{};
	CameraComponent* m_pCameraComp{};
};


