int g_iToneIndex = 0;
float g_fToneGradient= 0.f;

texture		g_DiffuseTexture;
sampler DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
	addressU = clamp;
	addressV = clamp;
};

texture		g_ShadeTexture;
sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_SpecularTexture;
sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_EmissiveTexture;
sampler EmissiveSampler = sampler_state
{
	texture = g_EmissiveTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_BloomTexture;
sampler BloomSampler = sampler_state
{
	texture = g_BloomTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_DepthTexture;
sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_DistortionTexture;
sampler	DistortionSampler = sampler_state
{
	texture = g_DistortionTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_SSAOTexture;
sampler	SSAOSampler = sampler_state
{
	texture = g_SSAOTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_RimTexture;
sampler	RimSampler = sampler_state
{
	texture = g_RimTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

texture		g_GradingTexture;
sampler	GradingSampler = sampler_state
{
	texture = g_GradingTexture;
	minfilter = linear;
	magfilter = linear;
	MipFilter = NONE;
	MaxMipLevel = 0;
	MipMapLodBias = 0;

	addressU = clamp;
	addressV = clamp;
};

texture		g_FogColorTexture;
sampler	FogColorSampler = sampler_state
{
	texture = g_FogColorTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = clamp;
	addressV = clamp;
};

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse	= tex2D(DiffuseSampler, In.vTexUV);
	vector	vEmissive	= tex2D(EmissiveSampler, In.vTexUV);
	vector	vShade		= tex2D(ShadeSampler, In.vTexUV);
	vector	vSpecular	= tex2D(SpecularSampler, In.vTexUV);
	vector	vSSAO		= tex2D(SSAOSampler, In.vTexUV);


	float3 vFinalShade = vShade.r * vSSAO.r;

	Out.vColor = ((vDiffuse + vSpecular) * float4(vFinalShade, 1.f)) + vEmissive;

	return Out;
}

PS_OUT PS_TONEMAPPING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
	Out.vColor += tex2D(BloomSampler, In.vTexUV);

	// For Test
	if (0 == g_iToneIndex)
	{
		// DX Sample ========================================================================
		float Luminance = 1.08f;
		static const float fMiddleGray = 0.18f;
		static const float fWhiteCutoff = 0.9f;
		
		float3 Color = pow(Out.vColor.xyz, 1.f / 2.2f) * fMiddleGray / (Luminance + 0.001f);
		Color *= (1.f + (Color / (fWhiteCutoff * fWhiteCutoff)));
		Color /= (1.f + Color);
		Out.vColor = float4(pow(Color, 1.f / 2.2f), 1.f);
	}
	else if (1 == g_iToneIndex)
	{
		//ToneMapACES ========================================================================
		const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
		Out.vColor = saturate((Out.vColor * (A * Out.vColor + B)) / (Out.vColor * (C * Out.vColor + D) + E));
	}
	else if (2 == g_iToneIndex)
	{
		// EA studio ========================================================================
		float3 Color = Out.vColor.xyz;
		float3 x = max(0.f, Color - 0.004);
		Color = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
		//Color = pow(Color, 1.0 / 2.2);
		Out.vColor = float4(Color, 1.f);
	}
	else if (3 == g_iToneIndex)
	{
		// reinhardTone ========================================================================
		float3 mapped = Out.vColor.xyz / (Out.vColor.xyz + float3(1.0, 1.0, 1.0));
		mapped = pow(mapped, float3((1.0 / 2.2), (1.0 / 2.2), (1.0 / 2.2)));
		Out.vColor = float4(mapped, 1.f);
	}
	else if (4 == g_iToneIndex)
	{
		//// Uncharted2Tonemap ========================================================================
		float A = 0.15;
		float B = 0.50;
		float C = 0.10;
		float D = 0.20;
		float E = 0.02;
		float F = 0.30;

		float3 x = Out.vColor.rgb;
		float3 Color = ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
		Out.vColor = float4(Color, 1.f);
	}

	// MONO =========================================================
	float3 Color = Out.vColor.rgb;
	Color.rgb = lerp(Color, dot(Color.rgb, float3(0.3, 0.59, 0.11)), g_fToneGradient);
	Out.vColor = float4(Color, 1.f);
	
	return Out;
}

float PixelKernel[13] =
{
	-6,
	-5,
	-4,
	-3,
	-2,
	-1,
	0,
	1,
	2,
	3,
	4,
	5,
	6,
};

float BlurWeights[13] =
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

//https://copynull.tistory.com/287
PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse = tex2D(DiffuseSampler, In.vTexUV);
	//vector	vSSAO = pow(tex2D(SSAOSampler, In.vTexUV), 2.2);

	Out.vColor = vDiffuse;
	return Out;

	float4 color = 0;
	float2 samp = In.vTexUV;
	float pixelWidthX = 1.f / 1280.f;
	float pixelWidthY = 1.f / 720.f;

	for (int i = 0; i < 13; i++)
	{
		samp.x = In.vTexUV.x + PixelKernel[i] * pixelWidthX;
		samp.y = In.vTexUV.y + PixelKernel[i] * pixelWidthY;
		color += tex2D(DiffuseSampler, samp) * BlurWeights[i] * 1.3f;
		//color -= tex2D(SSAOSampler, samp).x * BlurWeights[i] * 1.3f;
	}

	color.a = 1;
	Out.vColor = color;

	return Out;
}

PS_OUT PS_BLURH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	//(1.0, 0.0) -> horizontal blur
	float2 dir = float2(1.0, 0.0);
	
	const float offset[] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
	const float weight[] = {
		0.2270270270, 0.1945945946, 0.1216216216,
		0.0540540541, 0.0162162162
	};
	float3 ppColour = 0;
	float3 FragmentColor = float3(0.0f, 0.0f, 0.0f);
	
	float hstep = dir.x;
	float vstep = dir.y;
	
	float pixelWidthX = 1.f / 1280.f;
	float pixelWidthY = 1.f / 720.f;

	float fBlurColor = 1.3f; // Ŭ ���� �� ���� ������

	for (int i = 1; i < 5; i++) {
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) + float2(hstep*offset[i], vstep*offset[i]) * pixelWidthX)).xyz * weight[i] * fBlurColor;
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) - float2(hstep*offset[i], vstep*offset[i]) * pixelWidthX)).xyz * weight[i] * fBlurColor;
	}

	ppColour += FragmentColor;
	
	Out.vColor = float4(ppColour, 1.0);
	
	return Out;
}

