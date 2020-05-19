#include "..\Headers\Renderer.h"
#include "PipeLine.h"
#include "Management.h"

//test
#include"TexEffect.h"

_float g_fShadow_X = 64.f;
_float g_fShadow_Y = 36.f;
_float g_fFov = 60.f;
_float g_fNear = 0.1f;
_float g_fFar = 500.f;
_short g_sShadow_X = 3840;
_short g_sShadow_Y = 2160;
//_short g_sShadow_X = 2560;
//_short g_sShadow_Y = 1440;
//_short g_sShadow_X = 1920;
//_short g_sShadow_Y = 1080;
//_short g_sShadow_X = 1280;
//_short g_sShadow_Y = 720;

CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Ready_Component_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_vShadowLightPos = _v3(-50.f, 50, 0.f);

	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Dev->GetViewport(&ViewPort);

	// Target_Diffuse // 
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Diffuse", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Normal
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Depth
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Emissive
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Emissive", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	// Target_Velocity
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Velocity", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_NormalForRim
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_SkinShading", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;
	// Target_BloomPower
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BloomPower", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_DecalDepth
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_DecalDepth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	// 명암을 저장한다.
	// Target_Shade (명암. 내적. 실수값을.)
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Specular
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_SSAO
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_SSAO", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_SSAO
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_SSAO_Blur", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;


	// Target_ShadowMap
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_ShadowMap", g_sShadow_X, g_sShadow_Y, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Shadow ( 원래 D3DFMT_A8R8G8B8 )
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Shadow", g_sShadow_X, g_sShadow_Y, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Distortion
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Distortion", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.2f, 0.2f, 1.f, 0.f))))
		return E_FAIL;

	// Target_Blend
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Blend", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_GodRay
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_GodRay", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Blur
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Blur", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_BlurH
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BlurH", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_BlurV
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BlurV", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_MotionBlurObj
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_MotionBlurObj", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_MotionBlurBlend
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_MotionBlurBlend", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_Bloom (BrightPass)
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Bloom", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_Rim
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Rim", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_ToneMapping
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_ToneMapping", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_BlurDOF
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BlurDOF", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	// Target_BlurSky
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BlurSky", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_DistortionAfter
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_DistortionAfter", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	// Target_DOFAfter
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_DOFAfter", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	// Target_ColorGradingAfter
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_ColorGradingAfter", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// MRT : Multi Render Target 그룹을 지어놓은것. 

	// For.MRT_Deferred : Target_Diffuse + Target_Normal
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Emissive")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_Velocity")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_SkinShading")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_BloomPower")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_DecalDepth")))
		return E_FAIL;

	// For.MRT_LightAcc
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Rim")))
		return E_FAIL;

	// For.MRT_SSAO
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_SSAO", L"Target_SSAO")))
		return E_FAIL;

	// For.MRT_SSAO_Blur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_SSAO_Blur", L"Target_SSAO_Blur")))
		return E_FAIL;
	
	// For.MRT_Distortion
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Distortion", L"Target_Distortion")))
		return E_FAIL;

	// For.MRT_Blend
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blend", L"Target_Blend")))
		return E_FAIL;

	// For.MRT_MotionBlur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_MotionBlurObj", L"Target_MotionBlurObj")))
		return E_FAIL;

	// For.MRT_MotionBlurBlend
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_MotionBlurBlend", L"Target_MotionBlurBlend")))
		return E_FAIL;

	// For.MRT_BrightPass
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_BrightPass", L"Target_Bloom")))
		return E_FAIL;

	// For.MRT_Blur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blur", L"Target_Blur")))
		return E_FAIL;

	// For.MRT_HDR
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_HDR", L"Target_ToneMapping")))
		return E_FAIL;

	// For.RenderOnly_Mesh
	m_pShader_RenderMesh = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_Mesh"));
	if (nullptr == m_pShader_RenderMesh)
		return E_FAIL;

	m_pShader_Trail = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_Effect"));
	if (nullptr == m_pShader_Trail)
		return E_FAIL;

	m_pShader_3DUI = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_3dUI"));
	if (nullptr == m_pShader_3DUI)
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_LightAcc = CShader::Create(m_pGraphic_Dev, L"../ShaderFiles/Shader_LightAcc.fx");
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	// For.Shader_Blend
	m_pShader_Blend = CShader::Create(m_pGraphic_Dev, L"../ShaderFiles/Shader_Blend.fx");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;

	// For.Shader_Shadow
	m_pShader_Shadow = CShader::Create(m_pGraphic_Dev, L"../ShaderFiles/Shader_Shadow.fx");
	if (nullptr == m_pShader_Shadow)
		return E_FAIL;

	// For.Shader_Effect
	m_pShader_Effect = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_Effect"));
	if (nullptr == m_pShader_Effect)
		return E_FAIL;

	// For.Shader_Blur
	m_pShader_Blur = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_Blur"));
	if (nullptr == m_pShader_Blur)
		return E_FAIL;

	// For.SSAO
	m_pShader_SSAO = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_SSAO"));
	if (nullptr == m_pShader_SSAO)
		return E_FAIL;
	
	// For.Buffer_ViewPort
	m_pViewPortBuffer = CBuffer_ViewPort::Create(m_pGraphic_Dev, 0.f - 0.5f, 0.f - 0.5f, (_float)ViewPort.Width, (_float)ViewPort.Height);
	if (nullptr == m_pViewPortBuffer)
		return E_FAIL;

	//m_pViewPortBufferForBlur = CBuffer_ViewPort::Create(m_pGraphic_Dev, 0.f, 0.f, (_float)ViewPort.Width / 4, (_float)ViewPort.Height / 4);


	// 디졸부
	m_pDissolveTexture = CTexture::Create(m_pGraphic_Dev, CTexture::TYPE_GENERAL, L"../../Client/Resources/Texture/Effect/Noise/Noise_13.dds");

	// For.m_pSSAOTexture
	m_pSSAOTexture = CTexture::Create(m_pGraphic_Dev, CTexture::TYPE_GENERAL, L"../../Client/Resources/Texture/Effect/Normal/Normal_4.dds");
	// static_cast<CTexture*>(CComponent_Manager::Get_Instance()->Clone_Component(SCENE_STATIC, L"Tex_Noise", nullptr));

	// For.m_pGradingTexture
	m_pGradingTexture = CTexture::Create(m_pGraphic_Dev, CTexture::TYPE_GENERAL, L"../../Client/Resources/Texture/Grading/LUT/LUT_0.png");
	m_pGradingTextureTest = CTexture::Create(m_pGraphic_Dev, CTexture::TYPE_GENERAL, L"../../Client/Resources/Texture/Grading/LUT/LUT_1.png");
	
	m_iInstanceCnt = 200;
	m_pInstanceData = new INSTANCEDATA[m_iInstanceCnt];

	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


