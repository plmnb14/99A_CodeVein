matrix		g_matWorld, g_matView, g_matProj;
matrix		g_matLightView, g_matLightProj;
matrix		g_matBias;
matrix		g_LightVP_Close, g_LightVP_Medium, g_LightVP_Far;

// ºû
vector		g_vLightDir;
vector		g_vLightPos;
float		g_fRange;

vector		g_vLightDiffuse;
vector		g_vLightAmbient;
vector		g_vLightSpecular;

vector		g_vMtrlAmbient = (vector)1.f;
vector		g_vMtrlSpecular = (vector)1.f;

vector		g_vCamPosition;
matrix		g_matProjInv;
matrix		g_matViewInv;

float3		LightDirection1 = float3(-1.5, 0.45, 0);


texture		g_DiffuseTexture;
texture		g_NormalTexture;
texture		g_SpecularTexture;
texture		g_EmissiveTexture;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

sampler		NormalSampler = sampler_state
{
	texture = g_NormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

sampler		SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

sampler		EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;
};

texture		g_ShadowMapTexture;
sampler ShadowMapSampler = sampler_state
{
	texture = g_ShadowMapTexture;

	//minfilter = linear;
	//magfilter = linear;
	//mipfilter = linear;
};


struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBinormal	: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition	: POSITION;
	float2		vTexUV		: TEXCOORD0;
	float4		vDepth		: TEXCOORD1;
	float4		vShadowUV	: TEXCOORD2;
	float4		vWorldPos   : TEXCOORD3;
	float4		vColor		: COLOR0;
	float		fDepth		: TEXCOORD4;
};


struct PS_IN
{
	float4		vPosition	: POSITION;
	float2		vTexUV		: TEXCOORD0;
	float4		vDepth		: TEXCOORD1;
	float4		vShadowUV	: TEXCOORD2;
	float4		vWorldPos   : TEXCOORD3;
	float4		vColor		: COLOR0;
	float		fDepth : TEXCOORD4;
};

struct PS_OUT
{
	vector		vDiffuse : COLOR0;
};

VS_OUT VS_SHADOWMAP(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(float4(In.vPosition.xyz, 1.f), g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matLightView);
	Out.vPosition = mul(Out.vPosition, g_matLightProj);

	Out.vDepth = Out.vPosition;

	return Out;
}

VS_OUT VS_SHADOW(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition.xyz, 1), matWVP);

	Out.vDepth = mul(float4(In.vPosition.xyz, 1), g_matWorld);
	Out.vDepth = mul(Out.vDepth, g_matLightView);
	Out.vDepth = mul(Out.vDepth, g_matLightProj);

	Out.vShadowUV = mul(Out.vDepth, g_matBias);

	return Out;
}


PS_OUT PS_SHADOWMAP(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float fDepth = In.vDepth.z;
	//float fDepth = In.vDepth.z / In.vDepth.w;

	Out.vDiffuse = float4(fDepth, fDepth, fDepth, 1.f);

	//Out.vDiffuse = fDepth;

	return Out;
}

PS_OUT PS_SHADOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vTexCoord[9];
	
	float fTexelSizeX = 1.f / 1280.f;
	float fTexelSizeY = 1.f / 720.f;
	
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
		//float fDepthValue = tex2Dproj(ShadowMapSampler, vTexCoord[i]).x;
		//
		//if (fDepthValue * In.vDepth.w < In.vDepth.z - 0.001f)
		//{
		//	fShadowTerms[i] = 0.2f;
		//}
		//
		//else
		//	fShadowTerms[i] = 1.f;
		//
		//fShadowTerm += float(fShadowTerms[i]);

		float A = tex2Dproj(ShadowMapSampler, vTexCoord[i]).x;
		float B = (In.vDepth.z - 0.00125f);
		
		fShadowTerms[i] = (A < B ? 0.f : 1.f);
		fShadowTerm += float(fShadowTerms[i]);
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

