
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
sampler	DistortionSampler = sampler_state
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
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	vector	vShade = pow(tex2D(ShadeSampler, In.vTexUV), 2.2);
	vector	vSpecular = tex2D(SpecularSampler, In.vTexUV);
	vector	vRim = pow(tex2D(BloomSampler, In.vTexUV), 2.2);

	Out.vColor = (vDiffuse + vSpecular + vRim) * vShade;
	//Out.vColor = pow(Out.vColor, 1 / 2.2);

	return Out;
}

PS_OUT PS_TONEMAPPING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
	Out.vColor += tex2D(BloomSampler, In.vTexUV);

	//// DX Sample ========================================================================
	//float Luminance = 0.08f;
	//static const float fMiddleGray = 0.18f;
	//static const float fWhiteCutoff = 0.9f;
	//
	//float3 Color = pow(Out.vColor, 1.f / 2.2f) * fMiddleGray / (Luminance + 0.001f);
	//Color *= (1.f + (Color / (fWhiteCutoff * fWhiteCutoff)));
	//Color /= (1.f + Color);
	//Out.vColor = float4(pow(Color, 1.f / 2.2f), 1.f);

	////ToneMapACES ========================================================================
	//const float A = 2.51, B = 0.03, C = 2.43, D = 0.59, E = 0.14;
	//Out.vColor = saturate((Out.vColor * (A * Out.vColor + B)) / (Out.vColor * (C * Out.vColor + D) + E));

	// EA studio ========================================================================
	float3 Color = Out.vColor.xyz;
	float3 x = max(0.f, Color - 0.004);
	Color = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
	//Color = pow(Color, 1.0 / 2.2);
	Out.vColor = float4(Color, 1.f);

	//// reinhardTone ========================================================================
	//float3 mapped = Out.vColor / (Out.vColor + float3(1.0, 1.0, 1.0));
	//mapped = pow(mapped, float3((1.0 / 2.2), (1.0 / 2.2), (1.0 / 2.2)));
	//Out.vColor = float4(mapped, 1.f);

	////// Uncharted2Tonemap ========================================================================
	//float A = 0.15;
	//float B = 0.50;
	//float C = 0.10;
	//float D = 0.20;
	//float E = 0.02;
	//float F = 0.30;
	//
	//float3 x = Out.vColor.rgb;
	//float3 Color = ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
	//Out.vColor = float4(Color, 1.f);



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

float4 gaussFilter[7] =
{
	0.0,-3.0,0.0,  1.0 / 64.0,
	0.0,-2.0,0.0,  6.0 / 64.0,
	0.0,-1.0,0.0, 15.0 / 64.0,
	0.0, 0.0,0.0, 20.0 / 64.0,
	0.0, 1.0,0.0, 15.0 / 64.0,
	0.0, 2.0,0.0,  6.0 / 64.0,
	0.0, 3.0,0.0,  1.0 / 64.0
};

//https://copynull.tistory.com/287
PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// 문제점 : 오른쪽, 아래에 얇은 줄이 생김 -> AdressUV Clamp로 해결

	vector	vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	float4 color = 0;
	float2 samp = In.vTexUV;
	float pixelWidthX = 1.f / 1280.f;
	float pixelWidthY = 1.f / 720.f;

	for (int i = 0; i < 13; i++)
	{
		samp.x = In.vTexUV.x + PixelKernel[i] * pixelWidthX;
		samp.y = In.vTexUV.y + PixelKernel[i] * pixelWidthY;
		color += tex2D(DiffuseSampler, samp) * BlurWeights[i] * 1.3f;
	}

	//////////////////////////////////////////////////////////////
	//float texOffset = 0.0;
	//for (int i = 0; i < 7; i++)
	//{
	//	color += tex2D(DiffuseSampler, float2(In.vTexUV.x + gaussFilter[i].x * pixelWidthX + texOffset,
	//	In.vTexUV.y + gaussFilter[i].y * pixelWidthY + texOffset)) * gaussFilter[i].w;
	//}

	color.a = 1;
	Out.vColor = color;

	return Out;
}

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
	Out.vColor = pow(vDiffuse, 1 / 2.2);
	//Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_Bloom(PS_IN In) // Extract Bright Color
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	//// Bloom 1====================================================================
	//float fThreshold = 0.5;	 // 이 기준점보다 밝으면 Bloom 타겟
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

	Out.vColor = vDiffuse;
	Out.vColor.rgb -= 0.85f; // 작은 값일 수록 빛에 민감한 광선

	Out.vColor = 3.0f * max(Out.vColor, 0.0f); // 큰 값일 수록 확실한 모양의 광선

	return Out;
}


