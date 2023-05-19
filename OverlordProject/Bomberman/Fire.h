#pragma once

class Character;
class Grid;
class BaseMaterial;
class Node;

class Fire final : public GameObject
{
public:
	Fire(int col, int row, const Character* const pOwner, Grid* pGrid);
	virtual ~Fire();

	Fire(const Fire& other) = delete;
	Fire(Fire&& other) noexcept = delete;
	Fire& operator=(const Fire& other) = delete;
	Fire& operator=(Fire&& other) noexcept = delete;

	virtual void Initialize(const SceneContext&) override;
	static void SetFireMaterials(BaseMaterial* pFireMaterial, PxMaterial* pStaticMaterial);

	virtual void Update(const SceneContext& sceneContext) override;

private:
	const Character* const m_pOwner;
	Grid* m_pGrid;
	Node* m_pNode;

	float m_LifeTime;

	static BaseMaterial* s_pFireMaterial;
	static PxMaterial* s_pStaticMaterial;
};

