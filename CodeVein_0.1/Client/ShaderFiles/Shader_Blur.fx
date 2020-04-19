

matrix		g_matWorld, g_matView, g_matProj, g_matLastWVP;
bool		g_bMotionBlur;
float		g_fRimPower, g_fBloomPower = 0.5f;

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

	// 월드변환, 뷰변환, 투영행렬변환.
	matrix		matWV, matWVP, matVP;

	matWV = mul(g_matWorld, g_matView);
	matVP = mul(g_matView, g_matProj);
	matWVP = mul(matWV, g_matProj);

	//==============================================================================

	float4 currentPos = mul(float4(In.vPosition.xyz, 1.f), matWVP);

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
};

// 픽셀의 색을 결정한다.
PS_OUT PS_MOTIONBLUR(PS_MOTIONBLUR_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5 + 0.5;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5 + 0.5;
	//float2 velocity = pow(abs(a - b), 1 / 3.0)*sign(a - b) * 0.5 + 0.5;
	float2 velocity = (a - b) * 0.5 + 0.5;

	velocity = pow(velocity, 3.0);

	float fMinValue = 0.45f;
	if (velocity.x < fMinValue &&
		velocity.y < fMinValue)
		Out.vVelocity.w = 0.f;

	Out.vVelocity = vector(velocity.xy, In.vProjPos.z / In.vProjPos.w, 1.f);
	
	if (!g_bMotionBlur)
		Out.vVelocity.w = 0;
	
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, g_fRimPower);
	Out.vBloomPower = vector(g_fBloomPower,0,0,0);

	return Out;
}

technique Default_Technique
{
	//====================================================================================================
	// 0 - MotionBlur
	//====================================================================================================
	pass MotionBlur
	{
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR();
	}
}

