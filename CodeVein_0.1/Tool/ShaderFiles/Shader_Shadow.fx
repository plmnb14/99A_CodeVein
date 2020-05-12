
matrix		g_matWorld;
matrix		g_matWVP;
matrix		g_matLightVP;
matrix		g_matBias;

int g_iScreenX = 1980;
int g_iScreenY = 720;

//==========================================================================

texture		g_ShadowMapTexture;

sampler ShadowMapSampler = sampler_state
{
	texture = g_ShadowMapTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = border;
	addressV = border;

	BorderColor = float4(1.f, 0.0f, 0.0f, 1.0f);
};

//==========================================================================

struct VS_IN_ShadowMap
{
	float3		vPosition	: POSITION;
};

struct VS_IN_ShadowRender
{
	float3		vPosition	: POSITION;
};

struct VS_OUT_ShadowMap
{
	float4		vPosition	: POSITION;
	float4		vDepth		: TEXCOORD0;
};

struct VS_OUT_ShadowRender
{
	float4		vPosition	: POSITION;
	float4		vDepth		: TEXCOORD0;
	float4		vShadowUV	: TEXCOORD1;
};

//==========================================================================

VS_OUT_ShadowMap VS_SHADOWMAP(VS_IN_ShadowMap In)
{
	VS_OUT_ShadowMap Out = (VS_OUT_ShadowMap)0;

	matrix matLightWVP = mul(g_matWorld, g_matLightVP);
	
	Out.vPosition = mul(float4(In.vPosition.xyz, 1.f), matLightWVP);
	Out.vDepth = Out.vPosition;

	return Out;
}

VS_OUT_ShadowRender VS_SHADOW(VS_IN_ShadowRender In)
{
	VS_OUT_ShadowRender Out = (VS_OUT_ShadowRender)0;

	matrix matLightWVP = mul(g_matWorld , g_matLightVP);

	Out.vPosition = mul(float4(In.vPosition.xyz, 1), g_matWVP);
	Out.vDepth = mul(float4(In.vPosition.xyz, 1), matLightWVP);
	Out.vShadowUV = mul(Out.vDepth, g_matBias);

	return Out;
}

//==========================================================================

struct PS_IN_ShadowMap
{
	float4		vPosition	: POSITION;
	float4		vDepth		: TEXCOORD0;
};

struct PS_IN_ShadowRender
{
	float4		vPosition	: POSITION;
	float4		vDepth		: TEXCOORD0;
	float4		vShadowUV	: TEXCOORD1;
};

struct PS_OUT
{
	float4		vDiffuse : COLOR0;
};

PS_OUT PS_SHADOWMAP(PS_IN_ShadowMap In)
{
	PS_OUT Out = (PS_OUT)0;

	// 정규화
	float fDepth = In.vDepth.z / In.vDepth.w;

	Out.vDiffuse = float4(fDepth, fDepth, fDepth, 1.f);

	return Out;
}

PS_OUT PS_SHADOW(PS_IN_ShadowRender In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTexCoord[9];
	
	//float fTexelSizeX = 1.f / 1280.f;
	//float fTexelSizeY = 1.f / 720.f;
	float fTexelSizeX = 1.f / g_iScreenX;
	float fTexelSizeY = 1.f / g_iScreenY;
	//float fTexelSizeX = 1.f / 2560.f;
	//float fTexelSizeY = 1.f / 1440.f;
	
	vTexCoord[0] = In.vShadowUV;
	vTexCoord[1] = In.vShadowUV + float4(-fTexelSizeX, 0.f, 0.f, 0.f);
	vTexCoord[2] = In.vShadowUV + float4(fTexelSizeX, 0.f, 0.f, 0.f);
	vTexCoord[3] = In.vShadowUV + float4(0.f, -fTexelSizeY, 0.f, 0.f);
	vTexCoord[4] = In.vShadowUV + float4(0.f, fTexelSizeY, 0.f, 0.f);
	vTexCoord[5] = In.vShadowUV + float4(-fTexelSizeX, -fTexelSizeY, 0.f, 0.f);
	vTexCoord[6] = In.vShadowUV + float4(fTexelSizeX, -fTexelSizeY, 0.f, 0.f);
	vTexCoord[7] = In.vShadowUV + float4(-fTexelSizeX, fTexelSizeY, 0.f, 0.f);
	vTexCoord[8] = In.vShadowUV + float4(fTexelSizeX, fTexelSizeY, 0.f, 0.f);;
	
	float fShadowTerms[9];
	float fShadowTerm = 0.f;
	
	for (int i = 0; i < 9; i++)
	{
		float fDepthValue = tex2Dproj(ShadowMapSampler, vTexCoord[i]).x;
		float cosTheta = dot(0.99f, 1.f);
		//float bias = 0.00025f * tan(acos(cosTheta));	// 카메라 10,10 일때
		float bias = 0.0025f * tan(acos(cosTheta));	// 카메라 64,36 일때

		bias = clamp(bias, 0, 0.01);

		if (fDepthValue * In.vDepth.w < In.vDepth.z - bias)
		{
			fShadowTerms[i] = 0.2f;
		}
		
		else
			fShadowTerms[i] = 1.f;
		
		fShadowTerm += float(fShadowTerms[i]);
	
		//float A = tex2Dproj(ShadowMapSampler, vTexCoord[i]).x;
		//float B = (In.vDepth.z - 0.00001f);
		//
		//fShadowTerms[i] = (A < B ? 0.2f : 1.f);
		//fShadowTerm += float(fShadowTerms[i]);
	}
	
	fShadowTerm /= 9.f;
	
	Out.vDiffuse = fShadowTerm;

	return Out;
}

technique Default_Technique
{
	pass ShadowMap
	{
		VertexShader = compile vs_3_0 VS_SHADOWMAP();
		PixelShader = compile ps_3_0 PS_SHADOWMAP();
	}

	pass Shadow
	{
		VertexShader = compile vs_3_0 VS_SHADOW();
		PixelShader = compile ps_3_0 PS_SHADOW();
	}

}

