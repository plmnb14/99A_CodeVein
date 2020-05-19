int g_iToneIndex = 0;
float g_fToneGradient= 0.f;
float g_fPixelWidth = 1.f / 1280.f;
float g_fPixelHeight = 1.f / 720.f ;

texture		g_ShadowMapTexture;

sampler ShadowMapSampler = sampler_state
{
	texture = g_ShadowMapTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	addressU = border;
	addressV = border;

	BorderColor = float4(1.f, 0.0f, 0.0f, 1.0f);

};

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

float g_FogDestiny;
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse	= tex2D(DiffuseSampler, In.vTexUV);
	vector	vEmissive	= tex2D(EmissiveSampler, In.vTexUV);
	vector	vShade		= tex2D(ShadeSampler, In.vTexUV);
	vector	vSpecular	= tex2D(SpecularSampler, In.vTexUV);
	vector	vSSAO		= tex2D(SSAOSampler, In.vTexUV);

	float AO = vSpecular.a;
	vSpecular.a = 0.f;

	float3 vFinalShade = vShade.r * vSSAO.r * AO;

	Out.vColor = ((vDiffuse + vSpecular) * float4(vFinalShade, 1.f)) + vEmissive;

	// ==================================================
	// FOG  =============================================
	vector	vDepth = tex2D(DepthSampler, In.vTexUV);

	if (0 == vDepth.x || 0 == g_FogDestiny)
		return Out;

	float PixelCameraZ = vDepth.y * 500.f;
	vector fogColor = tex2D(FogColorSampler, In.vTexUV);// vector(0.3, 0.3, 0.3, 1.0);
	float2 fog;
	fog.x = 500 / (500 - 0.1);
	fog.y = -1 / (500 - 0.1);
	
	// 선형 Fog
	vector vFog = fog.x + PixelCameraZ * fog.y;

	Out.vColor = vFog * Out.vColor + (1 - vFog) * fogColor;

	return Out;
}

PS_OUT PS_TONEMAPPING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
	Out.vColor += tex2D(BloomSampler, In.vTexUV);

	Out.vColor = pow(Out.vColor, 1 / 2.2);

	// MONO =========================================================
	float3 Color = saturate(Out.vColor.rgb);
	Color.rgb = lerp(Color, dot(Color.rgb, float3(0.3, 0.59, 0.11)), g_fToneGradient);
	Out.vColor = float4(Color, 1.f);
	
	return Out;
}

PS_OUT PS_TONEMAPPING_NoMono(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
	Out.vColor += tex2D(BloomSampler, In.vTexUV);

	Out.vColor = pow(Out.vColor, 1 / 2.2);

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

PS_OUT PS_BLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	//Out.vColor = vDiffuse;
	//return Out;

	float4 color = 0;
	float2 samp = In.vTexUV;

	for (int i = 0; i < 13; i++)
	{
		samp.x = In.vTexUV.x + PixelKernel[i] * g_fPixelWidth;
		samp.y = In.vTexUV.y + PixelKernel[i] * g_fPixelHeight;
		color.xyz += tex2D(DiffuseSampler, samp).xyz * BlurWeights[i] * 1.3f;
	}

	Out.vColor = color;

	return Out;
}

static const float offset[] = { 0.0, 1.0, 2.0, 3.0, 4.0 };
static const float weight[] = {
	0.2270270270, 0.1945945946, 0.1216216216,
	0.0540540541, 0.0162162162
};
static const float fBlurColor = 1.3f; // 클 수록 블러 색이 진해짐
PS_OUT PS_BLURH(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	float3 ppColour = 0;
	float3 FragmentColor = float3(0.0f, 0.0f, 0.0f);
	
	float hstep = 1.0;
	float vstep = 0.0;

	for (int i = 1; i < 5; i++) {
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) + float2(hstep*offset[i], vstep*offset[i]) * g_fPixelWidth)).xyz * weight[i] * fBlurColor;
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) - float2(hstep*offset[i], vstep*offset[i]) * g_fPixelWidth)).xyz * weight[i] * fBlurColor;
	}

	ppColour += FragmentColor;
	
	Out.vColor = float4(ppColour, 1.0);
	
	return Out;
}

PS_OUT PS_BLURV(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	float3 ppColour = 0;
	float3 FragmentColor = float3(0.0f, 0.0f, 0.0f);
	
	float hstep = 0.0;
	float vstep = 1.0;

	for (int i = 1; i < 5; i++) {
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) + float2(hstep*offset[i], vstep*offset[i]) * g_fPixelHeight)).xyz * weight[i] * fBlurColor;
		FragmentColor +=
			tex2D(DiffuseSampler, (float2(In.vTexUV) - float2(hstep*offset[i], vstep*offset[i]) * g_fPixelHeight)).xyz * weight[i] * fBlurColor;
	}
	ppColour += FragmentColor;
	
	Out.vColor = float4(ppColour, 1.0);

	return Out;
}

