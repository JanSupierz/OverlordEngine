#include "stdafx.h"
#include "Fire.h"
#include "Grid.h"
#include "Scenes/BombermanScene.h"
#include "Character.h"
#include "Bomb.h"
#include "Cube.h"

BaseMaterial* Fire::s_pFireMaterial{ nullptr };
PxMaterial* Fire::s_pStaticMaterial{ nullptr };

Fire::Fire(int col, int row, Character* pOwner, Grid* pGrid)
	:m_LifeTime{ 0.5f }, m_pOwner{ pOwner }, m_pGrid{ pGrid }, m_pNode{ m_pGrid->GetNode(col, row) }
{
	SetTag(L"Fire");
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
	
	const float geoSize{ 0.8f * halfCube };
	const auto geo{ PxBoxGeometry{ geoSize ,geoSize,geoSize } };
	pRigid->AddCollider(geo, *s_pStaticMaterial, true, PxTransform{ 0.f,halfCube,0.f });
	
	auto triggerCallBack = [=](GameObject*, GameObject* pOther, PxTriggerAction action)
	{
		if (action == PxTriggerAction::ENTER)
		{
			std::wstring tag{ pOther->GetTag() };

			if (tag == L"Cube")
			{
				Cube* pCube = static_cast<Cube*>(pOther);
				pCube->DestroyCube();
			}
			else if (tag == L"Player")
			{
				//Other player gains a point
				Character* pCharacter = static_cast<Character*>(pOther);

				if (m_pOwner != pCharacter)
				{
					m_pOwner->AddScore();
				}

				pCharacter->SetIsActive(false);
			}
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
