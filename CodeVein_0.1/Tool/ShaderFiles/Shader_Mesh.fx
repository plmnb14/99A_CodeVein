

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	float2 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xy;

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 0.f);
	//  스펙큘러 텍스쳐 추가되면 변경예정

	//========================================================================================================================


	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 0.f);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 0.f);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 0.f);

	//========================================================================================================================
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);
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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, SpecularIntensity.y);

	//========================================================================================================================
	float fRim = 1.f - saturate(dot(In.N, In.vRimDir));

	float4 fFinalRimColor = g_vRimColor;
	float4 fFinalRim = (pow(fRim, g_fRimPower) * fFinalRimColor) * g_fRimAlpha;

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNE(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	float3 TanNormal = tex2D(NormalSampler, In.vTexUV).xyz;

	TanNormal = normalize(TanNormal * 2.f - 1.f);

	float3x3 TBN = float3x3(normalize(In.T), normalize(In.B), normalize(In.N));
	TBN = transpose(TBN);

	float3 worldNormal = mul(TBN, TanNormal);

	Out.vNormal = vector(worldNormal.xyz * 0.5f + 0.5f, 0.f);

	//========================================================================================================================

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

PS_OUT_ADVENCE PS_Default_DNU(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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
	return Out;
}

PS_OUT_ADVENCE PS_Default_DNI(PS_IN In)
{
	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	float2 SpecularIntensity = tex2D(SpecularSampler, In.vTexUV).xy;

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

PS_OUT_ADVENCE PS_Default_DNSU(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
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

	Out.vEmissive = fFinalRim;
	//========================================================================================================================

	return Out;
}

PS_OUT_ADVENCE PS_Default_DNSUID(PS_IN In)
{
	// 디퓨즈 | 노말 | 이미시브

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	//========================================================================================================================

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

	Out.vEmissive = fFinalRim;
	//========================================================================================================================
	return Out;
}

PS_OUT_ADVENCE PS_Default_D(PS_IN In)
{
	// 유니온 아직 안함

	PS_OUT_ADVENCE			Out = (PS_OUT_ADVENCE)0;

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

	//========================================================================================================================

	Out.vNormal = vector(In.N.xyz * 0.5f + 0.5f, 1.f);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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
	
	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	
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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);

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

PS_OUT PS_MOTIONBLUR(PS_BLURIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5 + 0.5;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5 + 0.5;
	//float2 velocity = pow(abs(a - b), 1 / 3.0)*sign(a - b) * 0.5 + 0.5;
	float2 velocity = (a - b) * 0.5 + 0.5;
	velocity = pow(velocity, 3.0);

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
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

	float4 vColor = (float4)0.f;

	vColor = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
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


technique Default_Technique
{
	// 기본적으로 모든 애들은 NonAlpha 임
	//====================================================================================================
	// 0 - Default ( DNS )
	//====================================================================================================
	pass Default_Rendering
	{
		// RenderState		
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
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		cullmode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DISSOLVE();
	}
	//====================================================================================================
	// 4 - Default ( D N )
	//====================================================================================================
	pass Default_DN
	{
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
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSU();
	}

	//====================================================================================================
	// 12 - Default ( D N S E ID )
	//====================================================================================================
	pass Default_DNSEID
	{
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

		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_D();
	}

	//====================================================================================================
	// 16 - Default ( D N ID )
	//====================================================================================================
	pass Default_DNE
	{
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
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNEU();
	}

	//====================================================================================================
	// 19 - ForFlag ( D )
	//====================================================================================================
	//pass Default_DNEU
	//{
	//	AlphablendEnable = false;
	//
	//	AlphaTestEnable = true;
	//	AlphaRef = 0;
	//	AlphaFunc = Greater;
	//
	//	VertexShader = compile vs_3_0 VS_Flag();
	//	PixelShader = compile ps_3_0 PS_Flag();
	//}

	//====================================================================================================
	// 19 - Default ( D N S H )
	//====================================================================================================
	pass Default_DNSH
	{
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSH();
	}

	//====================================================================================================
	// 19 - Default ( D N S E U )
	//====================================================================================================
	pass Default_DNSEU
	{
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_Default_DNSEU();
	}
}

