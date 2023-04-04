#include "stdafx.h"
#include "DiffuseMaterial_Skinned.h"

DiffuseMaterial_Skinned::DiffuseMaterial_Skinned():
	Material(L"Effects/PosNormTex3D_Skinned.fx")
{
}

void DiffuseMaterial_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Skinned::OnUpdateModelVariables(const SceneContext&, const ModelComponent* pModel) const
{
	//Retrieve The Animator from the ModelComponent
	ModelAnimator* pAnimator{ pModel->GetAnimator() };
	ASSERT_NULL_(pAnimator);

	//Retrieve the BoneTransforms from the Animator
	const auto& transforms{ pAnimator->GetBoneTransforms() };

	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms
	SetVariable_MatrixArray(L"gBones", reinterpret_cast<const float*>(transforms.data()), static_cast<UINT>(transforms.size()));
}
