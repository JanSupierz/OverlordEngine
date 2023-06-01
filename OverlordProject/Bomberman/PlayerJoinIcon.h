#pragma once

class PlayerJoinIcon final : public GameObject
{
public:
	PlayerJoinIcon(const std::wstring& spritePath, SpriteFont* pFont);
	virtual ~PlayerJoinIcon();

	PlayerJoinIcon(const PlayerJoinIcon& other) = delete;
	PlayerJoinIcon(PlayerJoinIcon&& other) noexcept = delete;
	PlayerJoinIcon& operator=(const PlayerJoinIcon& other) = delete;
	PlayerJoinIcon& operator=(PlayerJoinIcon&& other) noexcept = delete;

	virtual void Initialize(const SceneContext&) override;
	virtual void Update(const SceneContext& sceneContext) override;
	virtual void Draw(const SceneContext&) override;
	void InitPosition();

	bool IsFree() const { return m_PlayerIndex == -1; }
	void SetPlayer(int index, const std::string& text = "");
	int GetPlayerIndex() const { return m_PlayerIndex; }
	PlayerJoinIcon* GetNext() const;
	PlayerJoinIcon* GetPrevious() const;
	void SetNext(PlayerJoinIcon* pNext);
	void SetPrevious(PlayerJoinIcon* pPrevious);

private:
	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 0.f,0.f,0.f,1.f };
	XMFLOAT2 m_TextPosition{ -50.f, 30.f };
	std::string m_Text{};
	int m_PlayerIndex{ -1 };
	PlayerJoinIcon* m_pNext{nullptr};
	PlayerJoinIcon* m_pPrevious{nullptr};
};
