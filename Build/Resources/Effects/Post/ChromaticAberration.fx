//=============================================================================
//// Shader uses position and texture
//=============================================================================
//https://www.youtube.com/watch?v=3CsNRBme6nU

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

float2 gOffsetPixelsR = float2(5.f, -5.f);
float2 gOffsetPixelsG = float2(3.f, 1.f);
float2 gOffsetPixelsB = float2(2.f, 3.f);

//Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

//Create Rasterizer State (Backface culling) 
RasterizerState BackCulling
{
    CullMode = BACK;
};


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
    
	// Set the Position
    output.Position = float4(input.Position, 1.f);
	
	// Set the TexCoord
    output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
    float width, height;
    gTexture.GetDimensions(width, height);
    
    float dx = 1 / width;
    float dy = 1 / height;
    
    float2 offsetR = float2(gOffsetPixelsR.x * dx, gOffsetPixelsR.y * dy);
    float2 offsetG = float2(gOffsetPixelsG.x * dx, gOffsetPixelsG.y * dy);
    float2 offsetB = float2(gOffsetPixelsB.x * dx, gOffsetPixelsB.y * dy);
    
    float r = gTexture.Sample(samPoint, input.TexCoord + offsetR).r;
    float g = gTexture.Sample(samPoint, input.TexCoord + offsetG).g;
    float b = gTexture.Sample(samPoint, input.TexCoord + offsetB).b;
    
	// Step 5: return the final color
    return float4(r, g, b, 1.f);
}


//TECHNIQUE
//---------
technique11 ChromaticAberration
{
    pass P0
    {
        SetRasterizerState(BackCulling);
        SetDepthStencilState(EnableDepth, 0);

        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}