PS_OUT PS_BLURV(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	//(0.0, 1.0) -> vertical blur
	float2 dir = float2(0.0, 1.0);
	
	const float offset[] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
	const float weight[] = {
		0.2270270270, 0.1945945946, 0.1216216216,
		0.0540540541, 0.0162162162
	};
	float3 ppColour = 0;
	float3 FragmentColor = float3(0.0f, 0.0f, 0.0f);
	
	float hstep = dir.x;
	float vstep = dir.y;

	float pixelWidthX = 1.f / 1280.f;
	float pixelWidthY = 1.f / 720.f;

	float fBlurColor = 1.3f; // Ŭ ���� �� ���� ������

	for (int i = 1; i < 5; i++) {
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) + float2(hstep*offset[i], vstep*offset[i]) * pixelWidthY)).xyz * weight[i] * fBlurColor;
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) - float2(hstep*offset[i], vstep*offset[i]) * pixelWidthY)).xyz * weight[i] * fBlurColor;
	}
	ppColour += FragmentColor;
	
	Out.vColor = float4(ppColour, 1.0);

	return Out;
}

float g_Focus_DOF;
float g_Range_DOF;

float g_FogDestiny;

float MAXCOLOR = 31.0;
float COLORS = 32.0;
float WIDTH = 1024.0;
float HEIGHT = 32.0;
PS_OUT PS_AFTER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// ============================================
	// Calc Distortion ============================
	float2 Trans = In.vTexUV;// +0.001f;
	vector	Noise = tex2D(DistortionSampler, Trans);
	//Noise.xy *= 1.f - (Noise.x + Noise.y);
	Noise.xy *= Noise.w; // ���İ�

	float fPower = Noise.z;
	//fPower *= Noise.w;

	float2 UV = In.vTexUV + (Noise.xy * fPower);
	if (Noise.w <= 0)
		UV = In.vTexUV;
	// Calc Distortion End ========================
	// ============================================

	vector	vDiffuse = tex2D(DiffuseSampler, UV);
	Out.vColor = pow(vDiffuse, 1 / 2.2);
	//Out.vColor = vDiffuse;

	// ============================================
	// DOF =========================================

	vector vDepth = tex2D(DepthSampler, UV);
	float PixelCameraZ = vDepth.y * 500.f;
	vector	vBlur = tex2D(ShadeSampler, UV);
	Out.vColor = lerp(Out.vColor, vBlur, saturate(g_Range_DOF * abs(g_Focus_DOF - PixelCameraZ)));

	// ===========================================================
	// Color Grading =============================================
	float4 px = Out.vColor;
	px = saturate(px);

	float cell = px.b * MAXCOLOR;
	
	float cell_l = floor(cell);
	float cell_h = ceil(cell);
	
	float half_px_x = 0.5 / WIDTH;
	float half_px_y = 0.5 / HEIGHT;
	float r_offset = half_px_x + px.r / COLORS * (MAXCOLOR / COLORS);
	float g_offset = half_px_y + px.g * (MAXCOLOR / COLORS);
	
	float2 lut_pos_l = float2(cell_l / COLORS + r_offset, g_offset);
	float2 lut_pos_h = float2(cell_h / COLORS + r_offset, g_offset);
	
	float4 graded_color_l = tex2D(GradingSampler, lut_pos_l);
	float4 graded_color_h = tex2D(GradingSampler, lut_pos_h);
	
	float4 graded_color = lerp(graded_color_l, graded_color_h, frac(cell));
	
	Out.vColor = graded_color;


	// ==================================================
	// FOG  =============================================
	if (0 == g_FogDestiny ||
		0 == vDepth.x)
		return Out;

	vector fogColor = tex2D(FogColorSampler, In.vTexUV);// vector(0.3, 0.3, 0.3, 1.0);
	float2 fog;
	fog.x = 500 / (500 - 0.1);
	fog.y = -1 / (500 - 0.1);

	float fDestiny = g_FogDestiny;
	if (0 == vDepth.x)
		fDestiny = 0.001f;
	
	//  ���� Fog
	vector vFog = 1 / exp(pow(PixelCameraZ * fDestiny, 2));

	if (0 == vDepth.x)
		vFog *= /*UV.y*/ saturate(1.f - (UV.y + 0.3f));

	//// ���� Fog
	//vector vFog = fog.x + PixelCameraZ * fog.y;

	Out.vColor = vFog * Out.vColor + (1 - vFog) * fogColor;

	//const float FOG_MIN = 0.9;
	//const float FOG_MAX = 0.99;
	//Out.vColor = lerp(Out.vColor, fogColor, lerp(FOG_MIN, FOG_MAX, PixelCameraZ));

	return Out;
}

