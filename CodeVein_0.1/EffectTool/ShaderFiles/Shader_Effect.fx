

matrix		g_matWorld, g_matView, g_matProj, g_matInvWorld;
float		g_fAlpha = 1;
float		g_fDistortion = 0.15f;
float		g_fDissolve = 0.f;
vector		g_vColor = { 0, 0, 0, 1 };
bool		g_bUseRGBA = false;
bool		g_bUseColorTex = false;
bool		g_bUseMaskTex = false;
bool		g_bReverseColor = false;
bool		g_bDissolve = false;

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
	//float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;

	float4		vInstanceRight	: TEXCOORD1;
	float4		vInstanceUp		: TEXCOORD2;
	float4		vInstanceLook	: TEXCOORD3;
	float4		vInstancePos	: TEXCOORD4;
	float4		vColor			: TEXCOORD5;
	float3		vOption01		: TEXCOORD6;
	bool4		vOption02		: TEXCOORD7;
	bool4		vOption03		: TEXCOORD8;

};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;

	
};

struct VS_INSTANCE_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD9;

	float4		vInstanceRight	: TEXCOORD1;
	float4		vInstanceUp		: TEXCOORD2;
	float4		vInstanceLook	: TEXCOORD3;
	float4		vInstancePos	: TEXCOORD4;
	float4		vColor			: TEXCOORD5;
	float3		vOption01		: TEXCOORD6;
	bool4		vOption02		: TEXCOORD7;
	bool4		vOption03		: TEXCOORD8;
};

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

VS_INSTANCE_OUT VS_INSTANCE(VS_IN In)
{
	VS_INSTANCE_OUT		Out = (VS_INSTANCE_OUT)0;

	float4x4 matWorld, matWVP;
	matWorld = float4x4(In.vInstanceRight,
		In.vInstanceUp,
		In.vInstanceLook,
		float4(In.vInstancePos.xyz, 1.f));

	matWVP = mul(matWorld, g_matView);
	matWVP = mul(matWVP, g_matProj);

	Out.vPosition = mul(float4(In.vPosition.xyz, 1.f), matWVP);

	Out.vProjPos = Out.vPosition;
	Out.vTexUV = In.vTexUV;
	Out.vColor		    = In.vColor		   ;
	Out.vOption01	    = In.vOption01	   ;
	Out.vOption02	    = In.vOption02	   ;
	Out.vOption03	    = In.vOption03	   ;
	
	return Out;
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	//float4		vColor : COLOR0;
};

struct PS_INSTANCE_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD9;

	float4		vInstanceRight	: TEXCOORD1;
	float4		vInstanceUp		: TEXCOORD2;
	float4		vInstanceLook	: TEXCOORD3;
	float4		vInstancePos	: TEXCOORD4;
	float4		vColor			: TEXCOORD5;
	float3		vOption01		: TEXCOORD6;
	bool4		vOption02		: TEXCOORD7;
	bool4		vOption03		: TEXCOORD8;
};

struct PS_OUT
{
	vector		vColor		:	COLOR0;
	vector		vDistortion :	COLOR1;
};

float3 lumCoeff = float3(0.2125, 0.7154, 0.0721);
float3 root3 = float3(0.57735, 0.57735, 0.57735);
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

PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_bUseColorTex)
	{
		Out.vColor = pow(tex2D(ColorSampler, In.vTexUV), 2.2);
		//Out.vColor = tex2D(ColorSampler, In.vTexUV);
		Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;
	}
	else
	{
		Out.vColor = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
		//Out.vColor = pow(tex2D(DiffuseSampler, In.vTexUV)  * In.vColor, 2.2);
		Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;
	}

	if (g_bUseRGBA)
	{
		Out.vColor.xyz = g_vColor.xyz;
		Out.vColor.a *= g_vColor.a;
	}
	else
	{
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
	}

	if (g_bUseMaskTex)
	{
		//float fGradientUV = In.vTexUV + (g_fAlpha);
		vector vGradientMask = tex2D(GradientSampler, In.vTexUV);
		Out.vColor.a *= vGradientMask.x;
	}

	if (g_bDissolve)
	{
		float4 fxColor = tex2D(DiffuseSampler, In.vTexUV);

		if (Out.vColor.a == 0.f)
			clip(-1);

		if (fxColor.r >= g_fDissolve)
			Out.vColor.a = 1;
		else
			Out.vColor.a = 0;
	}

	if (g_bReverseColor)
		Out.vColor.rgb = 1 - Out.vColor.rgb;

	// 소프트 이펙트 ==========================================================================================
	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 500.f;

	Out.vColor.a = (Out.vColor.a * saturate(fViewZ - In.vProjPos.w)) * g_fAlpha;
	// =========================================================================================================



	return Out;
}

PS_OUT PS_INSTANCE(PS_INSTANCE_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor		= In.vColor;
	float fDistortion	= In.vOption01.x;
	float fDissolve		= In.vOption01.y;
	float fAlpha		= In.vOption01.z;
	 bool bDissolve		= In.vOption02.x;
	 bool bUseColorTex	= In.vOption02.y;
	 bool bReverseColor = In.vOption02.z;
	 bool bUseRGBA		= In.vOption02.w;
	 bool bUseMaskTex	= In.vOption03.x;

	if (bUseColorTex)
	{
		Out.vColor = pow(tex2D(ColorSampler, In.vTexUV), 2.2);
		Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;
	}
	else
	{
		Out.vColor = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
		Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;
	}

	if (bUseRGBA)
	{
		Out.vColor.xyz = vColor.xyz;
		Out.vColor.a *= vColor.a;
	}
	else
	{
		// ==============================================================================================
		// [Memo]  g_vColor.x = Hue / g_vColor.y = Contrast / g_vColor.z = Brightness / g_vColor.w = Saturation
		// ==============================================================================================
		float3 intensity;
		float half_angle = 0.5 * radians(vColor.x); // Hue is radians of 0 tp 360 degree
		float4 rot_quat = float4((root3 * sin(half_angle)), cos(half_angle));
		float3x3 rot_Matrix = QuaternionToMatrix(rot_quat);
		Out.vColor.rgb = mul(rot_Matrix, Out.vColor.rgb);
		Out.vColor.rgb = (Out.vColor.rgb - 0.5) *(vColor.y + 1.0) + 0.5;
		Out.vColor.rgb = Out.vColor.rgb + vColor.z;
		intensity = float(dot(Out.vColor.rgb, lumCoeff));
		Out.vColor.rgb = lerp(intensity, Out.vColor.rgb, vColor.w);
		// End ==========================================================================================
	}

	if (bUseMaskTex)
	{
		vector vGradientMask = tex2D(GradientSampler, In.vTexUV);
		Out.vColor.a *= vGradientMask.x;
	}

	if (bDissolve)
	{
		float4 fxColor = tex2D(DiffuseSampler, In.vTexUV);

		if (Out.vColor.a == 0.f)
			clip(-1);

		if (fxColor.r >= fDissolve)
			Out.vColor.a = 1;
		else
			Out.vColor.a = 0;
	}

	if (bReverseColor)
		Out.vColor.rgb = 1 - Out.vColor.rgb;

	// 소프트 이펙트 ==========================================================================================
	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 500.f;

	Out.vColor.a = (Out.vColor.a * saturate(fViewZ - In.vProjPos.w)) * fAlpha;
	// =========================================================================================================

	return Out;
}

