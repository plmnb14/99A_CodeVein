
vector		g_vCamPosition;
matrix		g_matProjInv;
matrix		g_matViewInv;

// 노멀
texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

// 뎁스
texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// SSAO 랜덤 텍스쳐 (임의의 노말이미지 사용)
texture		g_RandomTexture;
sampler RandomSampler = sampler_state
{
	texture = g_RandomTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

//  블러 처리용 SSAO 텍스쳐
texture		g_SSAOTexture;
sampler SSAOSampler = sampler_state
{
	texture = g_SSAOTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

#define TEXCX 1.f / 1280
#define TEXCY 1.f / 720

float g_fBlurOffsetX[4] = { TEXCX * -1.5f, TEXCX * -0.5f, TEXCX * 0.5f, TEXCX * 1.5f };
float g_fBlurOffsetY[4] = { TEXCY * -1.5f, TEXCY * -0.5f, TEXCY * 0.5f, TEXCY * 1.5f };


struct PS_IN
{
	float2		vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector		vSSAO : COLOR0;
};

float g_fSampleRad  = 4.0f;		// 샘플링 반경
float g_fIntensity	= 7.0f;		// ao 강도
float g_fScale		= 1.0f;		// 사이 거리
float g_fBias		= 0.6f;		// 너비 제어

//float g_fSampleRad = 0.1f;		// 샘플링 반경
//float g_fIntensity = 0.35f;		// ao 강도
//float g_fScale = 1.f;		// 사이 거리
//float g_fBias = 0.01f;		// 너비 제어

float2 Generate_Random(in float2 _vUV)
{
	float2 vScreenXY = float2(1280.f, 720.f);
	float2 vRandomXY = (float2)64.f;

	return normalize(tex2D(RandomSampler, vScreenXY * _vUV / vRandomXY).xy * 2.f - 1.f);
}

float3 Generate_Position(in float2 _vUV)
{
	float3		vDepthInfo = tex2D(DepthSampler, _vUV).xyz;

	float		fViewZ = vDepthInfo.y * 500.f;

	float4		vWorldPos;

	vWorldPos.x = _vUV.x * 2.f - 1.f;
	vWorldPos.y = _vUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthInfo.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_matProjInv);
	vWorldPos = mul(vWorldPos, g_matViewInv);

	return vWorldPos.xyz;
}

float3 Generate_Normal(in float2 _vUV)
{
	return normalize(tex2D(NormalSampler, _vUV).xyz * 2.f - 1.f);
}

float Generate_AmbientOcclusion(in float2 uv, in float2 tcoord, in float3 p, in float3 cnorm)
{
	float3	diff = Generate_Position(tcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * g_fScale;

	return max(0.f, dot(cnorm, v) - g_fBias)*(1.f / (1.f + d)) * g_fIntensity;
}

PS_OUT PS_SSAO(PS_IN In)
{
	// 뷰시점 거리 체크만 안넣어놨어요.
	// 시간이 없어서 추가해야됨.

	PS_OUT			Out = (PS_OUT)0;

	Out.vSSAO.rgb = 1.f;

	float2 vVec[4] = { float2(1.f,0.f) , float2(-1.f,0.f), float2(0.f,1.f), float2(0.f,-1.f) };

	float3 p	= Generate_Position(In.vTexUV);
	float3 n	= Generate_Normal(In.vTexUV);
	float2 rand = Generate_Random(In.vTexUV);

	float ao = 0.f;
	float rad = g_fSampleRad / p.z;

	//int	iterations = lerp(6.0 , 2.0, p.z / 500.f);
	int	iterations = 4;
	for (int i = 0; i < iterations; ++i)
	{
		float2 coord1 = reflect(vVec[i], rand) * rad;
		float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);

		ao += Generate_AmbientOcclusion(In.vTexUV, coord1 * 0.25, p, n);
		ao += Generate_AmbientOcclusion(In.vTexUV, coord2 * 0.5, p, n);
		ao += Generate_AmbientOcclusion(In.vTexUV, coord1 * 0.75, p, n);
		ao += Generate_AmbientOcclusion(In.vTexUV, coord2, p, n);
	}

	ao /= (float)iterations * 4.f;

	//Out.vSSAO = ao;
	Out.vSSAO	= 1.f - ao;
	Out.vSSAO.a = 1.f;

	return Out;
}

PS_OUT PS_SSAO_Blur (PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	for (int i = 0; i < 4; i++)
	{
		Out.vSSAO.xyz += tex2D(SSAOSampler, In.vTexUV + float2(g_fBlurOffsetX[i], g_fBlurOffsetY[i])).xyz;
	}

	Out.vSSAO.xyz *= 0.25f;

	return Out;
}

technique Default_Technique
{
	pass SSAO
	{
		PixelShader = compile ps_3_0 PS_SSAO();
	}

	pass SSAO_Blur
	{
		PixelShader = compile ps_3_0 PS_SSAO_Blur();
	}

}

