#pragma once
class PlayerJoinIcon;

class MainMenuScene final : public GameScene
{
public:
	MainMenuScene();
	~MainMenuScene() override;

	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) noexcept = delete;
	MainMenuScene& operator=(const MainMenuScene& other) = delete;
	MainMenuScene& operator=(MainMenuScene&& other) noexcept = delete;

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
		Join,Start,Next,Previous
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

	FMOD::Channel* m_pChannel2D{ nullptr };
	FMOD::Channel* m_pChannel3D{ nullptr };
};


