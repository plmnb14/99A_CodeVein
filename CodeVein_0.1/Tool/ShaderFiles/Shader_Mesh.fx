

matrix		g_matWorld, g_matView, g_matProj, g_matLastWVP;// , g_matInvVP, g_matLastVP;
matrix		g_LightVP_Close, g_LightVP_Medium, g_LightVP_Far;
float		g_fFxAlpha;
bool		g_bDissolve = false;

float3		g_vCamPos;

vector		g_vRimColor = (1.f, 1.f, 1.f, 1.f);
float		g_fRimPower = 5.f;
float		g_fRimAlpha = 0.f;

float		g_fSpecularPower = 1.0f;
float		g_fEmissivePower = 1.0f;
float		g_fRoughnessPower = 1.0f;
float		g_fMinSpecular = 0.f;

float		g_fID_R_Power = 0.f;
float		g_fID_G_Power = 0.f;
float		g_fID_B_Power = 0.f;

bool		g_bToonRimLight = false;


float4		g_vLightDir;

//=====================================================

texture		g_DiffuseTexture;
texture		g_NormalTexture;
texture		g_SpecularTexture;
texture		g_EmissiveTexture;
texture		g_IDTexture;
texture		g_UnionTexture;
texture		g_RoughnessTexture;
texture		g_TransperencyTexture;
texture		g_HeightTexture;
texture		g_ZTexture;

//=====================================================
	
float4		g_vSpecularColor = (0.1f, 0.1f, 0.1f, 0.1f);

