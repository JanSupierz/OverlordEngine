//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

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
	// Step 1: find the dimensions of the texture (the texture has a method for that)	
    float width, height;
    gTexture.GetDimensions(width, height);
	
	// Step 2: calculate dx and dy (UV space for 1 pixel)	
    float dx = 1.f / width;
    float dy = 1.f / height;
	
	// Step 3: Create a double for loop (5 iterations each)
    const int nrIterations = 5;
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
    for (int x = -nrIterations / 2; x <= nrIterations / 2; ++x)
    {
        for (int y = -nrIterations / 2; y <= nrIterations / 2; ++y)
        {
            //Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
            float2 offset = float2(x * dx * 2, y * dy * 2);
            color += gTexture.Sample(samPoint, input.TexCoord + offset);
        }
    
    }
    
    // Step 4: Divide the final color by the number of passes (in this case 5*5)	
    color /= nrIterations * nrIterations;
    
	// Step 5: return the final color
    return color;
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
        SetRasterizerState(BackCulling);
        SetDepthStencilState(EnableDepth, 0);

        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}