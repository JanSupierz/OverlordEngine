#pragma once
enum class PickUpType { NotBlockable, DoubleRange, PlaceTwoBombs, Highest = PlaceTwoBombs, None };

class Grid;
class Node;

class PickUp final : public GameObject
{
public:
	PickUp(Node* pNode, Grid* pGrid, PickUpType type);
	virtual ~PickUp();

	PickUp(const PickUp& other) = delete;
	PickUp(PickUp&& other) noexcept = delete;
	PickUp& operator=(const PickUp& other) = delete;
	PickUp& operator=(PickUp&& other) noexcept = delete;

	virtual void Initialize(const SceneContext&) override;

	static void SetPhysicsMaterial(PxMaterial* pStaticMaterial);
	static void SetPickUpMaterial(PickUpType type, BaseMaterial* pMaterial);
	PickUpType GetType() const;
private:
	Grid* m_pGrid;
	Node* m_pNode;
	PickUpType m_Type;
	bool m_IsActive{ true };

	static std::unordered_map<PickUpType, BaseMaterial*> s_pPickUpMaterials;
	static PxMaterial* s_pStaticMaterial;
};