//====================================================================================================
// Diffuse Map
//====================================================================================================
sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Normal Map
//====================================================================================================
sampler		NormalSampler = sampler_state
{
	texture = g_NormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Normal Map
//====================================================================================================
sampler		ZSampler = sampler_state
{
	texture = g_ZTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Specular Map
//====================================================================================================
sampler		SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Emissive Map
//====================================================================================================
sampler		EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

//====================================================================================================
// Height Map
//====================================================================================================
sampler		HeightSampler = sampler_state
{
	texture = g_HeightTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

//====================================================================================================
// ID Map
//====================================================================================================
sampler		IDSampler = sampler_state
{
	texture = g_IDTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Union Map
//====================================================================================================
sampler		UnionSampler = sampler_state
{
	texture = g_UnionTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Roughness Map
//====================================================================================================
sampler		RoughnessSampler = sampler_state
{
	texture = g_RoughnessTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================
// Transparency Map
//====================================================================================================
sampler		TransparencySampler = sampler_state
{
	texture = g_TransperencyTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================


texture		g_FXTexture;
sampler		FXSampler = sampler_state
{
	texture = g_FXTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


matrix		g_MatrixPalette[200];
int			g_iNumBoneInfluences = 8;

struct VS_IN_Dynamic_Skinned
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBinormal	: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		weights		: BLENDWEIGHT0;
	int4		boneIndices : BLENDINDICES0;
};

struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBinormal	: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_IN_Flag
{
	float3		vPosition	: POSITION;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float3		N	: NORMAL;
	float3		T	: TANGENT;
	float3		B	: BINORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vRimDir : TEXCOORD2;
	float4		vRimNormal : TEXCOORD3;
};

struct VS_OUT_Flag
{
	float3		vPosition	: POSITION;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_BLUROUT
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

// 정점의 기초적인 변환을 수행한다.
// 정점의 구성 요소를 변형할 수 있다.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	// 월드변환, 뷰변환, 투영행렬변환.
	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.N = normalize(mul(In.vNormal, (float3x3)g_matWorld));
	Out.T = normalize(mul(In.vTangent, (float3x3)g_matWorld));
	Out.B = normalize(mul(In.vBinormal, (float3x3)g_matWorld));

	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;

	float4 vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);

	Out.vRimDir = float4(normalize(g_vCamPos - vWorldPos.xyz), 1.f);
	Out.vRimNormal = float4(normalize(In.vNormal) , 0.f);

	return Out;
}

VS_BLUROUT VS_MOTIONBLUR(VS_IN In)
{
	VS_BLUROUT	Out = (VS_BLUROUT)0;

	// 월드변환, 뷰변환, 투영행렬변환.
	matrix		matWV, matWVP, matVP;

	matWV = mul(g_matWorld, g_matView);
	matVP = mul(g_matView, g_matProj);
	matWVP = mul(matWV, g_matProj);

	//==============================================================================

	float4 currentPos = mul(float4(In.vPosition.xyz, 1.f), matWVP);

	// Use the world position, and transform by the previous view-projection matrix.    
	float4 previousPos = mul(float4(In.vPosition.xyz, 1.f), g_matLastWVP);

	Out.vPosition = currentPos;
	Out.vProjPos = Out.vPosition;
	Out.vLastPos = previousPos;

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_Flag VS_Flag(VS_IN_Flag In)
{
	VS_OUT_Flag			Out = (VS_OUT_Flag)0;

	// 월드변환, 뷰변환, 투영행렬변환.
	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_Dynamic_Skinned(VS_IN_Dynamic_Skinned In)
{
	VS_OUT			Out = (VS_OUT)0;

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	float lastWeight = 0.f;
	int n = g_iNumBoneInfluences - 1;
	
	float3 vInNormal = normalize(In.vNormal);
	float3 vInTangent = normalize(In.vTangent);
	float3 vInBinormal = normalize(In.vBinormal);
	
	float3 vOutNormal = float3(0.f, 0.f, 0.f);
	float3 vOutTangent = float3(0.f, 0.f, 0.f);
	float3 vOutBinormal = float3(0.f, 0.f, 0.f);
	
	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.vPosition, g_MatrixPalette[In.boneIndices[i]]);
	
		vOutNormal += In.weights[i] * mul(vInNormal, g_MatrixPalette[In.boneIndices[i]]);
		vOutTangent += In.weights[i] * mul(vInTangent, g_MatrixPalette[In.boneIndices[i]]);
		vOutBinormal += In.weights[i] * mul(vInBinormal, g_MatrixPalette[In.boneIndices[i]]);
	}
	
	lastWeight = 1.f - lastWeight;
	p += lastWeight * mul(In.vPosition, g_MatrixPalette[In.boneIndices[n]]);
	
	vOutNormal += lastWeight * mul(vInNormal, g_MatrixPalette[In.boneIndices[n]]);
	vOutTangent += lastWeight * mul(vInTangent, g_MatrixPalette[In.boneIndices[n]]);
	vOutBinormal += lastWeight * mul(vInBinormal, g_MatrixPalette[In.boneIndices[n]]);
	
	p.w = 1.f;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.N = normalize(mul(vOutNormal, (float3x3)g_matWorld));
	Out.T = normalize(mul(vOutTangent, (float3x3)g_matWorld));
	Out.B = normalize(mul(vOutBinormal, (float3x3)g_matWorld));

	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;

	float4 vWorldPos = mul(vector(In.vPosition, 1.f), g_matWorld);

	Out.vRimDir = float4(normalize(g_vCamPos - vWorldPos.xyz), 1.f);
	Out.vRimNormal = float4(normalize(In.vNormal), 0.f);

	return Out;
}
// POSITION시멘틱을 가진 멤버변수에 대해서 W값으로 XYZW를 나누는 연산을 수행.(원근 투영)
// 투영스페이스 상에 존재하는 정점(-1, 1 ~ 1, -1)을 뷰포트영역상의 정점(0, 0 ~ WINCX, WINCY)으로 변환한다.
// 래스터라이즈 : 세개 정점에 둘러쌓여진 영역안에 존재하는 픽셀의 정보를 정점정보를 기반하여 생성한다.

struct PS_IN
{
	float4		vPosition	: POSITION;
	float3		N			: NORMAL;
	float3		T			: TANGENT;
	float3		B			: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		vProjPos	: TEXCOORD1;
	float4		vRimDir		: TEXCOORD2;
	float4		vRimNormal : TEXCOORD3;
};

struct PS_BLURIN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
	vector		vVelocity : COLOR3;
};

struct PS_OUT_ADVENCE
{
	vector		vDiffuse	: COLOR0;
	vector		vNormal		: COLOR1;
	vector		vDepth		: COLOR2;
	vector		vEmissive	: COLOR3;
};

// 픽셀의 색을 결정한다.
PS_OUT_ADVENCE PS_MAIN(PS_IN In)
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	float2 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xy;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);
	//  스펙큘러 텍스쳐 추가되면 변경예정

	//========================================================================================================================


	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DN(PS_IN In)
{
	// 디퓨즈 | 노말

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNT(PS_IN In)
{
	// 디퓨즈 | 노말 | 반투명맵

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);
	// Depth.z == SpecularIntensity.x ( 스펙큘러의 x )

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNS(PS_IN In)
{
	// 디퓨즈 | 노말 | 스펙큘러

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);
	// SpecularIntensity.y 가 스펙큘러 같음

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, SpecularIntensity.y);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	if (g_bToonRimLight)
		fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNE(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = pow(tex2D(EmissiveSampler, In.vTexUV), 2.2) + fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNSE(PS_IN In)
{
	// 유니온 아직 안함

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, SpecularIntensity.x , SpecularIntensity.y);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = pow(tex2D(EmissiveSampler, In.vTexUV), 2.2) * 3.f + fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNR(PS_IN In)
{
	// 러프니스는 아직 구현은 안해놓음.

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNU(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;

	// 메탈니스 : 빛 전체의 강도
	float Metalness = vUnion.x;
	// 러프니스 : 정반사의 정도.. 쉽게 말하면 == Specular Power
	float Roughness = vUnion.y;
	// AO
	float AO = vUnion.z;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	//Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);
	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);

	//========================================================================================================================

	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness * g_fRoughnessPower, Metalness * g_fSpecularPower);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness, Metalness);

	//========================================================================================================================
	//float fRim = 1.f - saturate(dot(In.N, In.vRimDir));
	//
	//float4 fFinalRimColor = g_vRimColor;
	//float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;
	//
	//if (g_bToonRimLight)
	//	fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);
	//
	//Out.vEmissive = fFinalRim;
	//Out.vEmissive.a = 1.f;


	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	Out.vEmissive.a = 1.f;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNI(PS_IN In)
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	float2 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xy;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNSU(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;

	// 메탈니스 : 빛 전체의 강도
	float Metalness = vUnion.x;
	// 러프니스 : 정반사의 정도.. 쉽게 말하면 == Specular Power
	float Roughness = vUnion.y;
	// AO
	float AO = vUnion.z;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);

	//========================================================================================================================

	float finalMetalness = (Metalness * g_fSpecularPower) + g_fMinSpecular * SpecularIntensity.x;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness * g_fRoughnessPower, finalMetalness);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;	
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNSH(PS_IN In)
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	//Out.vDiffuse = ceil(Out.vDiffuse * 3.f) / 3.f;

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float HeightValue = tex2D(HeightSampler, In.vTexUV).x;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, SpecularIntensity.x, SpecularIntensity.y);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	if(g_bToonRimLight)
		fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);
	//fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNSHU(PS_IN In)
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = tex2D(DiffuseSampler, In.vTexUV);
	//Out.vDiffuse = ceil(Out.vDiffuse * 3.f) / 3.f;

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float HeightValue = tex2D(HeightSampler, In.vTexUV).x;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, SpecularIntensity.x, SpecularIntensity.y);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	if (g_bToonRimLight)
		fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);
	//fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	//float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;
	//
	//// 얼굴 빛
	//float FaceTon = vUnion.x;
	//// 눈 쉐도우
	//float EyeShadow = vUnion.y;
	//// 입술
	//float Leaps = vUnion.z;
	//
	//float4 vLeapColor = (1.f, 0.f, 0.f, 1.f);
	//float4 vShadowColor = (1.f, 0.f, 0.f, 1.f);
	//float4 vFaceColor = (1.f, 0.75f, 0.79f, 1.f);
	//
	////if (FaceTon > 0.f)
	////{
	////	Out.vDiffuse += vFaceColor;
	////}
	//
	//if (EyeShadow > 0.f)
	//{
	//	//float4 tmpColor = 
	//	//	float4(
	//	//	vShadowColor.x * EyeShadow, 
	//	//	vShadowColor.y * EyeShadow,
	//	//	vShadowColor.z * EyeShadow,
	//	//	1.f);
	//	float4 tmpColor = float4(1.f * EyeShadow, 0.f, 0.f, 1.f);
	//	Out.vDiffuse.xyz += tmpColor;
	//}
	//
	//if (Leaps > 0.f)
	//{
	//	float4 tmpColor = float4(1.f * Leaps, 0.f, 0.f, 1.f);
	//
	//	Out.vDiffuse.xyz += tmpColor;
	//}

	Out.vDiffuse.xyz = pow(Out.vDiffuse.xyz, 2.2);

	return Out;
}
PS_OUT_ADVENCE PS_Default_DNSUID(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	Out.vDiffuse.w = tex2D(DiffuseSampler, In.vTexUV).w;

	//========================================================================================================================

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float3 vIDValue = tex2D(IDSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;

	// 메탈니스 : 빛 전체의 강도
	float Metalness = vUnion.x;
	// 러프니스 : 정반사의 정도.. 쉽게 말하면 == Specular Power
	float Roughness = vUnion.y;
	// AO
	float AO = vUnion.z;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);

	//========================================================================================================================

	float fMetalness_R = ((Metalness * g_fSpecularPower) + g_fMinSpecular * SpecularIntensity.x) * (vIDValue.x * g_fID_R_Power);
	float fMetalness_G = ((Metalness * g_fSpecularPower) + g_fMinSpecular * SpecularIntensity.x) * (vIDValue.y * g_fID_G_Power);
	float fMetalness_B = ((Metalness * g_fSpecularPower) + g_fMinSpecular * SpecularIntensity.x) * (vIDValue.z * g_fID_B_Power);

	float finalMetalness = fMetalness_R + fMetalness_G + fMetalness_B;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness * g_fRoughnessPower, finalMetalness);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	//fFinalRim = smoothstep(0.f, 0.025f, fFinalRim);

	Out.vEmissive = fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_D(PS_IN In)
{
	// 유니온 아직 안함

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = 1.f;
	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	Out.vNormal = vector(In.N.xyz * 0.5f + 0.5f, 2.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);

	//========================================================================================================================
	//float fRim = 1.f - saturate(dot(In.N, In.vRimDir));
	//
	//float4 fFinalRimColor = g_vRimColor;
	//float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = 0.f;// fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNID(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 vIDValue = tex2D(IDSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	float fMetalness_R = ((1.f * g_fSpecularPower) + g_fMinSpecular) * (vIDValue.x * g_fID_R_Power);
	float fMetalness_G = ((1.f * g_fSpecularPower) + g_fMinSpecular) * (vIDValue.y * g_fID_G_Power);
	float fMetalness_B = ((1.f * g_fSpecularPower) + g_fMinSpecular) * (vIDValue.z * g_fID_B_Power);

	float finalMetalness = fMetalness_R + fMetalness_G + fMetalness_B;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f * g_fRoughnessPower, finalMetalness);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNEID(PS_IN In)
