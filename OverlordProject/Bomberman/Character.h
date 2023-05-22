#pragma once

class ModelAnimator;
class Grid;

struct CharacterDesc
{
	CharacterDesc(
		PxMaterial* pMaterial,		
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_PlaceBomb{ -1 };
	GamepadIndex gamepadIndex{ 0 };

	UINT currentClipId{ 0 };
	UINT clipId_Walking{ 0 };
	UINT clipId_Idle{ 0 };
	UINT clipId_Floating{ 0 };
	UINT clipId_Death{ 0 };
	UINT clipId_PlaceBomb{ 0 };
};

enum class CharacterAction
{
	running, standing, placingBomb, stopPlacing
};

class Character : public GameObject
{
public:
	Character(const CharacterDesc& characterDesc, Grid* pGrid);
	~Character() override = default;

	Character(const Character& other) = delete;
	Character(Character&& other) noexcept = delete;
	Character& operator=(const Character& other) = delete;
	Character& operator=(Character&& other) noexcept = delete;

	void DrawImGui();
	void SetAnimator(ModelAnimator* pAnimator);

	int GetIndex() const;

	void AddScore();
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;
	void UpdateAnimations(const SceneContext& sceneContext);

private:
	void PlaceBomb() const;

	ControllerComponent* m_pControllerComponent{};

	CharacterDesc m_CharacterDesc;
	float m_TotalYaw{};								//Total camera Pitch(X) and Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)
	float m_AnimationTimeLeft{};

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)
	ModelAnimator* m_pAnimator{ nullptr };
	CharacterAction m_CurrentAction{ CharacterAction::standing };
	Grid* m_pGrid;
	int m_Score;
	bool m_CanPlaceBomb;
};