PS_OUT PS_Bloom(PS_IN In) // Extract Bright Color
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse	= tex2D(DiffuseSampler, In.vTexUV);
	vector	vBloomPower = tex2D(BloomSampler, In.vTexUV);

	//// Bloom 1====================================================================
	//float fThreshold = 0.5;	 // �� ���������� ������ Bloom Ÿ��
	//Out.vColor = saturate((vDiffuse - fThreshold) / (1 - fThreshold));

	//// Bloom 2====================================================================
	////FragColor = vec4(lighting, 1.0);
	//float4 FragColor = vDiffuse;
	//float4 BrightColor;
	//float brightness = dot(FragColor.rgb, float3(0.2126, 0.3152, 0.322));
	//if (brightness > 1.0)
	//	BrightColor = float4(FragColor.rgb, 1.0);
	//else
	//	BrightColor = float4(0.0, 0.0, 0.0, 1.0);
	//Out.vColor = BrightColor;

	// Bloom 3====================================================================

	float fBloomPower = vBloomPower.x;
	if (fBloomPower == 0.f)
		fBloomPower = 0.75f;
	//fBloomPower = 1.5f; // �ʹ� ��Ƽ� �ӽ�

	Out.vColor = vDiffuse;
	Out.vColor.rgb -= fBloomPower; // ���� ���� ���� ���� �ΰ��� ����
	// ���� ���� ���ǿ�

	Out.vColor = 3.0f * max(Out.vColor, 0.0f); // ū ���� ���� Ȯ���� ����� ����
	// ���� ���� ���

	return Out;
}


