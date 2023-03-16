#pragma once
class W3_PongScene final : public GameScene
{
public:
	W3_PongScene();
	~W3_PongScene() override = default;

	W3_PongScene(const W3_PongScene& other) = delete;
	W3_PongScene(W3_PongScene&& other) noexcept = delete;
	W3_PongScene& operator=(const W3_PongScene& other) = delete;
	W3_PongScene& operator=(W3_PongScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void OnGUI() override;

private:
	void Reset();

	GameObject* m_pPaddleLeft{};
	GameObject* m_pPaddleRight{};

	GameObject* m_pBall{};

	RigidBodyComponent* m_pRigidBody{};
	FixedCamera* m_pFixedCamera{};

	const float m_HalfWidth{ 20.f }, m_HalfLength{ 35.f };

	bool m_IsPaused{ true };
};


