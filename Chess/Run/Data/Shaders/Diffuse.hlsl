//------------------------------------------------------------------------------------------------
struct vs_input_t
{
	float3 modelPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 modelTangent : TANGENT;
	float3 modelBitangent : BITANGENT;
	float3 modelNormal : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct v2p_t
{
	float4 clipPosition : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 worldTangent : TANGENT;
	float3 worldBitangent : BITANGENT;
	float3 worldNormal : NORMAL;
    float3 pixelPosition : PIXELPOSITION;
};

struct LightSource
{
    float3 Position;
    float padding;
    float3 Direction;
    float padding2;
    float3 Color;
    float Intensity;
    float InnerRadius;
    float OutterRadius;
    float InnerPenumbra;
    float OutterPenumbra;
};

//------------------------------------------------------------------------------------------------
cbuffer LightConstants : register(b1)
{
	float3 SunDirection;
	float SunIntensity;
	float AmbientIntensity;
    float3 SunColor;
    LightSource LightSources[8];
};

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 WorldToCameraTransform;	// View transform
	float4x4 CameraToRenderTransform;	// Non-standard transform from game to DirectX conventions
	float4x4 RenderToClipTransform;		// Projection transform
    float3 CameraPosition;
    float paddingCC;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
	float4x4 ModelToWorldTransform;		// Model transform
	float4 ModelColor;
};
//------------------------------------------------------------------------------------------------
cbuffer PerFrameConstants : register(b4)
{
	float c_time;		
    int c_debugInt;
	float c_debugFloat;
    float paddingPF;
};


//------------------------------------------------------------------------------------------------
Texture2D diffuseTexture : register(t0);
SamplerState samplerState : register(s0);
Texture2D normalMapTexture : register(t1);
SamplerState normalMapsamplerState : register(s1);
Texture2D SGETexture : register(t2);
SamplerState SGESamplerState : register(s2);

//------------------------------------------------------------------------------------------------
float SmoothStep3(float x)
{
    return (3.0 * (x * x)) - (2.0 * x) * (x * x);
}


//------------------------------------------------------------------------------------------------
float3 SmoothStop2(float3 v)
{
    float3 inverse = 1.0 - v;
    return 1.0 - (inverse * inverse);
}


//------------------------------------------------------------------------------------------------
float3 SmoothStop3(float3 v)
{
    float3 inverse = 1.0 - v;
    return 1.0 - (inverse * inverse * inverse);
}


//------------------------------------------------------------------------------------------------
float3 SmoothStart3(float3 v)
{
    return v * v * v;
}


//------------------------------------------------------------------------------------------------
float SnapToNearestFractional(float x, int numIntervals)
{
    float intervalSize = 1.0f / (float) numIntervals;
    x *= (float) numIntervals;
    x -= frac(x);
    x /= (float) numIntervals;
    return x;
}

//------------------------------------------------------------------------------------------------
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    float fraction = (inValue - inStart) / (inEnd - inStart);
    float outValue = outStart + fraction * (outEnd - outStart);
    return outValue;
}


//------------------------------------------------------------------------------------------------
float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    float fraction = saturate((inValue - inStart) / (inEnd - inStart));
    float outValue = outStart + fraction * (outEnd - outStart);
    return outValue;
}

//------------------------------------------------------------------------------------------------
float PercentageInRange(float input, float min, float max)
{
    
    if (min == max)
        return input < max;
    else 
        return saturate((input - min) / (max - min));
}

float3 XYZ2RGB(float3 XYZ){
    float R = (XYZ.x + 1) / 2;
    float G = (XYZ.y + 1) / 2;
    float B = (XYZ.z + 1) / 2;
    return float3 (R, G, B);
}

float3 RGB2XYZ(float3 RGB){
    float X = RGB.x * 2 - 1;
    float Y = RGB.y * 2 - 1;
    float Z = RGB.z * 2 - 1;
    return float3 (X, Y, Z);
}

float SpecularFactor(float gloss)
{
    return gloss * 31.f + 1.f;
}

float3x3 Orthonormalize(float3x3 inputMatrix)
{
    float3 IBasis = inputMatrix[0];
    float3 JBasis = inputMatrix[1];
    float3 KBasis = inputMatrix[2];

    IBasis = normalize(IBasis);
    float3 Ki = dot(KBasis, IBasis) * IBasis;
    KBasis = KBasis - Ki;
    KBasis = normalize(KBasis);
    float3 Ji = dot(JBasis, IBasis) * IBasis;
    float3 Jk = dot(JBasis, KBasis) * KBasis;
    JBasis = JBasis - Ji - Jk;
    JBasis = normalize(JBasis);
    return float3x3(IBasis, JBasis, KBasis);
}