PS_OUT PS_Default(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

PS_OUT MotionBlurForObj(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//float uVelocityScale = g_fCurFrame / g_fTargetFrame;
	int MAX_SAMPLES = 12;

	//float2 texelSize = float2(1.f / 1280.f, 1.f / 720.f);
	float2 screenTexCoords = In.vTexUV.xy;// *texelSize;

	float4 velocity = tex2D(ShadeSampler, screenTexCoords);
	//velocity.xy = pow(velocity.xy * 2 - 1, 3.0);
	//velocity *= uVelocityScale;
	velocity.xy = pow(velocity.xy, 1.0 / 3.0);
	velocity.xy = velocity.xy * 2.0 - 1.0;

	//float speed = length(velocity / texelSize);
	//float nSamples = clamp(int(speed), 1, MAX_SAMPLES);

	//vector	vDepthInfo = tex2D(DepthSampler, screenTexCoords);

	Out.vColor = tex2D(DiffuseSampler, screenTexCoords);

	// ����
	velocity.y *= 0.5f;
	velocity.xy = (clamp(velocity.x, -0.55f, 0.55f), clamp(velocity.y, -0.25f, 0.25f));

	for (int i = 1; i < MAX_SAMPLES; ++i) {
		// ���� ��ü�� ������ ����. ���� �͵鸸 ó���ض�
		//if (velocity.z < vDepthInfo.x + 0.34f)
		{
			float2 offset = velocity.xy * (float(i) / float(MAX_SAMPLES - 1) - 0.4);
			Out.vColor += tex2D(DiffuseSampler, screenTexCoords + offset);
		}
	}
	Out.vColor /= float(MAX_SAMPLES);
	Out.vColor.a *= tex2D(ShadeSampler, screenTexCoords).w;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering // 0
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();

		minfilter[0] = point;
		magfilter[0] = point;
	}

	pass ToneMapping // 1
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_TONEMAPPING();

		minfilter[0] = point;
		magfilter[0] = point;
	}

	pass Blur // 2
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;// compile vs_3_0 VS_BLUR();
		PixelShader = compile ps_3_0 PS_BLUR();

		minfilter[0] = linear;
		magfilter[0] = linear;
	}

	pass BrightPass // 3
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_Bloom();

		minfilter[0] = point;
		magfilter[0] = point;
	}

	pass Distortion // 4
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_AFTER();

		minfilter[0] = point;
		magfilter[0] = point;
	}

	pass Default // 5
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_Default();
	}

	pass MotionBlur // 6
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 MotionBlurForObj();
	}


	pass BlurH // 7
	{
		ZWriteEnable = false;
	
		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;
	
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLURH();
	}
	
	pass BlurV // 8
	{
		ZWriteEnable = false;
	
		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;
	
		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_BLURV();
	}
}