PS_OUT PS_Default(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

matrix		g_matInvVP, g_matLastVP;
PS_OUT PS_MotionBlur(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//// Get the depth buffer value at this pixel.
	//float zOverW = tex2D(DepthSampler, In.vTexUV);
	//// H is the viewport position at this pixel in the range -1 to 1.    
	//float4 H = float4(In.vTexUV.x * 2 - 1, (1 - In.vTexUV.y) * 2 - 1, zOverW, 1);
	//// Transform by the view-projection inverse.   
	//float4 D = mul(H, g_matInvVP);
	//// Divide by w to get the world position.    
	//float4 worldPos = D / D.w; 

	// Current viewport position    
	//float4 currentPos = H;

	vector	vDepthInfo = tex2D(DepthSampler, In.vTexUV);
	float	fViewZ = vDepthInfo.g * 500.f;
	//float	zOverW = vDepthInfo.x;
	float	zOverW = 1;
	float4 H = float4(In.vTexUV.x * 2 - 1, (In.vTexUV.y * -2) + 1, zOverW, 1);
	//H = H * fViewZ;

	float4 D = mul(H, g_matInvVP);
	vector worldPos = D / D.w;

	float4 currentPos = H;

	// Use the world position, and transform by the previous view-projection matrix.    
	float4 previousPos = mul(worldPos, g_matLastVP);
	// Convert to nonhomogeneous points [-1,1] by dividing by w.
	previousPos /= previousPos.w;
	// Use this frame's position and last frame's to compute the pixel velocity.   
	float2 velocity = (currentPos.xy - previousPos.xy) / 2.f;

	// Get the initial color at this pixel.    
	float4 color = tex2D(DiffuseSampler, In.vTexUV);
	In.vTexUV += velocity;

	int g_numSamples = 3;
	for (int i = 1; i < g_numSamples; ++i, In.vTexUV += velocity)
	{   // Sample the color buffer along the velocity vector.    
		float4 currentColor = tex2D(DiffuseSampler, In.vTexUV);
		// Add the current color to our color sum.   
		color += currentColor;
	}
	// Average all of the samples to get the final blur color.
	float4 finalColor = color / g_numSamples;

	Out.vColor = finalColor;

	return Out;
}

PS_OUT MotionBlurForObj(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// ==========================================================
	// Get the initial color at this pixel.    
	float4 velocity = tex2D(ShadeSampler, In.vTexUV);
	float4 color = tex2D(DiffuseSampler, In.vTexUV);
	In.vTexUV += velocity.xy;

	int g_numSamples = 10;
	for (int i = 1; i < g_numSamples; ++i, In.vTexUV += velocity.xy)
	{   // Sample the color buffer along the velocity vector.    
		float4 currentColor = tex2D(DiffuseSampler, In.vTexUV);
		// Add the current color to our color sum.   
		color += currentColor;
	}
	// Average all of the samples to get the final blur color.
	float4 finalColor = color / g_numSamples;
	finalColor.a *= velocity.a;
	Out.vColor = finalColor; 

	//// From Velocity Map =============================================
	//const int SAMPLES = 26;
	//const float samples = 26;
	//
	//float4 velocityTest = tex2D(ShadeSampler, In.vTexUV);
	//velocityTest.xy /= 1.f;
	//
	//int cnt = 1;
	//float4 Color = 0;
	//float4 Scene = tex2D(DiffuseSampler, In.vTexUV);
	//
	//for (int i = cnt; i < SAMPLES; i++)
	//{
	//	Color = tex2D(DiffuseSampler, In.vTexUV + velocityTest.xy * (float)i);
	//
	//	if (velocityTest.a < Color.a + 0.04f)
	//	{
	//		cnt++;
	//		Scene += Color;
	//	}
	//}
	//
	//Out.vColor = Scene / (float)cnt;

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
		//
		//AlphaBlendEnable = true;
		//SrcBlend = One;
		//DestBlend = InvSrcAlpha;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MotionBlur();
	}

	pass MotionBlurForObj // 7
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 MotionBlurForObj();
	}
}