float CalculateSpecular(float4 SGE, float3 pixelToLightDir, float3 PixelPosition, float3 normal)
{
    float specular = SGE.x;
    float glossiness = SGE.y;
	
    float specularExponent = RangeMap(glossiness, 0,1,1,32);
    
    float3 pixelToCameraDir = normalize(CameraPosition-PixelPosition);
	
    
    float3 sunIdealReflectionDir = normalize(pixelToLightDir + pixelToCameraDir); 
    float sunSpecularDot = saturate(dot(sunIdealReflectionDir, normal));
    float sunSpecularStrength = specular * pow(sunSpecularDot, specularExponent);
    
    return sunSpecularStrength;

}
float GetDistAndPenumbraFalloff(LightSource source, float3 position)
{
    float result = 1.f;
    float3 positionToSource = position - source.Position;
    
    float3 pixelToLightDir = normalize(positionToSource);
    
    float distanceToLight = length(positionToSource);
    
    float falloff = saturate(RangeMap(distanceToLight, source.InnerRadius, source.OutterRadius, 1.f, 0.f));
    falloff = SmoothStep3(falloff);
    
    float penumbra = saturate(RangeMap(dot(source.Direction, -pixelToLightDir), source.OutterPenumbra, source.InnerPenumbra, 0.f, 1.f));
    penumbra = SmoothStep3(penumbra);
    
    return penumbra * falloff;
}





//------------------------------------------------------------------------------------------------
v2p_t VertexMain(vs_input_t input)
{
	float4 modelPosition = float4(input.modelPosition, 1);
	float4 worldPosition = mul(ModelToWorldTransform, modelPosition);
	float4 cameraPosition = mul(WorldToCameraTransform, worldPosition);
	float4 renderPosition = mul(CameraToRenderTransform, cameraPosition);
	float4 clipPosition = mul(RenderToClipTransform, renderPosition);

    

	float3 worldTangent = mul(ModelToWorldTransform, float4(input.modelTangent, 0.0f)).xyz;
	float3 worldBitangent = mul(ModelToWorldTransform, float4(input.modelBitangent, 0.0f)).xyz;
	float3 worldNormal = mul(ModelToWorldTransform, float4(input.modelNormal, 0.0f)).xyz;


    
	v2p_t v2p;
	v2p.clipPosition = clipPosition;
	v2p.color = input.color;
	v2p.uv = input.uv;
	v2p.worldTangent = worldTangent;
	v2p.worldBitangent = worldBitangent;
	v2p.worldNormal = worldNormal;
    v2p.pixelPosition = float3(worldPosition.x, worldPosition.y, worldPosition.z);
	return v2p;
}



//------------------------------------------------------------------------------------------------
float4 PixelMain(v2p_t input) : SV_Target0
{
    float ambient =   AmbientIntensity;
    
	float4 textureColor = diffuseTexture.Sample(samplerState, input.uv);

    float4 diffuseTexel = diffuseTexture.Sample(samplerState, input.uv);
    float4 surfaceColor = input.color;
    
    float4 normalMap = normalMapTexture.Sample(normalMapsamplerState, input.uv);
    float3 normalMapXYZ = normalize(RGB2XYZ(normalMap.rgb));
    
    float4 SGE = SGETexture.Sample(SGESamplerState, input.uv);
    
    
    float4 diffuseColor = diffuseTexel * surfaceColor;
    clip(diffuseColor.a - 0.01f);
    
    float3x3 TBNToWorld = float3x3(normalize(input.worldTangent), normalize(input.worldBitangent), normalize(input.worldNormal));
    float3 TBBNNormal = mul(normalMapXYZ, TBNToWorld);

    float3 totalDiffuseLight = float3(0.f, 0.f, 0.f); 
    float3 totalSpecularLight = float3(0.f, 0.f, 0.f);
    
    float sunlightStrength =  saturate(dot(-SunDirection, TBBNNormal));
    float3 diffuseLightFromSun = sunlightStrength * SunColor;
    totalDiffuseLight += diffuseLightFromSun * SunIntensity + SunColor*AmbientIntensity;
    
    float sunSpecularLight = CalculateSpecular(SGE, -SunDirection, input.pixelPosition, TBBNNormal);
    totalSpecularLight += sunSpecularLight*SunIntensity;
    
    for (int i = 0; i < 8; i++)
    {
        if (LightSources[i].Intensity > 0)
        {
            float3 pixelToLightDisp = LightSources[i].Position - input.pixelPosition;
            float3 pixelToLightDir = normalize(pixelToLightDisp);
            
            float totalFalloff = GetDistAndPenumbraFalloff(LightSources[i], input.pixelPosition);
            
            float lightStrength = totalFalloff * LightSources[i].Intensity * saturate(RangeMap(dot(pixelToLightDir, TBBNNormal), -ambient, 1.0, 0.0, 1.0));
            float3 diffuseLight = lightStrength * LightSources[i].Color;
            totalDiffuseLight += diffuseLight;
            
            
            float specularStrength = CalculateSpecular(SGE, pixelToLightDir, input.pixelPosition, TBBNNormal);
            specularStrength *= totalFalloff;
    
            float3 specularLight = specularStrength * LightSources[i].Color;
            totalSpecularLight += specularLight;
        }
    
    }
    float3 emissiveLight = diffuseTexel.rgb * SGE.z;
    float3 finalRGB = (saturate(totalDiffuseLight) * diffuseColor.rgb) + (totalSpecularLight * SGE.x) + emissiveLight;
    float4 finalColor = float4(finalRGB, diffuseColor.a)*ModelColor;
    //finalColor = float4(1,0,1,1);
    return finalColor;
}