//#ifdef _DEBUG

	//=====================================================================================================================

	_float fTargetSize = 120.f;

	// For.Target_Diffuse`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Diffuse", 0.0f, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Normal`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Normal", 0.0f, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;
	
	// For.Target_Depth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Depth", 0.0f, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Depth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Emissive", 0.0f, fTargetSize * 3, fTargetSize, fTargetSize)))
		return E_FAIL;

	//=====================================================================================================================

	// For.Target_Depth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Velocity", fTargetSize, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_DecalDepth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DecalDepth", fTargetSize, fTargetSize * 1, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_RimNormal`s Debug Buffer ==  툰 쉐이딩
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_SkinShading", fTargetSize, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	//=====================================================================================================================

	// For.Target_Shade`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shade", fTargetSize * 2.f, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_SSAO`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_SSAO", fTargetSize * 2.f, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_SSAO_Blur`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_SSAO_Blur", fTargetSize * 2.f, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Specular`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Specular", fTargetSize * 2.f, fTargetSize * 3, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Rim`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Rim", fTargetSize * 2.f, fTargetSize * 4, fTargetSize, fTargetSize)))
		return E_FAIL;

	//=====================================================================================================================

	// For.Target_ShadowMap`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_ShadowMap", fTargetSize * 3, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Shadow`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow", fTargetSize * 3, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	//=====================================================================================================================

	// For.Target_Distortion`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Distortion", fTargetSize * 4, 0.f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Blur`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Blur", fTargetSize * 4, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_MotionBlur`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_MotionBlurObj", fTargetSize * 4, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	//=====================================================================================================================

	// For.Target_Blend`s Debug Buffer // 톤매핑 직전
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Blend", fTargetSize * 5, 0.f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Bloom`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Bloom", fTargetSize * 5, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_ToneMapping`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_ToneMapping", fTargetSize * 5, fTargetSize * 2.f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_BlurDOF`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_BlurDOF", fTargetSize * 5, fTargetSize * 3, fTargetSize, fTargetSize)))
		return E_FAIL;
	
	//=====================================================================================================================
	// For.Target_DistortionAfter`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DistortionAfter", fTargetSize * 6, 0.f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_DOFAfter`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DOFAfter", fTargetSize * 6, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	//// For.Target_ColorGradingAfter`s Debug Buffer
	//if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_ColorGradingAfter", fTargetSize * 6, fTargetSize * 2, fTargetSize, fTargetSize)))
	//	return E_FAIL;

	//=====================================================================================================================
	// For.Target_GodRay`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_GodRay", fTargetSize * 7, 0, fTargetSize, fTargetSize)))
		return E_FAIL;
//#endif

	return NOERROR;
}

HRESULT CRenderer::Ready_Component(void * pArg)
{

	return NOERROR;
}