PS_OUT PS_DISTORTION(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	if (g_bUseColorTex)
	{
		Out.vColor = tex2D(ColorSampler, In.vTexUV);
		Out.vColor *= tex2D(DiffuseSampler, In.vTexUV).x;
	}
	else
	{
		Out.vColor = tex2D(DiffuseSampler, In.vTexUV);
		//Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;
	}

	if (g_bUseMaskTex)
	{
		vector vGradientMask = tex2D(GradientSampler, In.vTexUV);
		Out.vColor *= vGradientMask.x;
	}

	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	//vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	//float		fViewZ = vDepthInfo.y * 500.f;
	//Out.vColor.a = Out.vColor.a * saturate(fViewZ - In.vProjPos.w);

	//float fPower = In.vProjPos.w;
	//Out.vColor *= (1.f - (Out.vColor.x + Out.vColor.y));
	//Out.vColor -= (Out.vColor.x + Out.vColor.y);
	//Out.vColor *= fPower;

	Out.vColor *= g_fAlpha;
	Out.vColor.z = g_fDistortion;
	Out.vDistortion = saturate(Out.vColor);

	return Out;
}

PS_OUT PS_MESHEFFECT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = pow(tex2D(ColorSampler, In.vTexUV), 2.2);
	//Out.vColor = tex2D(ColorSampler, In.vTexUV);
	Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;

	if (g_bUseRGBA)
	{
		Out.vColor.xyz = g_vColor.xyz;
		Out.vColor.a *= g_vColor.a;
	}
	else
	{
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
	}

	Out.vColor.a *= g_fAlpha;

	if(g_bReverseColor)
		Out.vColor.rgb = 1 - Out.vColor.rgb;

	return Out;
}
matrix		g_matProjInv;
matrix		g_matViewInv;
PS_OUT PS_SSD(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	vector		vDepthInfo = tex2D(DepthSampler, In.vTexUV); //Depth값을 알아옵니다.  
	float		fViewZ = vDepthInfo.g * 500.f;
	vector		vWorldPos;
	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthInfo.x;
	vWorldPos.w = 1.f;
	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_matProjInv);
	vWorldPos = mul(vWorldPos, g_matViewInv);

	float3 decalLocalPos = float3(0, 0, 0);
	decalLocalPos = mul(vWorldPos, g_matInvWorld).xyz;
	
	float2 decalUV = decalLocalPos.xy + 0.5f;
	
	//float dist = abs(decalLocalPos.z); //decal의 local 깊이
	//float scaleDistance = max(dist * 2.0f, 1.0f);//Local깊이를 0.0과 1.0으로 맵핑시킵니다. 
	//float fadeOut = 1.0f - scaleDistance;

	float4 Color = float4(1, 1, 1, 1);
	if (g_bUseColorTex)
	{
		Color = tex2D(ColorSampler, In.vTexUV);
		Color *= tex2D(DiffuseSampler, In.vTexUV).x;
	}
	else
	{
		Color = tex2D(DiffuseSampler, In.vTexUV);
	}

	//Color.a *= fadeOut; // FadeOut값을 곱해주면 표면의 부분 이외는 사라지게 됩니다.  
	
	Out.vColor = Color;
	
	//Out.vColor = float4(1, 1, 1, 1);

	return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	Out.vColor.a = tex2D(DiffuseSampler, In.vTexUV).x;

	// 소프트 이펙트 ==========================================================================================
	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 500.f;

	Out.vColor.a = (Out.vColor.a * saturate(fViewZ - In.vProjPos.w)) * g_fAlpha;
	// =========================================================================================================

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering // 0
	{	
		ZWriteEnable = false;
		AlphablendEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}	

	pass Distortion // 1
	{
		ZWriteEnable = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DISTORTION();
	}

	pass MeshEffect // 2
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

	pass InstanceTexEffect // 3
	{
		zwriteenable = false;

		AlphablendEnable = true;
		AlphaTestEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		blendop = add;

		cullmode = none;

		VertexShader = compile vs_3_0 VS_INSTANCE();
		PixelShader = compile ps_3_0 PS_INSTANCE();
	}

	pass SSD // 4
	{
		zwriteenable = false;

		AlphablendEnable = true;
		AlphaTestEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		//blendop = add;
		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_SSD();
	}

	pass TRAIL // 5
	{
		zwriteenable = false;

		AlphablendEnable = true;
		AlphaTestEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		//blendop = add;
		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_TRAIL();
	}
}

