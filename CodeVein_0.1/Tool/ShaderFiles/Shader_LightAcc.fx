

// ��
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

// ���
texture		g_NormalTexture;

sampler NormalSampler = sampler_state
{
	texture = g_NormalTexture;
};

// ����
texture		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// SSAO ���� �ؽ��� (������ �븻�̹��� ���)
texture		g_SSAOTexture;
sampler SSAOSampler = sampler_state
{
	texture = g_SSAOTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector		vShade : COLOR0;
	vector		vSpecular : COLOR1;
	vector		vSSAO : COLOR2;
};

float g_sample_rad = 2.f;		// ���ø� �ݰ�
float g_intensity = 1.1f;		// ao ����
float g_scale = 0.3f;			// ���� �Ÿ�
float g_bias = 0.3f;			// �ʺ� ����
float3 getPosition(in float3 vDepth, in float2 uv)
{
	float		fViewZ = vDepth.g * 500.f;
	vector		vWorldPos;
	vWorldPos.x = uv.x * 2.f - 1.f;
	vWorldPos.y = uv.y * -2.f + 1.f;
	vWorldPos.z = vDepth.x;
	vWorldPos.w = 1.f;
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_matProjInv);
	//vWorldPos = mul(vWorldPos, g_matViewInv);

	return vWorldPos.xyz;
}

float2 getRandom(in float2 uv)
{
	float2 g_screen_size = { 1280.f, 720.f };
	float2 random_size = { 64.f, 64.f }; // ���÷� �ؽ��� ������
	return normalize(tex2D(SSAOSampler, g_screen_size * uv / random_size).xy * 2.0f - 1.0f);
}

float doAmbientOcclusion(in float3 vDepth, in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
	float3 diff = getPosition(vDepth, tcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff)*g_scale;
	return max(0.0, dot(cnorm, v) - g_bias)*(1.0 / (1.0 + d))*g_intensity;
}

float Get_SSAO(in float3 vNormal, in float3 vDepth, in float2 uv)
{
	const float2 vec[4] = { float2(1,0),float2(-1,0), float2(0,1),float2(0,-1) };
	float3 p = getPosition(vDepth, uv);
	float3 n = vNormal;
	float2 rand = getRandom(uv);
	float ao = 0.0f;
	float rad = g_sample_rad / p.z;

	//**SSAO Calculation**// 
	//int iterations = 4;
	int iterations = lerp(6.0, 2.0, p.z / 500.f);
	for (int j = 0; j < iterations; ++j)
	{
		float2 coord1 = reflect(vec[j], rand)*rad;
		float2 coord2 = float2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707);

		ao += doAmbientOcclusion(vDepth, uv, coord1 * 0.25, p, n);
		ao += doAmbientOcclusion(vDepth, uv, coord2 * 0.5, p, n);
		ao += doAmbientOcclusion(vDepth, uv, coord1 * 0.75, p, n);
		ao += doAmbientOcclusion(vDepth, uv, coord2, p, n);
	}

	ao /= (float)iterations * 4.0;

	return ao;
}

PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector		vDepthInfo = tex2D(DepthSampler, In.vTexUV);

	float		fViewZ = vDepthInfo.g * 500.f;

	// 0 ~ 1
	// -1 ~ 1
	vector		vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + saturate(g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vWorldPos;

	// 1. ���������̽� ���� ��ġ�� ���غ���. ( ������ġ * ���� * �� * ���� * 1.f / w)
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthInfo.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;

	// 2. ��( ������ġ * ���� * �� )
	vWorldPos = mul(vWorldPos, g_matProjInv);

	// 3. ����( ������ġ * ����)
	vWorldPos = mul(vWorldPos, g_matViewInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f) * (g_vLightSpecular * g_vMtrlSpecular);
	Out.vSpecular.a = 0.f;

	// RimLight ====================================================================
	float fRimWidth = 1.5f;
	vector vCamPos = normalize(g_vCamPosition - vWorldPos);
	float fRim = smoothstep((1.f - fRimWidth), (1.f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));
	//float fRim = smoothstep(max(1.f - fRimWidth + vDepthInfo.x, 0.5f), max(1.f - fRimWidth + vDepthInfo.x, 0.9f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));
	float4 rc = g_vLightDiffuse;
	Out.vShade += pow(fRim, 2.f) * rc;
	// RimLight End ====================================================================


	// SSAO ====================================================================
	vNormal = mul(vNormal, g_matProjInv);
	float ao = Get_SSAO(vNormal.xyz, vDepthInfo.xyz, In.vTexUV);
	Out.vSSAO = float4(ao, 0, 0, 1);
	// SSAO End ====================================================================

	return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector		vDepthInfo = tex2D(DepthSampler, In.vTexUV);

	float		fViewZ = vDepthInfo.g * 500.f;

	vector		vWorldPos;

	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthInfo.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_matProjInv);
	vWorldPos = mul(vWorldPos, g_matViewInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fLength = length(vLightDir);
	float		fAtt = saturate((g_fRange - fLength) / g_fRange);

	vector		vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = fAtt * g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + saturate(g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(vLightDir), vNormal);
	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = fAtt * g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f) * (g_vLightSpecular * g_vMtrlSpecular);
	Out.vSpecular.a = 0.f;

	// RimLight ====================================================================
	float fRimWidth = 1.5f;
	vector vCamPos = normalize(g_vCamPosition - vWorldPos);
	float fRim = smoothstep((1.f - fRimWidth), (1.f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));
	float4 rc = g_vLightDiffuse;
	Out.vShade += pow(fRim, 2.f) * rc;
	// RimLight End ====================================================================

	// SSAO ====================================================================
	vNormal = mul(vNormal, g_matProjInv);
	float ao = Get_SSAO(vNormal.xyz, vDepthInfo.xyz, In.vTexUV);
	Out.vSSAO = float4(ao, 0, 0, 1);
	// SSAO End ====================================================================

	return Out;
}

technique Default_Technique
{
	pass Directional
	{
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_DIRECTIONAL();
	}

	pass Point
	{
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN_POINT();
	}

}

