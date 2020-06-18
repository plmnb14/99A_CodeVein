
matrix		g_matWorld, g_matView, g_matProj;
matrix		g_matWVP, g_matLastWVP;
float		g_fMinValue = 0.25f;
float		g_fBloomPower = 0.f;

struct VS_IN
{
	float3		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBinormal	: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_MOTIONBLUR_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV	: TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_MOTIONBLUR_OUT VS_MOTIONBLUR(VS_IN In)
{
	VS_MOTIONBLUR_OUT	Out = (VS_MOTIONBLUR_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(float4(In.vPosition.xyz, 1.f), matWVP);
	Out.vProjPos = Out.vPosition;
	Out.vLastPos = mul(float4(In.vPosition.xyz, 1.f), g_matLastWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_MOTIONBLUR_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vVelocity : COLOR0;
	vector		vBloomPower : COLOR1;
	vector		vDecalDepth : COLOR2;
};

// 픽셀의 색을 결정한다.
PS_OUT PS_MOTIONBLUR(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_fMinValue && velocity.y >= g_fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vBloomPower = vector(g_fBloomPower,0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_Decal(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_fMinValue && velocity.y >= g_fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vBloomPower = vector(g_fBloomPower, 0.f, 0.f, 0.f);

	Out.vDecalDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_Height(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_fMinValue && velocity.y >= g_fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vBloomPower = vector(g_fBloomPower, 0.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_HeightSkin(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_fMinValue && velocity.y >= g_fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vBloomPower = vector(g_fBloomPower, 0.f, 1.f, 0.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_Alpha(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_fMinValue && velocity.y >= g_fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vBloomPower = vector(g_fBloomPower, 0.f, 1.f, 1.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_RenderOnlyObject(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f; 
	float2 velocity = pow(((a - b) * 0.5f + 0.5f) , 3.0f);

	Out.vVelocity.w = velocity.x >= g_fMinValue && velocity.y >= g_fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vBloomPower = vector(g_fBloomPower, 0.f, 0.f, 0.f);
	Out.vDecalDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 1.f, 1.f);

	return Out;
}

technique Default_Technique
{
	//====================================================================================================
	// 0 - MotionBlur
	//====================================================================================================
	pass MotionBlur
	{
		cullmode = ccw;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR();
	}

	//====================================================================================================
	// 1 - MotionBlur + Skin ( 일반 피부)
	//====================================================================================================
	pass MotionBlur_Height
	{
		cullmode = ccw;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR_HeightSkin();
	}

	//====================================================================================================
	// 2 - MotionBlur + HeightMap (얼굴)
	//====================================================================================================
	pass MotionBlur_HeightSkin
	{
		cullmode = ccw;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR_Height();
	}

	//====================================================================================================
	// 3 - MotionBlur + EyeRash
	//====================================================================================================
	pass MotionBlur_Alpha
	{
		AlphaTestEnable = true;
		Alphafunc = greater;
		Alpharef = 0xc0;
		cullmode = none;

		AlphablendEnable = true;
		srcblend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		blendop = add;

		cullmode = none;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR_Alpha();
	}

	//====================================================================================================
	// 4 - MotionBlur for RenderOnlyObject
	//====================================================================================================
	pass MotionBlur_RenderOnlyObject
	{
		cullmode = ccw;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR_RenderOnlyObject();
	}

	//====================================================================================================
	// 5 - MotionBlur + DecalTarget
	//====================================================================================================
	pass MotionBlur_Decal
	{
		cullmode = ccw;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR_Decal();
	}
}

