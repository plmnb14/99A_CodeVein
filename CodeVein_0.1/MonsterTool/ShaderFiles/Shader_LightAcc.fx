

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

// 노멀 for Rim
texture		g_RimNormalTexture;

sampler RimNormalSampler = sampler_state
{
	texture = g_RimNormalTexture;
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
};

PS_OUT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector		vDepthInfo = tex2D(DepthSampler, In.vTexUV);

	float		fViewZ = vDepthInfo.g * 500.f;

	float		Roughness = vDepthInfo.z;
	float		Metalness = vDepthInfo.w;
	float		AO = vNormalInfo.w;

	// 0 ~ 1
	// -1 ~ 1
	vector		vNormal = vector((vNormalInfo.xyz * 2.f - 1.f), 0.f) ;

	float2		vHeightValue = tex2D(RimNormalSampler, In.vTexUV).xy;

	Out.vShade = g_vLightDiffuse * saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + saturate(g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 1.f;

	//Out.vShade.x = smoothstep(0.33, 0.67, Out.vShade.x);
	//Out.vShade.x = smoothstep(0.33, 0.67, Out.vShade.y);
	//Out.vShade.x = smoothstep(0.33, 0.67, Out.vShade.z);

	if (vHeightValue.x > 0.0001f)
	{
		Out.vShade.xyz = ceil(Out.vShade.xyz * 2.f) / 2.f;

		if (vHeightValue.y > 0.f)
		{
			Out.vShade.xyz += 0.5f;
			Out.vShade.xyz = saturate(Out.vShade.xyz);
		}

		//else
		//{
		//	Out.vShade.xyz = ceil(Out.vShade.xyz * 2.f) / 2.f;
		//}

		//if (Out.vShade.x > 0.1f)
		//{
			//Out.vShade.xyz = 1.f;
			//Out.vShade.xyz = ceil(Out.vShade.xyz * 10.f) / 10.f;
			//
			//if (vHeightValue.y > Out.vShade.x)
			//{
			//	Out.vShade.xyz = 1.f;
			//}
		//}
		//Out.vShade.xyz = ceil(Out.vShade.xyz * 2.f) / 2.f;
		//Out.vShade.xyz = 1.f; 
		// ceil(Out.vShade.xyz * 2.f) / 2.f;
		//Out.vShade.xyz = HeightValue;
	}

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
	
	//float4 lightingPosition = mul(vWorldPos, g_matLightView);
	//lightingPosition = mul(lightingPosition, g_matLightProj);
	//
	//float2 vUV;
	//vUV = lightingPosition.xy / lightingPosition.w;
	//vUV.y = -vUV.y;
	//vUV.y = vUV * 0.5f + 0.5f;
	//
	//float fShadow = tex2D(ShadowMapSampler, vUV).x;
	//float DepthBias = 0.001f;
	//
	//float fDepth = (lightingPosition.z / lightingPosition.w);
	//
	//if (fDepth > fShadow + DepthBias)
	//{
	//	Out.vShade.rgb *= 0.2f;
	//
	//	return Out;
	//}

	// Shadow End ====================================================================

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = (g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 15.f * Roughness)) * Metalness;
	Out.vSpecular.a = AO;

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


	float4		vSpecularIntensity = float4(vDepthInfo.z, vDepthInfo.w, vNormalInfo.w, 1.f);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fLength = length(vLightDir);
	float		fAtt = saturate((g_fRange - fLength) / g_fRange);

	vector		vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	vector		vReflect = reflect(normalize(vLightDir), vNormal);


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

	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = fAtt * g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 20.f); // *vSpecularIntensity.y;
	Out.vSpecular.a = 0.f;

	// RimLight ====================================================================

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

