

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
	vector		vRim : COLOR2;
};



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

	// 1. 투영스페이스 상의 위치를 구해보자. ( 로컬위치 * 월드 * 뷰 * 투영 * 1.f / w)
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthInfo.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;

	// 2. 뷰( 로컬위치 * 월드 * 뷰 )
	vWorldPos = mul(vWorldPos, g_matProjInv);

	// 3. 월드( 로컬위치 * 월드)
	vWorldPos = mul(vWorldPos, g_matViewInv);

	vector		vLook = vWorldPos - g_vCamPosition;

	// RimLight ====================================================================
	float fRimWidth = 1.5f;

	vector vCamPos = normalize(g_vCamPosition - vWorldPos);
	float fRim = smoothstep((1.f - fRimWidth), (1.f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));

	//float fRim = smoothstep(max(1.f - fRimWidth + vDepthInfo.x, 0.5f), max(1.f - fRimWidth + vDepthInfo.x, 0.9f), (vDepthInfo.x) - saturate(abs(dot(vNormal, vCamPos))));
	//float fRim = smoothstep((1.f - fRimWidth), (1.f), (vDepthInfo.x) - max(0, (dot(vNormal, vCamPos))));

	float4 rc = g_vLightDiffuse;
	//Out.vRim = pow(fRim, 2.f) * rc;
	Out.vShade += pow(fRim, 2.f) * rc;
	// RimLight End ====================================================================
	
	Out.vSpecular = g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f) * (g_vLightSpecular * g_vMtrlSpecular * vNormalInfo.w);
	Out.vSpecular.a = 0.f;

	return Out;
}

PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vNormalInfo = tex2D(NormalSampler, In.vTexUV);
	vector		vDepthInfo = tex2D(DepthSampler, In.vTexUV);

	float		fViewZ = vDepthInfo.g * 500.f;


	vector		vWorldPos;

	// 1. 투영스페이스 상의 위치를 구해보자. ( 로컬위치 * 월드 * 뷰 * 투영 * 1.f / w)
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthInfo.x;
	vWorldPos.w = 1.f;

	vWorldPos = vWorldPos * fViewZ;

	// 2. 뷰( 로컬위치 * 월드 * 뷰 )
	vWorldPos = mul(vWorldPos, g_matProjInv);

	// 3. 월드( 로컬위치 * 월드)
	vWorldPos = mul(vWorldPos, g_matViewInv);


	vector		vLightDir = vWorldPos - g_vLightPos;

	float		fLength = length(vLightDir);

	float		fAtt = saturate((g_fRange - fLength) / g_fRange);



	// 0 ~ 1
	// -1 ~ 1
	vector		vNormal = vector(vNormalInfo.xyz * 2.f - 1.f, 0.f);

	Out.vShade = fAtt * g_vLightDiffuse * saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + saturate(g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 1.f;

	// RimLight ====================================================================
	float fRimWidth = 1.5f;
	vector vCamPos = g_vCamPosition - vWorldPos;
	float fRim = smoothstep(max(1.f - fRimWidth + vDepthInfo.x, 0.5f), max(0.1f + vDepthInfo.x - fRimWidth, 0.7f), vDepthInfo.x - saturate(abs(dot(vNormal, vCamPos))));
	float4 rc = g_vLightDiffuse;
	Out.vRim = pow(fRim, 2.f) * rc;
	// RimLight End ====================================================================

	vector		vReflect = reflect(normalize(vLightDir), vNormal);


	vector		vLook = vWorldPos - g_vCamPosition;

	Out.vSpecular = fAtt * g_vLightDiffuse * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f) * (g_vLightSpecular * g_vMtrlSpecular);
	Out.vSpecular.a = 0.f;

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

