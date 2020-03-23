


texture		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_BloomTexture;
sampler BloomSampler = sampler_state
{
	texture = g_BloomTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_DepthTexture;
sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

texture		g_DistortionTexture;
sampler		DistortionSampler = sampler_state
{
	texture = g_DistortionTexture;
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
	vector		vColor : COLOR0;
	vector		vBloom : COLOR1;
};

float4 AdjustSaturation(float4 color, float saturation)
{
	// We define gray as the same color we used in the grayscale shader
	float grey = dot(color.xyz, float3(0.3, 0.59, 0.11));

	return lerp(grey, color, saturation);
}

PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse = tex2D(DiffuseSampler, In.vTexUV);
	vector	vShade = tex2D(ShadeSampler, In.vTexUV);
	vector	vSpecular = tex2D(SpecularSampler, In.vTexUV);

	Out.vColor = (vDiffuse + vSpecular) * vShade;

	// Bloom ====================================================================
	float fThreshold = 0.3;	 // 이 기준점보다 밝으면 Bloom 타겟
	Out.vBloom = saturate((Out.vColor - fThreshold) / (1 - fThreshold));
	// Bloom End ====================================================================

	return Out;
}

float offset[3] = { 0.0, 1.3846153846, 3.2307692308 };
float weight[3] = { 1.2270270270, 1.3162162162, 1.0702702703 };


PS_OUT PS_AFTER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// Calc Distortion =========================================
	float2 Trans = In.vTexUV + 0.001f;
	vector	Noise = tex2D(DistortionSampler, Trans);
	//Noise.x -= 0.5f;
	//Noise.y -= 0.5f;

	float2 UV = In.vTexUV + Noise.xy * 0.05f;
	if (Noise.w <= 0)
		UV = In.vTexUV;
	// Calc Distortion End =========================================

	vector	vDiffuse = tex2D(DiffuseSampler, UV);
	//vector  vBloom = tex2D(BloomSampler, UV);
	//
	////vector vBloom;
	//float fBloomSaturation = 1.0; // 채도
	//float fBloomIntensity = 2.3; // 강도 (밝기)
	////float fBlurDistance = 0.002 + ((vDepthInfo.x) * 0.001f);
	////
	////vBloom = tex2D(BloomSampler, float2(In.vTexUV.x + fBlurDistance, In.vTexUV.y + fBlurDistance));
	////vBloom += tex2D(BloomSampler, float2(In.vTexUV.x - fBlurDistance, In.vTexUV.y - fBlurDistance));
	////vBloom += tex2D(BloomSampler, float2(In.vTexUV.x + fBlurDistance, In.vTexUV.y - fBlurDistance));
	////vBloom += tex2D(BloomSampler, float2(In.vTexUV.x - fBlurDistance, In.vTexUV.y + fBlurDistance));
	////// We need to devide the color with the amount of times we added
	////// a color to it, in this case 4, to get the avg. color
	////vBloom = vBloom / 4;
	//
	//vBloom = tex2D(BloomSampler, float2(In.vTexUV) / 1280.0) * weight[0];
	//for (int i = 1; i < 3; i++) {
	//	vBloom +=
	//		tex2D(BloomSampler, (float2(In.vTexUV) + float2(0.0, offset[i])) / 1024.0)
	//		* weight[i];
	//	vBloom +=
	//		tex2D(BloomSampler, (float2(In.vTexUV) - float2(0.0, offset[i])) / 1024.0)
	//		* weight[i];
	//}
	//
	//vBloom = AdjustSaturation(vBloom, fBloomSaturation) * fBloomIntensity;
	//
	//vDiffuse *= (1 - saturate(vBloom));
	//vDiffuse += vBloom;
	//
	Out.vColor = vDiffuse;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass After_Rendering
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_AFTER();
	}

}