// 디퓨즈 | 노말 | 이미시브
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;
	
	//========================================================================================================================
	
	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	
	//========================================================================================================================
	
	float3 vIDValue = tex2D(IDSampler, In.vTexUV).xyz;
	
	//========================================================================================================================
	
	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;
	
	TanNormal = normalize(TanNormal * 2.f - 1.f);
	
	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);
	
	float3 worldNormal = mul(TBN, TanNormal);
	
	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);
	
	//========================================================================================================================
	
	float fMetalness_R = ((1.f * g_fSpecularPower) + g_fMinSpecular) * (vIDValue.x * g_fID_R_Power);
	float fMetalness_G = ((1.f * g_fSpecularPower) + g_fMinSpecular) * (vIDValue.y * g_fID_G_Power);
	float fMetalness_B = ((1.f * g_fSpecularPower) + g_fMinSpecular) * (vIDValue.z * g_fID_B_Power);

	float finalMetalness = fMetalness_R + fMetalness_G + fMetalness_B;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f * g_fRoughnessPower, finalMetalness);
	
	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = (pow(tex2D(EmissiveSampler, In.vTexUV), 2.2) + fFinalRim);
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNEU(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================
	
	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;

	// 메탈니스 : 빛 전체의 강도
	float Metalness = vUnion.x;
	// 러프니스 : 정반사의 정도.. 쉽게 말하면 == Specular Power
	float Roughness = vUnion.y;
	// AO
	float AO = vUnion.z;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);

	//========================================================================================================================

	float finalMetalness = (Metalness * g_fSpecularPower) + g_fMinSpecular;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness * g_fRoughnessPower, finalMetalness);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = (pow(tex2D(EmissiveSampler, In.vTexUV), 2.2) * g_fEmissivePower) + fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNSEU(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;

	// 메탈니스 : 빛 전체의 강도
	float Metalness = vUnion.x;
	// 러프니스 : 정반사의 정도.. 쉽게 말하면 == Specular Power
	float Roughness = vUnion.y;
	// AO
	float AO = vUnion.z;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);

	//========================================================================================================================

	float finalMetalness = (Metalness * g_fSpecularPower) + g_fMinSpecular * SpecularIntensity.x;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness * g_fRoughnessPower, finalMetalness);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = (pow(tex2D(EmissiveSampler, In.vTexUV), 2.2) * g_fEmissivePower) + fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_DZ(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xyz;

	//========================================================================================================================

	float4 ZValue = tex2D(ZSampler, In.vTexUV).xyzw;
	float3 TanNormal = ZValue.xyz;

	float Roughness = ZValue.w;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//========================================================================================================================

	float finalMetalness = (1.f * g_fSpecularPower) + g_fMinSpecular;
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT PS_MOTIONBLUR(PS_BLURIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5 + 0.5;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5 + 0.5;
	//float2 velocity = pow(abs(a - b), 1 / 3.0)*sign(a - b) * 0.5 + 0.5;
	float2 velocity = (a - b) * 0.5 + 0.5;
	velocity = pow(velocity, 3.0);

	Out.vDiffuse.a = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);
	Out.vVelocity = vector(velocity.xy, Out.vDepth.x, 1.f);

	//Out.vVelocity = In.vVelocity;

	return Out;
}