HRESULT CRenderer::Add_RenderList(RENDERID eGroup, CGameObject * pGameObject)
{
	if (RENDER_END <= eGroup ||
		nullptr == pGameObject)
		return E_FAIL;

	m_RenderList[eGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return NOERROR;
}

HRESULT CRenderer::Draw_RenderList()
{
	// 디퓨즈, 노멀타겟에 필요한 정보를 그려놓느낟.
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	// VelocityMap, NormalMap For Rim-light
	if (FAILED(Render_MotionBlurTarget()))
		return E_FAIL;

	if (FAILED(Render_ShadowMap()))
		return E_FAIL;

	// 노멀타겟과 빛정보를 이용하여 셰이드타겟에 값을 그리낟.
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	
	//// GodRay
	//if (FAILED(Render_GodRay()))
	//	return E_FAIL;

	// 디퓨즈, 셰이드 두 타겟을 혼합하여 백버퍼에 찍는다.
	// With Skybox(priority), With Alpha
	if (FAILED(Render_Blend()))
		return E_FAIL;

	// Bloom Target을 계산
	if (FAILED(Render_BrightPass()))
		return E_FAIL;

	// Blur
	if (FAILED(Render_Blur()))
		return E_FAIL;

	// 모션 블러 계산
	if (FAILED(Render_MotionBlurObj()))
		return E_FAIL;

	// 블렌드 + 모션블러
	if (FAILED(Render_MotionBlur()))
		return E_FAIL;

	// HDR
	if (FAILED(Render_ToneMapping()))
		return E_FAIL;

	if (FAILED(Render_BlurDOF()))
		return E_FAIL;

	// Distortion 타겟에 그림 // 순서 상관X
	if (FAILED(Render_DistortionTarget()))
		return E_FAIL;

	// 후처리
	if (FAILED(Render_Distortion()))
		return E_FAIL;
	if (FAILED(Render_DOF()))
		return E_FAIL;
	if (FAILED(Render_ColorGrading()))
		return E_FAIL;

	if (FAILED(Render_UI_Back()))
		return E_FAIL;
	if (FAILED(Render_3dUI()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

//#ifdef _DEBUG
	if (CInput_Device::Get_Instance()->Key_Down(DIK_NUMPAD9))
		m_bOnRenderTarget = !m_bOnRenderTarget;

	if (m_bOnRenderTarget)
	{
		if (nullptr == m_pTarget_Manager)
			return E_FAIL;

		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Deferred");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Velocity");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_LightAcc");
		m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_ShadowMap");
		m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_Shadow");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blend");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Distortion");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blur");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_MotionBlurObj");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_HDR");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_BrightPass");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_SSAO");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_SSAO_Blur");
		m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_BlurDOF");
		m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_DecalDepth");
		//m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_GodRay");
		m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_DistortionAfter");
		m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_DOFAfter");
		
		m_pGraphic_Dev->SetTexture(0, nullptr);
	}

//#endif

	return NOERROR;
}

void CRenderer::DOF_On(_bool bOn)
{
	m_bDOF = bOn;

	if (m_bDOF)
	{
		m_fFocus = 0.5f;
		m_fRange = 0.065f;
	}
	else
	{
		m_fFocus = 0.f;
		m_fRange = 0.f;
	}
	
}

void CRenderer::Mono_On(_bool bOn)
{
	m_bMono = bOn;

	//if (m_bMono)
	//	m_fToneGradient = 1.f;
}

void CRenderer::Fog_On(_bool bOn)
{
	m_bFog = bOn;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_PRIORITY].clear();

	m_pTarget_Manager->Begin_Render_Target(L"Target_BlurSky");
	for (auto& pGameObject : m_RenderList[RENDER_FOG])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_FOG].clear();
	m_pTarget_Manager->End_Render_Target(L"Target_BlurSky");


	return NOERROR;
}

HRESULT CRenderer::Render_NonAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	_mat matView, matProj;

	matView = pManagement->Get_Transform(D3DTS_VIEW);
	matProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	m_pShader_RenderMesh->Begin_Shader();

	if (FAILED(m_pShader_RenderMesh->Set_Value("g_matView", &matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader_RenderMesh->Set_Value("g_matProj", &matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShader_RenderMesh)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_Instancing_SetPass(m_pShader_RenderMesh)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}

			Safe_Release(pGameObject);
		}
	}

	m_pShader_RenderMesh->End_Shader();

	m_RenderList[RENDER_NONALPHA].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Deferred")))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CRenderer::Render_ShadowMap()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pTarget_Manager->New_Stencil(L"Target_ShadowMap");

	m_pGraphic_Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	
	_mat matWorld, matView, matProj, matLightVP;

	_v3 vCamPos = CManagement::Get_Instance()->Get_CamPosition();

	_v3 vLightPos = vCamPos + m_vShadowLightPos;
	_v3 vLookAt = vCamPos;

	D3DXMatrixLookAtLH(&matView, &vLightPos, &vLookAt, &WORLD_UP);
	D3DXMatrixOrthoLH(&matProj, g_fShadow_X, g_fShadow_Y, g_fNear, g_fFar);

	matLightVP = matView * matProj;

	m_pShader_Shadow->Set_Value("g_matLightVP", &matLightVP, sizeof(_mat));

	m_pShader_Shadow->Set_Value("g_iScreenX", &g_sShadow_X, sizeof(_int));
	m_pShader_Shadow->Set_Value("g_iScreenY", &g_sShadow_Y, sizeof(_int));

	m_pShader_Shadow->Begin_Shader();

	for (auto& pGameObject : m_RenderList[RENDER_SHADOWTARGET])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetPass(m_pShader_Shadow, 0))) // 그림자 맵 패스로 렌더
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
		}
	}

	m_pShader_Shadow->End_Shader();

	m_pTarget_Manager->Origin_Stencil(L"Target_ShadowMap");

	Render_Shadow();

	return NOERROR;
}

