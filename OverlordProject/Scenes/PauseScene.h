#pragma once

class PauseScene final : public GameScene
{
public:
	PauseScene();
	~PauseScene() override;

	PauseScene(const PauseScene& other) = delete;
	PauseScene(PauseScene&& other) noexcept = delete;
	PauseScene& operator=(const PauseScene& other) = delete;
	PauseScene& operator=(PauseScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	virtual void OnSceneActivated() override;
	virtual void OnSceneDeactivated() override;

private:
	enum InputIds
	{
		Start, Next, Previous
	};

	enum class Button
	{
		resume, quit, restart
	};

	Button m_CurrentButton{ Button::resume };

	FixedCamera* m_pFixedCamera{};

	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 0.f,0.f,0.f,1.f };

	FMOD::Sound* m_pMusicSound{};
	FMOD::Sound* m_pSelectSound{};
	FMOD::Sound* m_pClickSound{};

	FMOD::Channel* m_pChannelBackground2D{ nullptr };
	FMOD::Channel* m_pChannelEffects2D{ nullptr };

	std::vector<SpriteComponent*> m_pSprites{};
	std::vector<std::string> m_ButtonTexts{};
	std::vector<XMFLOAT2> m_ButtonTextPositions{};
};