// 최적화를 위해 main과 따로 나눔
PS_OUT_ADVENCE PS_DISSOLVE(PS_IN In)
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	float4 vColor = (float4)1.f;

	vColor.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	float4 fxColor = tex2D(FXSampler, In.vTexUV);

	if (vColor.a == 0.f)
		clip(-1);

	if (fxColor.r >= g_fFxAlpha)
		vColor.a = 1;
	else
		vColor.a = 0;


	if (fxColor.r >= g_fFxAlpha - 0.01 && fxColor.r <= g_fFxAlpha + 0.01)
		vColor = pow(float4(1.7, 0.95, 0.85, 1), 2.2); //

	//if (fxColor.r >= g_fFxAlpha - 0.02 && fxColor.r <= g_WfFxAlpha + 0.02)
	//	vColor = pow(float4(1.7, 1.7, 1.7, 1), 2.2); //

	//if (fxColor.r >= g_fFxAlpha - 0.03 && fxColor.r <= g_fFxAlpha + 0.03)
	//	vColor = pow(float4(1.7f, 0.8f, 0.2f, 1.f), 2.2); //

	Out.vDiffuse = vColor;

	//====================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 1.f);

	//====================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);

	//====================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	Out.vEmissive.a = 1.f;
	//====================================================================================================

	return Out;
}

