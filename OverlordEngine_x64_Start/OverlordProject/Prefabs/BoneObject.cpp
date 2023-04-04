#include "stdafx.h"
#include "BoneObject.h"

BoneObject::BoneObject(BaseMaterial* pMaterial, float length)
	:m_pMaterial(pMaterial), m_Length(length)
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
	const XMFLOAT3 position{ GetTransform()->GetPosition() };
	pBone->GetTransform()->Translate(position.x + m_Length, position.y, position.z);

	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{
	const XMMATRIX inverseWorldMatrix{ XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetTransform()->GetWorld())) };
	XMStoreFloat4x4(&m_BindPose, inverseWorldMatrix);
	
	for (BoneObject* pChild : GetChildren<BoneObject>())
	{
		pChild->CalculateBindPose();
	}
}

void BoneObject::Initialize(const SceneContext&)
{
	//Create an GameObject
	GameObject* pEmpty{ new GameObject };

	//Add pEmpty as a child to the BoneObject
	AddChild(pEmpty);

	//Add a ModelComponent to pEmpty, use Bone.ovm
	ModelComponent* pModel{ new ModelComponent(L"Meshes/Bone.ovm") };
	pEmpty->AddComponent(pModel);

	//Assign the BoneObject’s material to pModel
	pModel->SetMaterial(m_pMaterial);

	TransformComponent* pTransform{ pEmpty->GetTransform() };

	//Rotate pEmpty - 90 degrees around the Y - axis
	pTransform->Rotate(0.f, -90.f, 0.f);

	//Uniformly scale pEmpty to match the BoneObject’s length
	pTransform->Scale(m_Length);
}
