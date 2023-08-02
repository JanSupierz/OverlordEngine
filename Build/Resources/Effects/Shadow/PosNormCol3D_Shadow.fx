float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.01f;

float4 gColor : COLOR = float4(1, 1, 1, 1);
Texture2D gShadowMap;

SamplerComparisonState cmpSampler
{
	// sampler state
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = MIRROR;
	AddressV = MIRROR;

	// sampler comparison state
	ComparisonFunc = LESS_EQUAL;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float4 lPos : TEXCOORD1;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

BlendState EnableBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){
	VS_OUTPUT output;

	output.pos = mul(float4(input.pos, 1.f), gWorldViewProj);
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));

	//Calculate light space position
	output.lPos = mul(float4(input.pos, 1.f), gWorldViewProj_Light);

	return output;
}

float2 texOffset(int u, int v)
{
	//Calculate texture offset for shadow map
	float2 texelSize = 1.0f / float2(1280, 720);

	return float2(texelSize.x * u, texelSize.y * v);
}

float EvaluateShadowMap(float4 lpos)
{
	//Re-homogenize position after interpolation
	lpos.xyz /= lpos.w;

	//If position is not visible to the light - dont illuminate it
	//results in hard light frustum
	if (lpos.x < -1.f || lpos.x > 1.f ||
		lpos.y < -1.f || lpos.y > 1.f ||
		lpos.z < 0.f || lpos.z > 1.f) return 0.f;

	//Transform clip space coords to texture space coords (-1:1 to 0:1)
	lpos.x = (lpos.x * 0.5f) + 0.5f;
	lpos.y = (lpos.y * -0.5f) + 0.5f;

	//Apply shadow map bias
	lpos.z -= gShadowMapBias;

	//PCF sampling for shadow map
	float sum = 0;
	float x, y;

	//perform PCF filtering on a 4 x 4 texel neighborhood
	for (y = -1.5; y <= 1.5; y += 1.0)
	{
		for (x = -1.5; x <= 1.5; x += 1.0)
		{
			sum += gShadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
		}
	}

	float shadowValue = sum / 16.0;

	//Interpolate between shadowed and lit pixels based on the shadow value
	return lerp(0.5f, 1.f, shadowValue);
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float shadowValue = EvaluateShadowMap(input.lPos);

	float3 color_rgb= gColor.rgb;
	float color_a = gColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb *= diffuseStrength;
	
	return float4( color_rgb * shadowValue, color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique11 TransparencyTech
{
	pass P0
	{
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}