HRESULT CRenderer::Render_Shadow()
{
	m_pTarget_Manager->New_Stencil(L"Target_Shadow");
	m_pGraphic_Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	_float fOffsetX = 0.5f + (0.5f / 1280);
	_float fOffsetY = 0.5f + (0.5f / 720);

	//_float fOffsetX = 0.5f + (0.5f / g_sShadow_X);
	//_float fOffsetY = 0.5f + (0.5f / g_sShadow_Y);

	_mat matScaleBias;
	D3DXMatrixIdentity(&matScaleBias);

	matScaleBias._11 = 0.5f;
	matScaleBias._22 = -0.5f;
	matScaleBias._33 = 1.0f;
	matScaleBias._41 = fOffsetX;
	matScaleBias._42 = fOffsetY;
	matScaleBias._44 = 1.f;

	m_pShader_Shadow->Set_Value("g_matBias", matScaleBias, sizeof(_mat));

	m_pShader_Shadow->Set_Value("g_iScreenX", &g_sShadow_X, sizeof(_int));
	m_pShader_Shadow->Set_Value("g_iScreenY", &g_sShadow_Y, sizeof(_int));

	_mat matWorld, matView, matProj, matLightVP;

	_v3 vCamPos = CManagement::Get_Instance()->Get_CamPosition();

	_v3 vLightPos = vCamPos + m_vShadowLightPos;
	_v3 vLookAt = vCamPos;

	CALC::Generate_ViewMat(&matView, &vLightPos, &vLookAt, &WORLD_UP);
	CALC::Generate_ProjMat_Ortho(&matProj, &g_fShadow_X, &g_fShadow_Y, &g_fNear, &g_fFar);

	D3DXMatrixLookAtLH(&matView, &vLightPos, &vLookAt, &WORLD_UP);
	D3DXMatrixOrthoLH(&matProj, g_fShadow_X, g_fShadow_Y, g_fNear, g_fFar);

	matLightVP = matView * matProj;

	m_pShader_Shadow->Set_Value("g_matLightVP", &matLightVP, sizeof(_mat));

	m_pShader_Shadow->Set_Texture("g_ShadowMapTexture", m_pTarget_Manager->Get_Texture(L"Target_ShadowMap"));

	m_pShader_Shadow->Begin_Shader();

	_uint a = 0;

	for (auto& pGameObject : m_RenderList[RENDER_SHADOWTARGET])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetPass(m_pShader_Shadow, 1))) // 그림자 맵 패스로 렌더
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}

			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_SHADOWTARGET].clear();

	m_pShader_Shadow->End_Shader();

	m_pTarget_Manager->Origin_Stencil(L"Target_Shadow");
	

	return NOERROR;
}

