

matrix		g_matWorld, g_matView, g_matProj;
float		g_fAlpha;
vector		g_vColor;
bool		g_bUseRGBA = false;
bool		g_bUseColorTex = false;
bool		g_bReverseColor = false;

texture		g_DiffuseTexture;
sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_ColorTexture;
sampler		ColorSampler = sampler_state
{
	texture = g_ColorTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_GradientTexture;
sampler		GradientSampler = sampler_state
{
	texture = g_GradientTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_DepthTexture;
sampler		DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

float3x3 QuaternionToMatrix(float4 quat)
{
	float3 cross = quat.yzx * quat.zxy;
	float3 square = quat.xyz * quat.xyz;
	float3 wimag = quat.w * quat.xyz;

	square = square.xyz + square.yzx;

	float3 diag = 0.5 - square;
	float3 a = (cross + wimag);
	float3 b = (cross - wimag);

	return float3x3(
		2.0 * float3(diag.x, b.z, a.y),
		2.0 * float3(a.z, diag.y, b.x),
		2.0 * float3(b.y, a.x, diag.z));
}

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;		
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
	vector		vDistortion : COLOR1;
};

float3 lumCoeff = float3(0.2125, 0.7154, 0.0721);
float3 root3 = float3(0.57735, 0.57735, 0.57735);

PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_bUseColorTex)
	{
		Out.vColor = tex2D(ColorSampler, In.vTexUV);
		Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;
	}
	else
	{
		Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
		Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;

		if (g_bUseRGBA)
		{
			Out.vColor.xyz = g_vColor.xyz;
			Out.vColor.a *= g_vColor.a;
		}
	}

	// ==============================================================================================
	// [Memo]  g_vColor.x = Hue / g_vColor.y = Contrast / g_vColor.z = Brightness / g_vColor.w = Saturation
	// ==============================================================================================
	float3 intensity;
	float half_angle = 0.5 * radians(g_vColor.x); // Hue is radians of 0 tp 360 degree
	float4 rot_quat = float4((root3 * sin(half_angle)), cos(half_angle));
	float3x3 rot_Matrix = QuaternionToMatrix(rot_quat);
	Out.vColor.rgb = mul(rot_Matrix, Out.vColor.rgb);
	Out.vColor.rgb = (Out.vColor.rgb - 0.5) *(g_vColor.y + 1.0) + 0.5;
	Out.vColor.rgb = Out.vColor.rgb + g_vColor.z;
	intensity = float(dot(Out.vColor.rgb, lumCoeff));
	Out.vColor.rgb = lerp(intensity, Out.vColor.rgb, g_vColor.w);
	// End ==========================================================================================

	//float fGradientUV = In.vTexUV + (g_fAlpha);
	//vector vGradientMask = tex2D(GradientSampler, fGradientUV );
	//Out.vColor *= vGradientMask;

	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 300.f;

	Out.vColor.a = (Out.vColor.a * saturate(fViewZ - In.vProjPos.w)) * g_fAlpha;

	if (g_bReverseColor)
		Out.vColor.rgb = 1 - Out.vColor.rgb;

	return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	/////////////////////////////////////////////
	// Color
	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 300.f;

	Out.vColor.a = Out.vColor.a * saturate(fViewZ - In.vProjPos.w);

	//float fPower = In.vProjPos.w;
	Out.vColor.a -= (Out.vColor.x + Out.vColor.y);
	//Out.vColor *= fPower;
	Out.vDistortion = saturate(Out.vColor);


	return Out;
}

PS_OUT PS_MESHEFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(ColorSampler, In.vTexUV);
	Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;

	Out.vColor.a *= g_fAlpha;

	// Hue, Contrast, Brightness, Saturation ===============================================
	float3 intensity;
	float half_angle = 0.5 * radians(g_vColor.x); // Hue is radians of 0 tp 360 degree
	float4 rot_quat = float4((root3 * sin(half_angle)), cos(half_angle));
	float3x3 rot_Matrix = QuaternionToMatrix(rot_quat);
	Out.vColor.rgb = mul(rot_Matrix, Out.vColor.rgb);
	Out.vColor.rgb = (Out.vColor.rgb - 0.5) *(g_vColor.y + 1.0) + 0.5;
	Out.vColor.rgb = Out.vColor.rgb + g_vColor.z;
	intensity = float(dot(Out.vColor.rgb, lumCoeff));
	Out.vColor.rgb = lerp(intensity, Out.vColor.rgb, g_vColor.w);
	// Hue End ===============================================

	if(g_bReverseColor)
		Out.vColor.rgb = 1 - Out.vColor.rgb;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{	
		ZWriteEnable = false;
		AlphablendEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}	

	pass Distortion
	{
		ZWriteEnable = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DISTORTION();
	}

	pass MeshEffect
	{
		zwriteenable = false;

		AlphablendEnable = true;
		AlphaTestEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		blendop = add;

		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MESHEFFECT();
	}
}

