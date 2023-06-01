#pragma once
class PlayerJoinIcon;

class JoinMenuScene final : public GameScene
{
public:
	JoinMenuScene();
	~JoinMenuScene() override;

	JoinMenuScene(const JoinMenuScene& other) = delete;
	JoinMenuScene(JoinMenuScene&& other) noexcept = delete;
	JoinMenuScene& operator=(const JoinMenuScene& other) = delete;
	JoinMenuScene& operator=(JoinMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

	virtual void OnSceneActivated() override;
	virtual void OnSceneDeactivated() override;

private:
	void AddPlayer(int gamepadIndex);

	enum InputIds
	{
		Join, Start, Next, Previous
	};
	const int m_NumberInputIds{ 4 };

	FixedCamera* m_pFixedCamera{};
	std::vector<std::wstring> m_PlayerTextureNames{};
	std::vector<std::wstring> m_PlayerUITextureNames{};
	std::vector<bool> m_Joined{ false,false,false,false };
	std::vector<PlayerJoinIcon*> m_pIcons{};
	std::vector<std::string> m_PlayerTexts{};
	std::vector<std::string> m_Texts{};

	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };
	int m_NrPlayers{};

	FMOD::Sound* m_pMusicSound{};
	FMOD::Sound* m_pSelectSound{};
	FMOD::Sound* m_pClickSound{};
	FMOD::Sound* m_pJoinSound{};

	FMOD::Channel* m_pChannelBackground2D{ nullptr };
	FMOD::Channel* m_pChannelEffects2D{ nullptr };
};