HRESULT CRenderer::Render_MotionBlurTarget()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Velocity")))
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	_mat matView, matProj;

	matView = pManagement->Get_Transform(D3DTS_VIEW);
	matProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	m_pShader_Blur->Begin_Shader();

	if (FAILED(m_pShader_Blur->Set_Value("g_matView", &matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader_Blur->Set_Value("g_matProj", &matProj, sizeof(_mat))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_MOTIONBLURTARGET])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetPass(m_pShader_Blur, 0, true)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}
	
	m_pShader_Blur->End_Shader();

	m_RenderList[RENDER_MOTIONBLURTARGET].clear();


	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Velocity")))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CRenderer::Render_DistortionTarget()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Distortion")))
		return E_FAIL;

	m_pShader_Effect->Begin_Shader();

	_mat matView, matProj;

	matView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	matProj = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);


	if (FAILED(m_pShader_Effect->Set_Value("g_matView", &matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader_Effect->Set_Value("g_matProj", &matProj, sizeof(_mat))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetShader(m_pShader_Effect)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_DISTORTION].clear();

	m_pShader_Effect->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Distortion")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Alpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pShader_Trail->Begin_Shader();

	_mat matView, matProj;

	matView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	matProj = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);


	if (FAILED(m_pShader_Trail->Set_Value("g_matView", &matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader_Trail->Set_Value("g_matProj", &matProj, sizeof(_mat))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA_TRAIL])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_Instancing_SetPass(m_pShader_Trail)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_pShader_Trail->End_Shader();

	m_RenderList[RENDER_ALPHA_TRAIL].clear();

	//===============================================================================

	m_pShader_RenderMesh->Begin_Shader();

	if (FAILED(m_pShader_RenderMesh->Set_Value("g_matView", &matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader_RenderMesh->Set_Value("g_matProj", &matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShader_RenderMesh)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_Instancing_SetPass(m_pShader_RenderMesh)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_pShader_RenderMesh->End_Shader();

	m_RenderList[RENDER_ALPHA].clear();

	//===============================================================================

	m_pShader_3DUI->Begin_Shader();

	if (FAILED(m_pShader_3DUI->Set_Value("g_matView", &matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader_3DUI->Set_Value("g_matProj", &matProj, sizeof(_mat))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_Instancing_SetPass(m_pShader_3DUI)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}
	m_pShader_3DUI->End_Shader();

	m_RenderList[RENDER_ALPHA_UI].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_Effect()
{
	Render_Instance();

	m_pShader_Effect->Begin_Shader();

	for (auto& pGameObject : m_RenderList[RENDER_EFFECT])
	{
		if (nullptr != pGameObject)
		{
			if (CEffect::TYPE_TEX == static_cast<CEffect*>(pGameObject)->Get_EffType())
				continue;

			if (FAILED(pGameObject->Render_GameObject_SetShader(m_pShader_Effect)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	const _int LAYER_COUNT = 10;
	for (_int i = 0; i < LAYER_COUNT; i++)
	{
		for (auto& pGameObject : m_RenderList[RENDER_EFFECT])
		{
			if (CEffect::TYPE_TEX == static_cast<CEffect*>(pGameObject)->Get_EffType() &&
				i == static_cast<CTexEffect*>(pGameObject)->Get_EffectLayer())
			{
				if (FAILED(pGameObject->Render_GameObject_SetShader(m_pShader_Effect)))
				{
					Safe_Release(pGameObject);
					return E_FAIL;
				}
			}
		}
	}

	m_RenderList[RENDER_EFFECT].clear();

	for (auto& pGameObject : m_RenderList[RENDER_ORTHO])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetShader(m_pShader_Effect)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_ORTHO].clear();

	m_pShader_Effect->End_Shader();

	return NOERROR;
}


HRESULT CRenderer::Render_Instance()
{
	m_pShader_Effect->Begin_Shader();

	_int iIdx = 0;
	_ULonglong iSizeCheck = m_RenderList[RENDER_INSTANCE].size();
	ZeroMemory(m_pInstanceData, sizeof(INSTANCEDATA) * m_iInstanceCnt);

	for (auto& pGameObject : m_RenderList[RENDER_INSTANCE])
	{
		if (nullptr != pGameObject)
		{
			CTexEffect* pEff = static_cast<CTexEffect*>(pGameObject);
			m_pInstanceData[iIdx] = pEff->Get_InstanceData();
			Safe_Release(pGameObject);
			++iIdx;
			--iSizeCheck;

			if (iIdx >= m_iInstanceCnt || iSizeCheck == 0)
			{
				CBuffer_RcTex* pBuffer = static_cast<CBuffer_RcTex*>(pGameObject->Get_Component(L"Com_VIBuffer"));
				pBuffer->Render_Before_Instancing(m_pInstanceData, iIdx);

				_mat		ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
				_mat		ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

				if (FAILED(m_pShader_Effect->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
					return E_FAIL;
				if (FAILED(m_pShader_Effect->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
					return E_FAIL;

				D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
				D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

				if (FAILED(m_pShader_Effect->Set_Value("g_matProjInv", &ProjMatrix, sizeof(_mat))))
					return E_FAIL;
				if (FAILED(m_pShader_Effect->Set_Value("g_matViewInv", &ViewMatrix, sizeof(_mat))))
					return E_FAIL;
				m_pShader_Effect->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"));

				pEff->SetUp_ConstantTable_Instance(m_pShader_Effect);

				m_pShader_Effect->Begin_Pass(3);

				m_pShader_Effect->Commit_Changes();

				pBuffer->Render_DrawPrimitive_Instancing();

				m_pShader_Effect->End_Pass();

				pBuffer->Render_After_Instancing();

				ZeroMemory(m_pInstanceData, sizeof(INSTANCEDATA) * m_iInstanceCnt);
				iIdx = 0;
			}
		}
	}

	m_RenderList[RENDER_INSTANCE].clear();

	m_pShader_Effect->End_Shader();

	return NOERROR;
}

HRESULT CRenderer::Render_UI_Back()
{
	for (auto& pGameObject : m_RenderList[RENDER_UI_BACK])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_UI_BACK].clear();

	return NOERROR;
}

_bool Compare_Z(CGameObject* pDest, CGameObject* pSour)
{
	return pDest->Get_ViewZ() > pSour->Get_ViewZ();
}

HRESULT CRenderer::Render_UI()
{
	m_RenderList[RENDER_UI].sort(Compare_Z);

	for (auto& pGameObject : m_RenderList[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_UI].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_3dUI()
{
	for (auto& pGameObject : m_RenderList[RENDER_3DUI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_3DUI].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_SSAO()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_SSAO)
		return E_FAIL;

	m_pSSAOTexture->SetUp_OnShader("g_RandomTexture", m_pShader_SSAO, 0);

	if (FAILED(m_pShader_SSAO->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pShader_SSAO->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_Texture(L"Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pShader_SSAO->Set_Texture("g_SkinTexture", m_pTarget_Manager->Get_Texture(L"Target_SkinShading"))))
		return E_FAIL;
	//

	CPipeLine*		pPipeLine = CPipeLine::Get_Instance();
	if (nullptr == pPipeLine)
		return E_FAIL;

	Safe_AddRef(pPipeLine);

	m_pShader_SSAO->Set_Value("g_matProjInv", &pPipeLine->Get_Transform_Inverse(D3DTS_PROJECTION), sizeof(_mat));
	m_pShader_SSAO->Set_Value("g_matViewInv", &pPipeLine->Get_Transform_Inverse(D3DTS_VIEW), sizeof(_mat));

	//
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_SSAO")))
		return E_FAIL;

	m_pShader_SSAO->Begin_Shader();
	m_pShader_SSAO->Begin_Pass(0);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_SSAO->End_Pass();
	m_pShader_SSAO->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_SSAO")))
		return E_FAIL;

	//
	Safe_Release(pPipeLine);


	// SSAO 블러
	if (FAILED(m_pShader_SSAO->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_Texture(L"Target_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_SSAO_Blur")))
		return E_FAIL;

	m_pShader_SSAO->Begin_Shader();
	m_pShader_SSAO->Begin_Pass(1);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_SSAO->End_Pass();
	m_pShader_SSAO->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_SSAO_Blur")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager ||
		nullptr == m_pLight_Manager)
		return E_FAIL;

	CPipeLine*		pPipeLine = CPipeLine::Get_Instance();
	if (nullptr == pPipeLine)
		return E_FAIL;

	Safe_AddRef(pPipeLine);

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	m_pShader_LightAcc->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_Texture(L"Target_Normal"));
	m_pShader_LightAcc->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"));
	m_pShader_LightAcc->Set_Texture("g_ShadowMapTexture", m_pTarget_Manager->Get_Texture(L"Target_Shadow"));
	m_pShader_LightAcc->Set_Texture("g_RimNormalTexture", m_pTarget_Manager->Get_Texture(L"Target_SkinShading"));
	
	m_pShader_LightAcc->Set_Value("g_matProjInv", &pPipeLine->Get_Transform_Inverse(D3DTS_PROJECTION), sizeof(_mat));
	m_pShader_LightAcc->Set_Value("g_matViewInv", &pPipeLine->Get_Transform_Inverse(D3DTS_VIEW), sizeof(_mat));


	_mat matWorld, matView, matProj, matLightVP;

	_v3 vCamPos = CManagement::Get_Instance()->Get_CamPosition();

	_v3 vLightPos = vCamPos + m_vShadowLightPos;
	_v3 vLookAt = vCamPos;

	D3DXMatrixLookAtLH(&matView, &vLightPos, &vLookAt, &WORLD_UP);
	D3DXMatrixOrthoLH(&matProj, g_fShadow_X, g_fShadow_Y, g_fNear, g_fFar);

	matLightVP = matView * matProj;

	m_pShader_LightAcc->Set_Value("g_matLightVP", &matLightVP, sizeof(_mat));


	_v3 CamPos = pPipeLine->Get_CamPosition();
	m_pShader_LightAcc->Set_Value("g_vCamPosition", &_v4(CamPos, 1.f), sizeof(_v4));

	m_pShader_LightAcc->Begin_Shader();

	m_pLight_Manager->Render_Light(m_pShader_LightAcc);

	m_pShader_LightAcc->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	Safe_Release(pPipeLine);

	return NOERROR;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_Texture(L"Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_SpecularTexture", m_pTarget_Manager->Get_Texture(L"Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_EmissiveTexture", m_pTarget_Manager->Get_Texture(L"Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_RimTexture", m_pTarget_Manager->Get_Texture(L"Target_Rim"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_ShadowMapTexture", m_pTarget_Manager->Get_Texture(L"Target_Shadow"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_Texture(L"Target_SSAO_Blur"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_FogColorTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurSky"))))
		return E_FAIL;

	_float fDestiny = m_fFogDestiny;
	if (!m_bFog)
		fDestiny = 0.f;

	if (FAILED(m_pShader_Blend->Set_Value("g_FogDestiny", &fDestiny, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blend")))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;


	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(0);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	////GodRay Test
	//m_pShader_Blend->Begin_Shader();
	//m_pShader_Blend->Begin_Pass(5);
	//if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_GodRay"))))
	//	return E_FAIL;
	//m_pShader_Blend->Commit_Changes();
	//m_pViewPortBuffer->Render_VIBuffer();
	//m_pShader_Blend->End_Pass();
	//m_pShader_Blend->End_Shader();

	// Alpha
	if (FAILED(Render_Alpha()))
		return E_FAIL;

	// Effect
	if (FAILED(Render_Effect()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blend")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_GodRay()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"))))
		return E_FAIL;

	//_v3 vCamPos = CManagement::Get_Instance()->Get_CamPosition();
	//_v3 vLightPos = vCamPos + m_vShadowLightPos;

	m_pShader_Shadow->Set_Value("g_ScreenLightPos", &m_vShadowLightPos, sizeof(_v3));

	_mat ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	_mat matWVP = ViewMatrix * ProjMatrix;
	m_pShader_Shadow->Set_Value("g_matWVP", &matWVP, sizeof(_mat));

	float decay = 0.96815f;
	float exposure = 0.2f;
	float density = 0.926f;
	float weight = 0.58767f;
	_v4 vParam = _v4(density, decay, weight, exposure);
	m_pShader_Shadow->Set_Value("g_LightShaftValue", &vParam, sizeof(_v4));
	
	
	if (FAILED(m_pTarget_Manager->Begin_Render_Target(L"Target_GodRay")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(9);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_Render_Target(L"Target_GodRay")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_BrightPass()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_BloomTexture", m_pTarget_Manager->Get_Texture(L"Target_BloomPower"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_BrightPass")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(3);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_BrightPass")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Blur()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	// Blur H ==================================================
	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Bloom"))))
		return E_FAIL;

	m_pTarget_Manager->Begin_Render_Target(L"Target_BlurH");

	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(7);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	m_pTarget_Manager->End_Render_Target(L"Target_BlurH");
	// Blur H ==================================================

	for (_int i = 0; i < 13; ++i) // 홀수만
	{
		if (i % 2 == 0)
		{
			m_pTarget_Manager->Begin_Render_Target(L"Target_BlurV");

			if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurH"))))
				return E_FAIL;
			m_pShader_Blend->Begin_Shader();
			m_pShader_Blend->Begin_Pass(8);

			m_pViewPortBuffer->Render_VIBuffer();

			m_pShader_Blend->End_Pass();
			m_pShader_Blend->End_Shader();
			m_pTarget_Manager->End_Render_Target(L"Target_BlurV");
		}
		else
		{
			// Blur H ==================================================
			if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurV"))))
				return E_FAIL;

			m_pTarget_Manager->Begin_Render_Target(L"Target_BlurH");

			m_pShader_Blend->Begin_Shader();
			m_pShader_Blend->Begin_Pass(7);

			m_pViewPortBuffer->Render_VIBuffer();

			m_pShader_Blend->End_Pass();
			m_pShader_Blend->End_Shader();

			m_pTarget_Manager->End_Render_Target(L"Target_BlurH");
			// Blur H ==================================================

		}
	}
	
	// Blur ==================================================
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blur")))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurV"))))
		return E_FAIL;
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(5);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();
	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blur")))
		return E_FAIL;
	// Blur ==================================================

	return NOERROR;
}

HRESULT CRenderer::Render_MotionBlurObj()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	// 오브젝트 단위의 모션블러, 속도맵 ==============================================================================
	m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"));
	m_pShader_Blend->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_Texture(L"Target_Velocity"));
	m_pShader_Blend->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"));

	//_float fCurFrame = _float(CFrameMgr::Get_Instance()->Get_Frame());
	//_float fTargetFrame = 60.f;
	//m_pShader_Blend->Set_Value("g_fCurFrame", &fCurFrame, sizeof(_float));
	//m_pShader_Blend->Set_Value("g_fTargetFrame", &fTargetFrame, sizeof(_float));
	
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_MotionBlurObj")))
		return E_FAIL;

	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(6);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_MotionBlurObj")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_MotionBlur()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_MotionBlurBlend")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(5);

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"))))
		return E_FAIL;
	m_pShader_Blend->Commit_Changes();
	m_pViewPortBuffer->Render_VIBuffer();

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_MotionBlurObj"))))
		return E_FAIL;
	m_pShader_Blend->Commit_Changes();
	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_MotionBlurBlend")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ToneMapping()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_MotionBlurBlend")))) //Target_Blend 임시로 모션블러 꺼둠. 키려면 Target_MotionBlurBlend
		return E_FAIL;

	// Blur
	if (FAILED(m_pShader_Blend->Set_Texture("g_BloomTexture", m_pTarget_Manager->Get_Texture(L"Target_Blur"))))
		return E_FAIL;

	if(!m_bMono)
		(m_fToneGradient <= 0.f) ? m_fToneGradient = 0.f : m_fToneGradient -= DELTA_60;
	else
		(m_fToneGradient >= 0.5f) ? m_fToneGradient = 0.5f : m_fToneGradient += DELTA_60 * 6.f;

	_int iPass = 1;
	if (0.f == m_fToneGradient)
		iPass = 10;
	
	// Tone index
	if (FAILED(m_pShader_Blend->Set_Value("g_iToneIndex", &m_iToneIdx, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Value("g_fToneGradient", &m_fToneGradient, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(iPass);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlurDOF()
{
	if (!m_bDOF)
		return S_OK;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_ToneMapping"))))
		return E_FAIL;

	m_pTarget_Manager->Begin_Render_Target(L"Target_BlurH");

	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(7);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	m_pTarget_Manager->End_Render_Target(L"Target_BlurH");


	for (_int i = 0; i < 13; ++i) // 홀수만
	{
		if (i % 2 == 0)
		{
			m_pTarget_Manager->Begin_Render_Target(L"Target_BlurV");

			if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurH"))))
				return E_FAIL;
			m_pShader_Blend->Begin_Shader();
			m_pShader_Blend->Begin_Pass(8);

			m_pViewPortBuffer->Render_VIBuffer();

			m_pShader_Blend->End_Pass();
			m_pShader_Blend->End_Shader();
			m_pTarget_Manager->End_Render_Target(L"Target_BlurV");
		}
		else
		{
			// Blur H ==================================================
			if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurV"))))
				return E_FAIL;

			m_pTarget_Manager->Begin_Render_Target(L"Target_BlurH");

			m_pShader_Blend->Begin_Shader();
			m_pShader_Blend->Begin_Pass(7);

			m_pViewPortBuffer->Render_VIBuffer();

			m_pShader_Blend->End_Pass();
			m_pShader_Blend->End_Shader();

			m_pTarget_Manager->End_Render_Target(L"Target_BlurH");
			// Blur H ==================================================

		}
	}

	m_pTarget_Manager->Begin_Render_Target(L"Target_BlurDOF");

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurV"))))
		return E_FAIL;

	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(7);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	m_pTarget_Manager->End_Render_Target(L"Target_BlurDOF");
	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_ToneMapping"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DistortionTexture", m_pTarget_Manager->Get_Texture(L"Target_Distortion"))))
		return E_FAIL;

	m_pTarget_Manager->Begin_Render_Target(L"Target_DistortionAfter");

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(11);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	m_pTarget_Manager->End_Render_Target(L"Target_DistortionAfter");

	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_DistortionAfter"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurDOF"))))
		return E_FAIL;

	if (m_fFocus > 1.f) m_fFocus = 1.f;
	if (m_fFocus < 0.f) m_fFocus = 0.f;
	if (m_fRange > 1.f) m_fRange = 1.f;
	if (m_fRange < 0.f) m_fRange = 0.f;

	if (FAILED(m_pShader_Blend->Set_Value("g_Focus_DOF", &m_fFocus, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Value("g_Range_DOF", &m_fRange, sizeof(_float))))
		return E_FAIL;

	m_pTarget_Manager->Begin_Render_Target(L"Target_DOFAfter");

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(12);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	m_pTarget_Manager->End_Render_Target(L"Target_DOFAfter");

	return S_OK;
}

HRESULT CRenderer::Render_ColorGrading()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_DOFAfter"))))
		return E_FAIL;

	if (GetAsyncKeyState('M') & 0x8000)
	{
		m_pGradingTextureTest->SetUp_OnShader("g_GradingTexture", m_pShader_Blend, 0);
	}
	else
	{
		// GradingTexture
		m_pGradingTexture->SetUp_OnShader("g_GradingTexture", m_pShader_Blend, 0);
	}

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(13);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	return S_OK;
}

void CRenderer::Calc_CSM()
{
	////===================================================================================
	//// ViewMatrix 생성
	////===================================================================================
	//
	//_v3 vWorldCenter = V3_NULL;
	//_v3 vPos = vWorldCenter;
	//_v3 vLookAt = vWorldCenter; // + 디렉셔널 방향 * 카메라의  FarClip
	//_v3 vDirectionalDir = WORLD_LOOK;
	//_v3 vUp = WORLD_UP;
	//_v3 vRight = WORLD_RIGHT;
	//
	//CALC::V3_Cross_Normal(&vUp, &vDirectionalDir, &vRight);
	//
	//_mat matShadowView;
	//D3DXMatrixLookAtLH(&matShadowView, &vPos, &vLookAt, &vUp);
	//
	////===================================================================================
	//// Frustum 영역 추출
	////===================================================================================
	//
	//_float fShadowBoundRadius, fRadius;
	//_v3 vShadowBoundCenter;
	//_float arrCascadeRanges[4];
	//_float arrCascadeRadius[4];
	//
	//Calc_FrustumBoundSphere(arrCascadeRanges[0], arrCascadeRanges[3], vShadowBoundCenter, fRadius);
	//fShadowBoundRadius = max(fShadowBoundRadius, fRadius);
	//
	////===================================================================================
	//// ProjMatrix 생성
	////===================================================================================
	//_mat matShadowProj;
	//D3DXMatrixOrthoLH(&matShadowProj, fShadowBoundRadius, fShadowBoundRadius, -fShadowBoundRadius, fShadowBoundRadius);
	//
	////===================================================================================
	//// 플리커, 케스케이드 변환
	////===================================================================================
	//_uint iToltalCascade = 3;
	//_bool bAntiFlicker = true;
	//
	//
	//for (_uint i = 0; i < iToltalCascade; ++i)
	//{
	//	_mat matCascadeTrans, matCascadeScale;
	//
	//	//if (bAntiFlicker)
	//	//{
	//	//	_v3 vNewCenter;
	//	//	Calc_FrustumBoundSphere(arrCascadeRanges[i], arrCascadeRanges[i+1], vNewCenter, fRadius);
	//	//	arrCascadeRadius[i] = max(arrCascadeRadius[i], fRadius);
	//	//
	//	//
	//	//	_v3 vOffset;
	//	//	if()
	//	//}
	//}
}

void CRenderer::Calc_FrustumBoundSphere(_float fNear, _float fFar, _v3 & vBoundCenter, _float & fBoundRadius)
{
	//_mat vCamMatrix = CManagement::Get_Instance()->Get_TransformInverse(D3DTS_VIEW);
	//
	//_v3 vCamRight = *(_v3*)&vCamMatrix.m[0][0];
	//_v3 vCamUp = *(_v3*)&vCamMatrix.m[1][0];
	//_v3 vCamLook = *(_v3*)&vCamMatrix.m[2][0];
	//_v3 vCamPos = *(_v3*)&vCamMatrix.m[3][0];
	//_float fAspect , fFov;
	//
	//fAspect = 720.f / 1280.f;
	//fFov = 60.f;
	//
	//_float fTanFOV_X = tanf(fAspect * fFov);
	//_float fTanFOV_Y = tanf(fAspect);
	//
	//vBoundCenter = vCamPos + vCamLook * (fNear + 0.5f * (fNear + fFar));
	//
	//_v3 vBoundSpan = vCamPos + (-vCamRight * fTanFOV_X + vCamUp * fTanFOV_Y + vCamLook) * fFar - vBoundCenter;
	//fBoundRadius = D3DXVec3Length(&vBoundSpan);
}


CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRenderer*	pInstance = new CRenderer(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("Failed To Creating CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CComponent * CRenderer::Clone_Component(void * pArg)
{
	// 복제를 하려 들면 새로운객체를 만드는것이아니라, 
	// 원형객체의 주소를 그대로 리턴하겠다.

	Safe_AddRef(this);

	// this는 멤버함수안에서만 사용할 수 있으며, 
	// 멤버함수는 호출시에 반드시 객체를 필요로한다.
	// 해서 this는 멤버함수를 호출한 객체의 주소를 의미하낟.
	return this;
}

void CRenderer::Free()
{
	Safe_Delete_Array(m_pInstanceData);
	
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pGradingTexture);
	Safe_Release(m_pGradingTextureTest);
	Safe_Release(m_pSSAOTexture);
	Safe_Release(m_pViewPortBuffer);

	Safe_Release(m_pShader_Trail);
	Safe_Release(m_pShader_RenderMesh);
	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_LightAcc);
	Safe_Release(m_pShader_Shadow);
	Safe_Release(m_pShader_Effect);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_SSAO);
	Safe_Release(m_pShader_3DUI);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	CComponent::Free();
}