float3 lumCoeff = float3(0.2125, 0.7154, 0.0721);
float3 root3 = float3(0.57735, 0.57735, 0.57735);
float3x3 QuaternionToMatrix(float4 quat)
{
	float3 cross = quat.yzx * quat.zxy;
	float3 square = quat.xyz * quat.xyz;
	float3 wimag = quat.w * quat.xyz;

	square = square.xyz + square.yzx;

	float3 diag = 0.5 - square;
	float3 a = (cross + wimag);
	float3 b = (cross - wimag);

	return float3x3(
		2.0 * float3(diag.x, b.z, a.y),
		2.0 * float3(a.z, diag.y, b.x),
		2.0 * float3(b.y, a.x, diag.z));
}
float4 g_vColor;
PS_OUT_ADVENCE PS_Default_DNU_Custom(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = 1.f;
	Out.vDiffuse.xyz = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 vUnion = tex2D(UnionSampler, In.vTexUV).xyz;

	// 메탈니스 : 빛 전체의 강도
	float Metalness = vUnion.x;
	// 러프니스 : 정반사의 정도.. 쉽게 말하면 == Specular Power
	float Roughness = vUnion.y;
	// AO
	float AO = vUnion.z;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, AO);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, Roughness * g_fRoughnessPower, Metalness * g_fSpecularPower);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	// Custom Color
	// ==============================================================================================
	// g_vColor.x = Hue / y = Contrast / z = Brightness / w = Saturation
	// ==============================================================================================
	float4 vChangeColor = Out.vDiffuse;

	float3 intensity;
	float half_angle = 0.5 * radians(g_vColor.x); // Hue is radians of 0 ~ 360 degree
	float4 rot_quat = float4((root3 * sin(half_angle)), cos(half_angle));
	float3x3 rot_Matrix = QuaternionToMatrix(rot_quat);
	
	vChangeColor.rgb = mul(rot_Matrix, vChangeColor.rgb);
	vChangeColor.rgb = (vChangeColor.rgb - 0.5) *(g_vColor.y + 1.0) + 0.5;
	vChangeColor.rgb = vChangeColor.rgb + g_vColor.z;
	intensity = float(dot(vChangeColor.rgb, lumCoeff));
	vChangeColor.rgb = lerp(intensity, vChangeColor.rgb, g_vColor.w);

	Out.vDiffuse = vChangeColor;
	// End ==========================================================================================

	return Out;
}

