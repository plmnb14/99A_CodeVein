

matrix		g_matWorld, g_matView, g_matProj, g_matLastWVP;
matrix		g_matWVP;
bool		g_bMotionBlur = true;
bool        g_bDecalTarget = false;
float		g_MinValue = 0.25f;
float		g_fBloomPower = 0.f;

texture		g_HeightTexture;

//====================================================================================================
// Height Map
//====================================================================================================
sampler		HeightSampler = sampler_state
{
	texture = g_HeightTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};
//====================================================================================================

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
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

VS_MOTIONBLUR_OUT VS_MOTIONBLUR(VS_IN In)
{
	VS_MOTIONBLUR_OUT	Out = (VS_MOTIONBLUR_OUT)0;

	float4 currentPos = mul(float4(In.vPosition.xyz, 1.f), g_matWVP);

	// Use the world position, and transform by the previous view-projection matrix.    
	float4 previousPos = mul(float4(In.vPosition.xyz, 1.f), g_matLastWVP);

	Out.vPosition = currentPos;
	Out.vProjPos = Out.vPosition;
	Out.vLastPos = previousPos;

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_MOTIONBLUR_IN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vVelocity : COLOR0;
	vector		vNormal : COLOR1; // For RimLight
	vector		vBloomPower : COLOR2;
	vector		vDecalDepth : COLOR3;
};

// 픽셀의 색을 결정한다.
PS_OUT PS_MOTIONBLUR(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	float fMinValue = 0.25f;
	Out.vVelocity.w = velocity.x < fMinValue && velocity.y < fMinValue ? 0.f : Out.vVelocity.w;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);
	
	float HeightValue = tex2D(HeightSampler, In.vTexUV).x;

	Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
	Out.vBloomPower = vector(g_fBloomPower,0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_Decal(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	float fMinValue = 0.25f;
	Out.vVelocity.w = velocity.x < fMinValue && velocity.y < fMinValue ? 0.f : Out.vVelocity.w;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	float HeightValue = tex2D(HeightSampler, In.vTexUV).x;

	Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
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

	float fMinValue = 0.25f;
	Out.vVelocity.w = velocity.x >= fMinValue && velocity.y >= fMinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	float HeightValue = tex2D(HeightSampler, In.vTexUV).x;

	Out.vNormal = vector(1.f, 0.f, 1.f, 1.f);
	Out.vBloomPower = vector(g_fBloomPower, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_HeightSkin(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_MinValue && velocity.y >= g_MinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	Out.vNormal = vector(1.f, 0.f, 0.f, 1.f);
	Out.vBloomPower = vector(g_fBloomPower, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_Alpha(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f;
	float2 velocity = pow(((a - b) * 0.5f + 0.5f), 3.0f);

	Out.vVelocity.w = velocity.x >= g_MinValue && velocity.y >= g_MinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);
	Out.vNormal = vector(1.f, 0.f, 1.f, 1.f);
	Out.vBloomPower = vector(g_fBloomPower, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT PS_MOTIONBLUR_RenderOnlyObject(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5f + 0.5f;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5f + 0.5f; 
	float2 velocity = pow(((a - b) * 0.5f + 0.5f) , 3.0f);

	Out.vVelocity.w = velocity.x >= g_MinValue && velocity.y >= g_MinValue ? Out.vVelocity.w : 0.f;
	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);

	float HeightValue = tex2D(HeightSampler, In.vTexUV).x;

	Out.vNormal = vector(0.f, 0.f, 0.f, 0.f);
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

