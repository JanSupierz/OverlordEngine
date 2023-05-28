#pragma once
#include <unordered_map>

class Character;
class Grid;
class BaseMaterial;

class Bomb final: public GameObject
{
public:
	Bomb(int col, int row, Character* pOwner, Grid* pGrid);
	virtual ~Bomb();

	Bomb(const Bomb& other) = delete;
	Bomb(Bomb&& other) noexcept = delete;
	Bomb& operator=(const Bomb& other) = delete;
	Bomb& operator=(Bomb&& other) noexcept = delete;

	virtual void Initialize(const SceneContext& ) override;
	static void SetBombMaterials(BaseMaterial* pBombMaterial, PxMaterial* pStaticMaterial);

	virtual void Update(const SceneContext& sceneContext) override;
	static bool CheckExplosion();

	void Explode();
private:
	enum class PlayerCollision
	{
		disabled, enabled, shouldDisable, shouldEnable
	};

	PlayerCollision m_PlayerCollision{ PlayerCollision::disabled };

	Character* m_pOwner;
	Grid* m_pGrid;

	float m_LifeTime;
	const int m_MaxRange;

	const int m_StartRow, m_StartCol;
	bool m_AlreadyExploded;

	int m_PlayersInside{ 0 };

	bool m_ShouldRemoveCollision{ false };
	bool m_ShouldAddCollision{ false };
	RigidBodyComponent* m_pRigid{};

	static BaseMaterial* s_pBombMaterial;
	static PxMaterial* s_pStaticMaterial;

	static std::unordered_map<int, int> s_Characters;
	static bool s_BombExploded;
};

