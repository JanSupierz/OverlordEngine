#include "stdafx.h"
#include "UberMaterial.h"

UberMaterial::UberMaterial() :
	Material<UberMaterial>(L"Effects/UberShader.fx")
{}

void UberMaterial::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", XMFLOAT3{ -0.551f, 0.534f, -0.641f });

	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT4{ 1.f, 0.f, 0.f, 1.f });

	SetVariable_Vector(L"gColorSpecular", XMFLOAT4{ 0.f, 1.f, 0.2f, 1.f });
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
	SetVariable_Scalar(L"gShininess", 10.f);

	SetVariable_Vector(L"gColorAmbient", XMFLOAT4{ 0.3f, 0.3f, 0.3f, 1.f });
	SetVariable_Scalar(L"gAmbientIntensity", 0.1f);

	//Flip -> looks better
	SetVariable_Scalar(L"gFlipGreenChannel", true);
	SetVariable_Scalar(L"gUseTextureNormal", true);

	//Reflection of environment
	SetVariable_Scalar(L"gUseEnvironmentMapping", true);

	//Reflections - highlights on edges
	SetVariable_Scalar(L"gReflectionStrength", 0.3f);
	SetVariable_Scalar(L"gRefractionStrength", 0.4f);
	SetVariable_Scalar(L"gRefractionIndex", 0.6f);

	//Falloff - reflections only on the edges
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Vector(L"gColorFresnel", XMFLOAT4{ 1.f, 1.f, 1.f, 1.f });

	//Highlights on edges
	SetVariable_Scalar(L"gFresnelPower", 2.4f);
	SetVariable_Scalar(L"gFresnelMultiplier", 1.5f);
	SetVariable_Scalar(L"gFresnelHardness", 1.4f);

	//Highlights in center
	SetVariable_Scalar(L"gUseSpecularBlinn", false);
	SetVariable_Scalar(L"gUseSpecularPhong", true);
	
	SetVariable_Texture(L"gTextureDiffuse", ContentManager::Load<TextureData>(L"Textures/Skulls_DiffuseMap.tga"));
	SetVariable_Texture(L"gTextureSpecularIntensity", ContentManager::Load<TextureData>(L"Textures/Skulls_HeightMap.tga"));
	SetVariable_Texture(L"gTextureNormal", ContentManager::Load<TextureData>(L"Textures/Skulls_NormalMap.tga"));
	SetVariable_Texture(L"gCubeEnvironment", ContentManager::Load<TextureData>(L"Textures/Sunol_Cubemap.dds"));
}