technique Default_Technique
{
	// 기본적으로 모든 애들은 NonAlpha 임
	//====================================================================================================
	// 0 - Default ( DNS )
	//====================================================================================================
	pass Default_Rendering
	{	
		cullmode = ccw;

		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	//====================================================================================================
	// 1 - Default Alpha
	//====================================================================================================
	pass AlphaBlending
	{
		cullmode = ccw;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = DestAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	//====================================================================================================
	// 2 - MotionBlur
	//====================================================================================================
	pass MotionBlur
	{
		cullmode = ccw;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR();
	}
	//====================================================================================================
	// 3 - Disslove
	//====================================================================================================
	pass Dissolve
	{
		cullmode = ccw;

		//AlphaBlendEnable = true;
		//SrcBlend = SrcAlpha;
		//DestBlend = DestAlpha;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DISSOLVE();
	}
	//====================================================================================================
	// 4 - Default ( D N )
	//====================================================================================================
	pass Default_DN
	{
		cullmode = ccw;

		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DN();
	}
	//====================================================================================================
	// 5 - Default ( D N E )
	//====================================================================================================
	pass Default_DNE
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNE();
	}
	//====================================================================================================
	// 6 - Default ( D N S E )
	//====================================================================================================
	pass Default_DNSE
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSE();
	}

	//====================================================================================================
	// 7 - Default ( D N U )
	//====================================================================================================
	pass Default_DNU
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNU();
	}
	//====================================================================================================
	// 8 - Default ( D N I )
	//====================================================================================================
	pass Default_DNI
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNI();
	}
	//====================================================================================================
	// 9 - Default ( D N T )
	//====================================================================================================
	pass Default_DNT
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNT();
	}
	//====================================================================================================
	// 10 - Default ( D N R )
	//====================================================================================================
	pass Default_DNR
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNR();
	}

	//====================================================================================================
	// 11 - Default ( D N S U )
	//====================================================================================================
	pass Default_DNSU
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSU();
	}

	//====================================================================================================
	// 12 - Default ( D N S U ID )
	//====================================================================================================
	pass Default_DNSUID
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSUID();
	}

	//====================================================================================================
	// 13 - Default ( D N S )
	//====================================================================================================
	pass Default_DNS
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNS();
	}

	//====================================================================================================
	// 14 - Default ( D - NoTest)
	//====================================================================================================
	pass Default_D
	{
		cullmode = none;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_D();
	}

	//====================================================================================================
	// 15 - Default ( D - Test)
	//====================================================================================================
	pass Default_D_Test
	{
		AlphaTestEnable = true;
		Alphafunc = greater;
		Alpharef = 0xc0;
		cullmode = none;

		AlphablendEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		blendop = add;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_D();
	}

	//====================================================================================================
	// 16 - Default ( D N ID )
	//====================================================================================================
	pass Default_DNE
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNID();
	}

	//====================================================================================================
	// 17 - Default ( D N E ID )
	//====================================================================================================
	pass Default_DNEID
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNEID();
	}

	//====================================================================================================
	// 18 - Default ( D N E U )
	//====================================================================================================
	pass Default_DNEU
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNEU();
	}

	//====================================================================================================
	// 19 - Default ( D N S H )
	//====================================================================================================
	pass Default_DNSH
	{
		cullmode = ccw;

		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSH();
	}

	//====================================================================================================
	// 20 - Default ( D N S E U )
	//====================================================================================================
	pass Default_DNSEU
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSEU();
	}

	//====================================================================================================
	// 21 - Default ( D Z(NR) )
	//====================================================================================================
	pass Default_DZ
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DZ();
	}

	//====================================================================================================
	// 22 - Default ( D N S H U )
	//====================================================================================================
	pass Default_DNSHU
	{
		cullmode = ccw;
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSHU();
	}

	//====================================================================================================
	// 23 - For_CustomHair ( D N U )
	//====================================================================================================
	pass Default_DNU_Custom
	{
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNU_Custom();
	}

	//====================================================================================================
	// 24 - Skinned ( D N S U ID )
	//====================================================================================================
	//pass Skinned_DNSUID
	//{
	//	cullmode = ccw;
	//	AlphablendEnable = false;
	//
	//	AlphaTestEnable = true;
	//	AlphaRef = 0;
	//	AlphaFunc = Greater;
	//
	//	VertexShader = compile vs_3_0 VS_MAIN();
	//	PixelShader = compile ps_3_0 PS_Default_DNSUID();
	//}
}

