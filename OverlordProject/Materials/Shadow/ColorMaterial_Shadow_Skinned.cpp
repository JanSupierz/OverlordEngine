#include "stdafx.h"
#include "ColorMaterial_Shadow_Skinned.h"

ColorMaterial_Shadow_Skinned::ColorMaterial_Shadow_Skinned() :
	Material(L"Effects/Shadow/PosNormCol3D_Shadow_Skinned.fx")
{}

void ColorMaterial_Shadow_Skinned::SetColor(const XMFLOAT4& color) const
{
		SetVariable_Vector(L"gColor", color);
}

void ColorMaterial_Shadow_Skinned::SetColor(const XMVECTORF32& color) const
{
	SetVariable_Vector(L"gColor", color);
}

void ColorMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void ColorMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	//Update The Shader Variables
	//1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	//	LightWVP = model_world * light_viewprojection
	//	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	const auto pShadowMapRenderer{ ShadowMapRenderer::Get() };

	const XMMATRIX worldViewProjection
	{
		XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()) *
		XMLoadFloat4x4(&pShadowMapRenderer->GetLightVP())
	};

	XMFLOAT4X4 worldViewProjFloat4x4;
	XMStoreFloat4x4(&worldViewProjFloat4x4, worldViewProjection);

	SetVariable_Matrix(L"gWorldViewProj_Light", worldViewProjFloat4x4);

	//2. Update the ShadowMap texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());

	//3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);

	//4. Update Bones
	ModelAnimator* pAnimator{ pModel->GetAnimator() };
	ASSERT_NULL_(pAnimator);

	const auto& transforms{ pAnimator->GetBoneTransforms() };
	const auto pData{ reinterpret_cast<const float*>(transforms.data()) };
	const auto size{ static_cast<UINT>(transforms.size()) };

	SetVariable_MatrixArray(L"gBones", pData, size);
}
