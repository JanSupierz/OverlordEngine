#include "stdafx.h"
#include "ColorMaterial_Shadow.h"

ColorMaterial_Shadow::ColorMaterial_Shadow() :
	Material(L"Effects/Shadow/PosNormCol3D_Shadow.fx")
{}

void ColorMaterial_Shadow::SetColor(const XMFLOAT4& color) const
{
	SetVariable_Vector(L"gColor", color);
}

void ColorMaterial_Shadow::SetColor(const XMVECTORF32& color) const
{
	SetVariable_Vector(L"gColor", color);
}

void ColorMaterial_Shadow::InitializeEffectVariables()
{
}

void ColorMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
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
}
