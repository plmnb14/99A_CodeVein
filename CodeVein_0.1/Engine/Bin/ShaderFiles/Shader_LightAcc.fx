

// 빛
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

matrix		g_matLightView;
matrix		g_matLightProj;

matrix		g_LightVP_Close;

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
texture		g_SSAOTexture;
sampler SSAOSampler = sampler_state
{
	texture = g_SSAOTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_ShadowMapTexture;
sampler ShadowMapSampler = sampler_state
{
	texture = g_ShadowMapTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
	
	//addressU = border;
	//addressV = border;
	//
	//BorderColor = float4(1.0f, 1.0f, 1.0f, 0.0f);

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

float g_sample_rad = 2.f;		// 샘플링 반경
float g_intensity = 1.1f;		// ao 강도
float g_scale = 0.3f;			// 사이 거리
float g_bias = 0.3f;			// 너비 제어
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
	float2 random_size = { 64.f, 64.f }; // 샘플러 텍스쳐 사이즈
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

	float4		vSpecularIntensity = float4(vDepthInfo.z, vDepthInfo.w, vNormalInfo.w , 1.f);

	// 0 ~ 1
	// -1 ~ 1
	vector		vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + saturate(g_vLightAmbient * g_vMtrlAmbient));
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);
	vector		vWorldPos , vProjPos;

	// 1. 투영스페이스 상의 위치를 구해보자. ( 로컬위치 * 월드 * 뷰 * 투영 * 1.f / w)
	vProjPos.x = In.vTexUV.x * 2.f - 1.f;
	vProjPos.y = In.vTexUV.y * -2.f + 1.f;
	vProjPos.z = vDepthInfo.x;
	vProjPos.w = 1.f;

	vWorldPos = vProjPos * fViewZ;

	// 2. 뷰( 로컬위치 * 월드 * 뷰 )
	vWorldPos = mul(vProjPos, g_matProjInv);

	// 3. 월드( 로컬위치 * 월드)
	vWorldPos = mul(vWorldPos, g_matViewInv);


	// Shadow ====================================================================
	
	//float fShadow = tex2D(ShadowMapSampler, In.vTexUV).x;

	//float4 lightingPosition = mul(vWorldPos, g_matLightView);
	//lightingPosition = mul(lightingPosition, g_matLightProj);
	//
	//float2 vUV;
	//vUV = lightingPosition.xy / lightingPosition.w;
	//vUV.y = -vUV.y;
	//vUV.y = vUV * 0.5f + 0.5f;
	//
	//float fShadow = tex2D(ShadowMapSampler, vUV).x;
	//
	//float fDepth = (lightingPosition.z / lightingPosition.w);
	//
	//if (fDepth > fShadow + Bias)
	//{
	//	Out.vShade.rgb *= 0.2f;
	//
	//	return Out;
	//}

	//if (fShadow < 1.f)
	//{
	//}

	// Shadow End ====================================================================

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 20.f) * (vSpecularIntensity.x);
	Out.vSpecular.a = 0.f;

	// RimLight ====================================================================
	float fRimWidth = 1.5f;
	vector vCamPos = normalize(g_vCamPosition - vWorldPos);
	float fRim = smoothstep((1.f - fRimWidth), (1.f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));
	//float fRim = smoothstep(max(1.f - fRimWidth + vDepthInfo.x, 0.5f), max(1.f - fRimWidth + vDepthInfo.x, 0.9f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));
	float4 rc = g_vLightDiffuse;
	Out.vShade += (pow(fRim, 2.f) * rc);
	// RimLight End ====================================================================


	// SSAO ====================================================================
	vNormal = mul(vNormal, g_matProjInv);
	float ao = Get_SSAO(vNormal.xyz, vDepthInfo.xyz, In.vTexUV);
	Out.vSSAO = float4(ao, 0, 0, 1);
	// SSAO End ====================================================================

	//Out.vShade.rgb *= fShadow;

	//Out.vShade.rgb *= fShadow;

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


	// Shadow ====================================================================

	//float4 lightingPosition = mul(vWorldPos, g_LightVP_Close);
	//
	//float2 ShadowTexCoord = 0.5 * lightingPosition.xy /
	//	lightingPosition.w + float2(0.5, 0.5);
	//ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
	//
	//float fShadow = tex2D(ShadowMapSampler, ShadowTexCoord).x;
	//
	//float DepthBias = 0.001f;
	//float ourdepth = (lightingPosition.z / lightingPosition.w) - DepthBias;
	//
	////
	//if (fShadow <= ourdepth)
	//{
	//	Out.vShade.rgb *= 0.2f;
	//
	//	return Out;
	//}

	//float4 lightingPosition = mul(vWorldPos, g_LightVP_Close); 
	//
	//float2 ShadowTexCoord = 0.5 * lightingPosition.xy /
	//	lightingPosition.w + float2(0.5, 0.5);
	//ShadowTexCoord.y = -ShadowTexCoord.y;
	//
	//float4 fShadow = tex2D(ShadowMapSampler, ShadowTexCoord.xy);
	////
	//if (fShadow.x <= 0.6f)
	//{
	//	Out.vShade.rgb *= 0.2f;
	//
	//	return Out;
	//}

	//float4 fShadow = tex2D(ShadowMapSampler, In.vTexUV.xy); 
	////
	//if (fShadow.a <= 0.6f)
	//{
	//	Out.vShade.rgb *= 0.2f;
	//
	//	return Out;
	//}

	// Shadow ====================================================================



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

