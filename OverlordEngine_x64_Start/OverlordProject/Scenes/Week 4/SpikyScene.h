#pragma once
class SpikyMaterial;

class SpikyScene final: public GameScene
{
public:
	SpikyScene() :GameScene(L"SpikyScene") {}
	~SpikyScene() override = default;

	SpikyScene(const SpikyScene& other) = delete;
	SpikyScene(SpikyScene&& other) noexcept = delete;
	SpikyScene& operator=(const SpikyScene& other) = delete;
	SpikyScene& operator=(SpikyScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;

private:
	GameObject* m_pObject{};
	SpikyMaterial* m_pMaterial{};

	XMFLOAT4 m_Color{ DirectX::Colors::Red };
	float m_SpikeLength{ 0.5f };
	float m_Angle{};
};