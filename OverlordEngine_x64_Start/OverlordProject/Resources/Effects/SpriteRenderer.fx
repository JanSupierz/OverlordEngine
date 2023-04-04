float4x4 gTransform : WorldViewProjection;
Texture2D gSpriteTexture;
float2 gTextureSize;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

DepthStencilState NoDepth
{
    DepthEnable = FALSE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

//SHADER STRUCTS
//**************
struct VS_DATA
{
    uint TextureId : TEXCOORD0; //Can be ignored
    float4 TransformData : POSITION; //PosX, PosY, Depth (PosZ), Rotation
    float4 TransformData2 : POSITION1; //PivotX, PivotY, ScaleX, ScaleY
    float4 Color : COLOR;
};

struct GS_DATA
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

//VERTEX SHADER
//*************
VS_DATA MainVS(VS_DATA input)
{
    return input;
}

//GEOMETRY SHADER
//***************
void CreateVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float2 texCoord, float rotation, float2 rotCosSin, float2 offset, float2 pivotOffset)
{
    if (rotation != 0)
    {
        //Transform to origin
        pos.xy -= (pivotOffset + offset);

        //Rotate
        pos.xy = mul(float2x2(rotCosSin.x, -rotCosSin.y, rotCosSin.y, rotCosSin.x), pos.xy);

        //Retransform to initial position
        pos.xy += offset;
    }
    else
    {
        //Just apply the pivot offset
        pos.xy -= pivotOffset;
    }

	//Geometry Vertex Output
    GS_DATA geomData = (GS_DATA) 0;
    geomData.Position = mul(float4(pos, 1.0f), gTransform);
    geomData.Color = col;
    geomData.TexCoord = texCoord;
    triStream.Append(geomData);
}

[maxvertexcount(4)]
void MainGS(point VS_DATA vertex[1], inout TriangleStream<GS_DATA> triStream)
{
	//Given Data (Vertex Data)
    //Extract the position data from the VS_DATA vertex struct
    float3 position = vertex[0].TransformData.xyz;

    //Extract the offset data from the VS_DATA vertex struct (initial X and Y position)
    float2 offset = vertex[0].TransformData.xy;

    //Extract the rotation data from the VS_DATA vertex struct
    float rotation = vertex[0].TransformData.w;

    //Extract the pivot data from the VS_DATA vertex struct
    float2 pivot = vertex[0].TransformData2.xy;

    //Extract the scale data from the VS_DATA vertex struct
    float2 scale = vertex[0].TransformData2.zw;

    //Initial Texture Coordinate
    float2 texCoord = float2(0, 0); 

    //Cos and sin rotation
    float2 rotCosSin = float2(cos(rotation), sin(rotation));

    //Color
    float4 color = vertex[0].Color;

    //Scaled Pivot
    pivot.xy *= gTextureSize.xy * scale.xy;

	// LT----------RT //TringleStrip (LT > RT > LB, LB > RB > RT)
	// |          / |
	// |       /    |
	// |    /       |
	// | /          |
	// LB----------RB

	//VERTEX 1 [LT]
    CreateVertex(triStream, position, color, texCoord, rotation, rotCosSin, offset, pivot);

	//VERTEX 2 [RT]
    position.x += gTextureSize.x * scale.x;
    texCoord.x = 1;

    CreateVertex(triStream, position, color, texCoord, rotation, rotCosSin, offset, pivot);
                                 
	//VERTEX 3 [LB]
    position.x -= gTextureSize.x * scale.x;
    texCoord.x = 0;

    position.y += gTextureSize.y * scale.y;
    texCoord.y = 1;

    CreateVertex(triStream, position, color, texCoord, rotation, rotCosSin, offset, pivot);
                                 
	//VERTEX 4 [RB]
    position.x += gTextureSize.x * scale.x;
    texCoord.x = 1;

    CreateVertex(triStream, position, color, texCoord, rotation, rotCosSin, offset, pivot);
}

//PIXEL SHADER
//************
float4 MainPS(GS_DATA input) : SV_TARGET
{
    return gSpriteTexture.Sample(samPoint, input.TexCoord) * input.Color;
}

// Default Technique
technique11 Default
{
    pass p0
    {
        SetRasterizerState(BackCulling);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetDepthStencilState(NoDepth,0);
        SetVertexShader(CompileShader(vs_4_0, MainVS()));
        SetGeometryShader(CompileShader(gs_4_0, MainGS()));
        SetPixelShader(CompileShader(ps_4_0, MainPS()));
    }
}