float g_Focus_DOF;
float g_Range_DOF;

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
	Noise.xy *= Noise.w; // 알파값

	float fPower = Noise.z;

	float2 UV = In.vTexUV + (Noise.xy * fPower);
	if (Noise.w <= 0)
		UV = In.vTexUV;
	// Calc Distortion End ========================
	// ============================================

	vector	vDiffuse = tex2D(DiffuseSampler, UV);
	Out.vColor = vDiffuse;

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

	return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	// ============================================
	// Calc Distortion ============================
	float2 Trans = In.vTexUV;// +0.001f;
	vector	Noise = tex2D(DistortionSampler, Trans);
	Noise.xy *= Noise.w; // 알파값

	float fPower = Noise.z;

	float2 UV = In.vTexUV + (Noise.xy * fPower);
	if (Noise.w <= 0)
		UV = In.vTexUV;
	// Calc Distortion End ========================
	// ============================================

	vector	vDiffuse = tex2D(DiffuseSampler, UV);
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT PS_DOF(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	// ============================================
	// DOF =========================================
	vector vDepth = tex2D(DepthSampler, In.vTexUV);
	float PixelCameraZ = vDepth.y * 500.f;

	vector	vBlur = tex2D(ShadeSampler, In.vTexUV);
	Out.vColor = lerp(Out.vColor, vBlur, saturate(g_Range_DOF * abs(g_Focus_DOF - PixelCameraZ)));

	return Out;
}

PS_OUT PS_COLOR_GRADING(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

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

	return Out;
}


PS_OUT PS_Bloom(PS_IN In) // Extract Bright Color
{
	PS_OUT			Out = (PS_OUT)0;

	vector	vDiffuse	= tex2D(DiffuseSampler, In.vTexUV);
	vector	vBloomPower = tex2D(BloomSampler, In.vTexUV);

	float fBloomPower = vBloomPower.x;
	if (fBloomPower == 0.f)
		fBloomPower = 0.75f;
	//fBloomPower = 1.5f; // 너무 밝아서 임시

	Out.vColor = vDiffuse;
	Out.vColor.rgb -= fBloomPower; // 작은 값일 수록 빛에 민감한 광선
	// 작은 빛도 블룸되요

	Out.vColor = 3.0f * max(Out.vColor, 0.0f); // 큰 값일 수록 확실한 모양의 광선
	// 작은 빛도 블룸

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

	int MAX_SAMPLES = 12;

	float2 screenTexCoords = In.vTexUV.xy;// *texelSize;

	float4 velocity = tex2D(ShadeSampler, screenTexCoords);
	velocity.xy = pow(velocity.xy, 1.0 / 3.0);
	velocity.xy = velocity.xy * 2.0 - 1.0;

	Out.vColor = tex2D(DiffuseSampler, screenTexCoords);

	// 제한
	velocity.y *= 0.5f;
	velocity.xy = (clamp(velocity.x, -0.55f, 0.55f), clamp(velocity.y, -0.25f, 0.25f));

	for (int i = 1; i < MAX_SAMPLES; ++i) {
		float2 offset = velocity.xy * (float(i) / float(MAX_SAMPLES - 1) - 0.4);
		Out.vColor += tex2D(DiffuseSampler, screenTexCoords + offset);
	}
	Out.vColor /= float(MAX_SAMPLES);
	Out.vColor.a *= tex2D(ShadeSampler, screenTexCoords).w;

	return Out;
}

float4 g_LightShaftValue; // x = Destiny / y = Decay / z = Weight / w = Exposure
float3 g_ScreenLightPos;
matrix g_matWVP;
PS_OUT GodRay(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

#define NUM_SAMPLES 64

	float2 texCoord = In.vTexUV;

	float3 lightPos = mul(g_ScreenLightPos, g_matWVP).xyz;
	float2 screenLightPos = lightPos.xy / 1.0f * 0.5f - 0.5f;
	screenLightPos.y = 1.0f - screenLightPos.y;

	float2 DeltaTexCoord = (texCoord.xy - screenLightPos.xy); // 화면 공간에서 픽셀에서 광원까지의 벡터를 계산합니다.

	DeltaTexCoord *= 1.0f / NUM_SAMPLES * g_LightShaftValue.x; // 샘플링할 수로 나누고, 제어 팩터로 스케일링합니다.

	float3 Color = tex2D(DiffuseSampler, texCoord).xyz;
	if(Color.g < 1.f)
		return Out;

	float IlluminationDecay = 1.0f; // 일루미네이션 감소 팩터를 설정합니다.

	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		texCoord -= DeltaTexCoord; // 광선을 따라 샘플링

		float3 Sample = tex2D(DiffuseSampler, texCoord).xyz; // 샘플을 새 좌표로 위치시킴

		Sample *= IlluminationDecay * g_LightShaftValue.z; // 스케일 / 감쇄 팩터를 적용하여 감소.

		Color += Sample; // 조합 된 색상을 축적합니다. 

		IlluminationDecay *= g_LightShaftValue.y; // 감쇄 팩터 지수를 업데이트합니다. 
	}

	Out.vColor = saturate(float4(Color * g_LightShaftValue.w, 1.0)); // 추가 컨트롤 팩터와 함께 최종 색상을 출력합니다.

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

	pass GodRay // 9
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 GodRay();
	}

	pass ToneMapping_NO_MONO // 10
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_TONEMAPPING_NoMono();

		minfilter[0] = point;
		magfilter[0] = point;
	}

	pass DISTORTION // 11
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_DISTORTION();

		minfilter[0] = point;
		magfilter[0] = point;
	}

	pass DOF // 13
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_DOF();

		minfilter[0] = point;
		magfilter[0] = point;
	}


	pass COLOR_GRADING // 13
	{
		ZWriteEnable = false;

		AlphatestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_COLOR_GRADING();

		minfilter[0] = point;
		magfilter[0] = point;
	}
	
}

