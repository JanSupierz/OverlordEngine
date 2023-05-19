#include "stdafx.h"
#include "Fire.h"
#include "Grid.h"
#include "BombermanScene.h"

BaseMaterial* Fire::s_pFireMaterial{ nullptr };
PxMaterial* Fire::s_pStaticMaterial{ nullptr };

Fire::Fire(int col, int row, const Character* const pOwner, Grid* pGrid)
	:m_LifeTime{ 0.5f }, m_pOwner{ pOwner }, m_pGrid{ pGrid }, m_pNode{ m_pGrid->GetNode(col, row) }
{
	m_pNode->SetBlocked(false);
}

Fire::~Fire()
{
}

void Fire::Initialize(const SceneContext&)
{
	const XMFLOAT2 nodePos{ m_pNode->GetWorldPos() };

	const float cubeDimensions{ m_pGrid->GetCellSize() };
	const float halfCube{ cubeDimensions * 0.5f };

	//Translate to the node position
	const auto transform{ GetTransform() };

	transform->Translate(nodePos.x, cubeDimensions, nodePos.y);
	transform->Scale(cubeDimensions);

	//Model
	auto pModel{ AddComponent(new ModelComponent(L"Meshes/Bomberman/SideCubes.ovm")) };
	pModel->SetMaterial(s_pFireMaterial);

	//Rigid body
	auto pRigid{ AddComponent(new RigidBodyComponent(true)) };
	
	const auto geo{ PxBoxGeometry{ halfCube,halfCube,halfCube } };
	pRigid->AddCollider(geo, *s_pStaticMaterial, true, PxTransform{ 0.f,halfCube,0.f });
	
	auto triggerCallBack = [=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		if (action == PxTriggerAction::ENTER)
		{
			BombermanScene::RemoveGameObject(pOther);
		}
	};
	
	SetOnTriggerCallBack(triggerCallBack);
}

void Fire::SetFireMaterials(BaseMaterial* pFireMaterial, PxMaterial* pStaticMaterial)
{
	s_pFireMaterial = pFireMaterial;
	s_pStaticMaterial = pStaticMaterial;
}

void Fire::Update(const SceneContext& sceneContext)
{
	m_LifeTime -= sceneContext.pGameTime->GetElapsed();

	if (m_LifeTime < 0.f)
	{
		BombermanScene::RemoveGameObject(this);
	}
}
