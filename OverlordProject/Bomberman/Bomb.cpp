#include "stdafx.h"
#include "Bomb.h"
#include "Bomberman/Character.h"
#include "Bomberman/Grid.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Bomberman/Grid.h"
#include "Bomberman/Fire.h"
#include "Scenes/BombermanScene.h"

BaseMaterial* Bomb::s_pBombMaterial{ nullptr };
PxMaterial* Bomb::s_pStaticMaterial{ nullptr };
std::unordered_map<int, int> Bomb::s_Characters;
bool Bomb::s_BombExploded{ false };

Bomb::Bomb(int col, int row, const Character* const pOwner, Grid* pGrid)
	:m_pOwner{ pOwner }, m_pGrid{ pGrid }, m_LifeTime{ 3.f }, m_MaxRange{ 3 }, m_StartCol{ col }, m_StartRow{ row }, m_AlreadyExploded{ false }
{
	SetTag(L"Bomb");
}

Bomb::~Bomb()
{
	s_Characters[m_pOwner->GetIndex()] -= 1;
}

void Bomb::Initialize(const SceneContext&)
{
	const float cellSize{ m_pGrid->GetCellSize() };
	XMFLOAT2 nodePos{ m_pGrid->GetNodeWorldPos(m_StartCol, m_StartRow) };
	GetTransform()->Translate(nodePos.x, cellSize, nodePos.y);
	GetTransform()->Scale(cellSize);

	auto pModel = AddComponent(new ModelComponent(L"Meshes/Bomberman/Bomb.ovm"));
	pModel->SetMaterial(s_pBombMaterial);

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,1.f,1.f, .6f };

	const auto pVFX = AddChild(new GameObject);
	pVFX->GetTransform()->Translate(0.2f, 1.1f, 0.1f);
	pVFX->AddComponent(new ParticleEmitterComponent(L"Textures/Smoke.png", settings, 200));

	//Is kinematic because it can move when you have a power up
	auto pRigid{ AddComponent(new RigidBodyComponent(false)) };

	const auto geo{ PxSphereGeometry{cellSize * 0.5f} };
	pRigid->AddCollider(geo, *s_pStaticMaterial, false, PxTransform{ 0.f,cellSize * 0.5f,0.f });
	pRigid->SetConstraint(RigidBodyConstraint::All, false);

	s_Characters[m_pOwner->GetIndex()] += 1;
}

void Bomb::SetBombMaterials(BaseMaterial* pBombMaterial, PxMaterial* pStaticMaterial)
{
	s_pBombMaterial = pBombMaterial;
	s_pStaticMaterial = pStaticMaterial;
}

void Bomb::Update(const SceneContext& sceneContext)
{
	m_LifeTime -= sceneContext.pGameTime->GetElapsed();

	if (m_LifeTime < 0.f)
	{
		Explode();
	}
}

bool Bomb::CheckExplosion() 
{
	bool temp{ s_BombExploded };
	s_BombExploded = false;

	return temp;
}

void Bomb::Explode()
{
	s_BombExploded = true;

	if (m_AlreadyExploded) return;
	m_AlreadyExploded = true;

	const auto pos{ GetTransform()->GetWorldPosition() };
	Node* pNode{ m_pGrid->GetNode(XMFLOAT2{pos.x, pos.z}) };

	BombermanScene::AddGameObject(new Fire(pNode->GetCol(), pNode->GetRow(), m_pOwner, m_pGrid));

	for (int index{}; index < 4; ++index)
	{
		Node::Direction direction{ static_cast<Node::Direction>(index) };
		Node* pCurrentNode{ pNode };

		for (int range{}; range < m_MaxRange; ++range)
		{
			pCurrentNode = pCurrentNode->GetNeighbor(direction);

			//Check a different direction if no more neighbors in this direction
			if (!pCurrentNode || (pCurrentNode && pCurrentNode->GetCellState() == CellState::NonDestructible)) break;

			//You can't add children during the update of game objects
			BombermanScene::AddGameObject(new Fire(pCurrentNode->GetCol(), pCurrentNode->GetRow(), m_pOwner, m_pGrid));

			//Stop moving in this direction
			if (pCurrentNode->GetCellState() == CellState::Destructible)
			{
				break;
			}
		}
	}

	//You can't remove children during the update of game objects
	BombermanScene::RemoveGameObject(this);
}
