#include "..\Headers\TexEffect.h"

CTexEffect::CTexEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{

}

CTexEffect::CTexEffect(const CTexEffect& rhs)
	: CEffect(rhs)
	, m_iPass(rhs.m_iPass)
{
	CEffect::m_pInfo = rhs.m_pInfo;

	m_bClone = true;
}

HRESULT CTexEffect::Ready_GameObject_Prototype()
{
	// 생성 시, 오래 걸릴 수 있는 작업들을 수행한다.

	if (!m_pInfo)
	{
		m_pInfo = new EFFECT_INFO;
		ZeroMemory(m_pInfo, sizeof(EFFECT_INFO));
	}

	return NOERROR;
}

HRESULT CTexEffect::Ready_GameObject(void* pArg)
{
	// 복제해서 생성 된 후, 추가적으로 필요한 데이터들을 셋팅하낟.
	if (FAILED(Add_Component()))
		return E_FAIL;

	// Set Default Info
	m_pTransformCom->Set_Pos(V3_NULL);
	m_pTransformCom->Set_Scale(V3_ONE);
	m_pTransformCom->Set_Angle(V3_NULL);

	if (pArg)
		m_pDesc = (EFFECT_DESC*)pArg;
	else
	{
		m_pDesc = new EFFECT_DESC;
		m_pDesc->vWorldPos = { 0.f , 0.f , 0.f };
		m_pDesc->pTargetTrans = nullptr;
	}

	m_pManagement = CManagement::Get_Instance();
	if (nullptr == m_pManagement)
		return E_FAIL;
	
	//Safe_AddRef(m_pManagement);

	return NOERROR;
}

HRESULT CTexEffect::LateInit_GameObject()
{
	Setup_Info();
	Change_EffectTexture(m_pInfo->szName);
	Change_GradientTexture(m_pInfo->szGradientName);
	Change_ColorTexture(m_pInfo->szColorName);

	return S_OK;
}

_int CTexEffect::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(TimeDelta);

	_int a = 0;

	if (m_fCreateDelay > 0.f)
	{
		Check_CreateDelay(TimeDelta);
		return S_OK;
	}

	m_fLinearMovePercent += _float(TimeDelta);

	Check_Frame(TimeDelta);
	Check_LifeTime(TimeDelta);

	Setup_Billboard();
	Check_Move(TimeDelta);
	Check_Alpha(TimeDelta);
	Check_Color(TimeDelta);


	return S_OK;
}

_int CTexEffect::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (m_bIsDead || m_fCreateDelay > 0.f)
		return S_OK;

	RENDERID eGroup = RENDERID::RENDER_ALPHA;
	if (m_iPass == 0)
		eGroup = RENDERID::RENDER_ALPHA;
	else
		eGroup = RENDERID::RENDER_DISTORTION;

	if (FAILED(m_pRendererCom->Add_RenderList(eGroup, this)))
		return E_FAIL;

	return S_OK;
}


HRESULT CTexEffect::Render_GameObject()
{
	//Render_GameObject_HWInstance();

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;
	
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;
	
	m_pShaderCom->Begin_Shader();
	
	m_pShaderCom->Begin_Pass(m_iPass);
	
	m_pBufferCom->Render_VIBuffer();
	
	m_pShaderCom->End_Pass();
	
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CTexEffect::Render_GameObject_HWInstance()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;
	
	m_pBufferCom->Render_Before_Instancing();

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(m_iPass);
	
	// Set Texture
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	// Begin Pass 사이에 SetTexture 할 경우 바로 적용시키기 위해
	m_pShaderCom->Commit_Changes();

	m_pBufferCom->Render_DrawPrimitive_Instancing();
	
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	m_pBufferCom->Render_After_Instancing();

	return NOERROR;
}

void CTexEffect::Setup_Info()
{
	m_fLifeTime = m_pInfo->fLifeTime;
	m_vColor = m_pInfo->vStartColor;
	m_fAlpha = m_pInfo->fMaxAlpha;
	m_vLerpScale = m_pInfo->vStartScale;
	m_fMoveSpeed = m_pInfo->fMoveSpeed;
	m_fRotSpeed = m_pInfo->fRotSpeed;
	m_fAlphaSpeed = m_pInfo->fAlphaSpeed;
	m_fCreateDelay = m_pInfo->fCreateDelay;
	m_pTransformCom->Set_Scale(m_pInfo->vStartScale);
	m_pInfo->fMoveScaleSpeed = 1.f;

	m_fFrame = 0.f;
	m_fLinearMoveSpeed = 0.f;
	m_fLinearMovePercent = 0.f;
	m_vFollowPos = { 0.f, 0.f, 0.f };

	m_bFadeOutStart = false;

	if (m_pInfo->bDistortion)
		m_iPass = 1;

	if (m_pInfo->bFadeIn)
		m_fAlpha = 0.f;

	if (m_pInfo->bStaticFrame)
		m_fFrame = m_pInfo->fMaxFrame;

	if (m_pInfo->bRandomMove)
	{
		_v3 vRandDir = _v3(1.f, 1.f, 1.f);
		vRandDir = Engine::CCalculater::Random_Dir(m_pInfo->vRandDirectionRange.x, m_pInfo->vRandDirectionRange.y, m_pInfo->vRandDirectionRange.z);
		D3DXVec3Normalize(&vRandDir, &vRandDir);
		m_vDir = vRandDir;
	}

	if (m_pInfo->fMoveSpeed_Max > 0.f)
	{
		m_fMoveSpeed = Engine::CCalculater::Random_Num(0, _int(m_pInfo->fMoveSpeed_Max * 100)) * 0.01f;
		m_fMoveSpeed += _int(m_pInfo->fMoveSpeed_Min);
	}

	if (m_pInfo->fRotSpeed_Max > 0.f)
	{
		m_fRotSpeed = Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRotSpeed_Max * 100)) * 0.01f;
		m_fRotSpeed += _int(m_pInfo->fRotSpeed_Min);
	}

	if (m_pInfo->fAlphaSpeed_Max > 0.f)
	{
		m_fAlphaSpeed = Engine::CCalculater::Random_Num(0, _int(m_pInfo->fAlphaSpeed_Max * 100)) * 0.01f;
		m_fAlphaSpeed += _int(m_pInfo->fAlphaSpeed_Min);
	}

	if (m_pInfo->fCreateDelay_Max > 0.f)
	{
		m_fCreateDelay = Engine::CCalculater::Random_Num(0, _int(m_pInfo->fCreateDelay_Max * 100)) * 0.01f;
		m_fCreateDelay += _int(m_pInfo->fCreateDelay_Min);
	}

	if (m_pInfo->bRandStartPos)
	{
		_v3 vPos = _v3(m_pInfo->fRandStartPosRange_Min[AXIS_X], m_pInfo->fRandStartPosRange_Min[AXIS_Y], m_pInfo->fRandStartPosRange_Min[AXIS_Z]);

		_float fMinus = Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f;
		vPos += _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_X] * 100)) * 0.01f * fMinus,
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_Y] * 100)) * 0.01f * fMinus,
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_Z] * 100)) * 0.01f * fMinus);

		m_pTransformCom->Set_Pos(vPos + m_pDesc->vWorldPos);
		m_vLerpPos = (vPos + m_pDesc->vWorldPos);
	}
	else
	{
		m_pTransformCom->Set_Pos(m_pInfo->vStartPos + m_pDesc->vWorldPos);
		m_vLerpPos = (m_pInfo->vStartPos + m_pDesc->vWorldPos);
	}


	if (m_pInfo->bRandomRot)
	{
		_float fMinus = Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f;
		_v3 vPos = _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.x * 100)) * 0.01f * fMinus,
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.y * 100)) * 0.01f * fMinus,
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.z * 100)) * 0.01f * fMinus);

		m_vRot = vPos;
	}
}

void CTexEffect::Setup_Billboard()
{
	_mat matBill, matView, matWorld;

	matWorld = m_pTransformCom->Get_WorldMat();

	matView = m_pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixIdentity(&matBill);

	if (m_pInfo->bBillBoard)
	{
		matBill = matView;
		memset(&matBill._41, 0, sizeof(_v3));
		D3DXMatrixInverse(&matBill, NULL, &matBill);
		//memcpy(&matBill._41, &m_pTransformCom->Get_Pos(), sizeof(_v3));
		//
		//_float fScale[3]{ m_pTransformCom->Get_Size().x, m_pTransformCom->Get_Size().y, m_pTransformCom->Get_Size().z };
		//for (int i = 0; i < 3; ++i)
		//{
		//	for (int j = 0; j < 4; ++j)
		//		matBill(i, j) *= fScale[i];
		//}

		m_pTransformCom->Set_WorldMat(matBill * matWorld);
	}
	else if (m_pInfo->bOnlyYRot)
	{
		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;

		D3DXMatrixInverse(&matBill, NULL, &matBill);
		//memcpy(&matBill._41, &m_pTransformCom->Get_Pos(), sizeof(_v3));
		//
		//_float fScale[3]{ m_pTransformCom->Get_Size().x, m_pTransformCom->Get_Size().y, m_pTransformCom->Get_Size().z };
		//for (int i = 0; i < 3; ++i)
		//{
		//	for (int j = 0; j < 4; ++j)
		//		matBill(i, j) *= fScale[i];
		//}
		//
		////D3DXMatrixRotationX(&matBill, m_vRot.x);
		////D3DXMatrixRotationY(&matBill, m_vRot.y);
		//D3DXMatrixRotationZ(&matBill, m_vRot.z);

		m_pTransformCom->Set_WorldMat((matBill * matWorld));
	}

	Compute_ViewZ(&m_pTransformCom->Get_Pos());
}

void CTexEffect::Check_Frame(_double TimeDelta)
{
	if (m_pInfo->bStaticFrame)
		return;

	m_fFrame += m_pInfo->fMaxFrame * m_pInfo->fAnimSpeed * _float(TimeDelta);
	if (m_pInfo->fMaxFrame != 0.f && m_pInfo->fMaxFrame <= m_fFrame)
	{
		m_bIsDead = true;
	}
}

void CTexEffect::Check_Move(_double TimeDelta)
{
	if (m_pInfo->bDirMove)
	{
		if (m_pInfo->bLinearMove)
		{
			D3DXVec3Lerp(&m_vLerpPos, &m_vLerpPos, &m_pInfo->vMoveDirection, m_fLinearMovePercent);

			m_pTransformCom->Set_Pos(m_vLerpPos);
		}
		else
			m_pTransformCom->Add_Pos(m_pInfo->vMoveDirection * m_fMoveSpeed * _float(TimeDelta));

	}

	if (m_pInfo->bRandomMove)
	{
		_v3 vMove = m_vDir * m_fMoveSpeed * _float(TimeDelta);
		if (m_pDesc->pTargetTrans)
		{
			_v3 vPos = m_pDesc->pTargetTrans->Get_Pos();
			m_vFollowPos += vMove;
			vPos += m_vFollowPos;
			m_pTransformCom->Set_Pos(vPos);
		}
		else
		{
			m_pTransformCom->Add_Pos(vMove);
		}
	}

	if (m_pInfo->bScaleMove)
	{
		D3DXVec3Lerp(&m_vLerpScale, &m_vLerpScale, &m_pInfo->vMoveScale, m_fLinearMovePercent * m_pInfo->fMoveScaleSpeed);
		m_pTransformCom->Set_Scale(m_vLerpScale);
	}

	if (m_pInfo->bRandomRot)
	{
		m_pTransformCom->Add_Angle(AXIS_X, (m_vRot.x * _float(TimeDelta) * m_fRotSpeed));
		m_pTransformCom->Add_Angle(AXIS_Y, (m_vRot.y * _float(TimeDelta) * m_fRotSpeed));
		m_pTransformCom->Add_Angle(AXIS_Z, (m_vRot.z * _float(TimeDelta) * m_fRotSpeed));
	}
	else
	{
		m_pTransformCom->Add_Angle(AXIS_X, (m_pInfo->vRotDirection.x * _float(TimeDelta) * m_fRotSpeed));
		m_pTransformCom->Add_Angle(AXIS_Y, (m_pInfo->vRotDirection.y * _float(TimeDelta) * m_fRotSpeed));
		m_pTransformCom->Add_Angle(AXIS_Z, (m_pInfo->vRotDirection.z * _float(TimeDelta) * m_fRotSpeed));
	}
}

void CTexEffect::Check_LifeTime(_double TimeDelta)
{
	m_fLifeTime -= _float(TimeDelta);

	if (0.f >= m_fLifeTime)
		m_bIsDead = true;
}

void CTexEffect::Check_Alpha(_double TimeDelta)
{
	if (!m_bFadeOutStart && m_pInfo->bFadeIn)
	{
		m_fAlpha += _float(TimeDelta) * m_fAlphaSpeed;
	}

	if (m_fAlpha >= m_pInfo->fMaxAlpha)
		m_bFadeOutStart = true;

	if (m_bFadeOutStart && m_pInfo->bFadeOut)
	{
		m_fAlpha -= _float(TimeDelta) * m_fAlphaSpeed;
	}

	if (m_fAlpha < 0.f)
		m_fAlpha = 0.f;
	if (m_pInfo->fMaxAlpha < m_fAlpha)
		m_fAlpha = m_pInfo->fMaxAlpha;
}

void CTexEffect::Check_Color(_double TimeDelta)
{
	if (m_pInfo->bColorMove)
	{
		D3DXVec4Lerp(&m_vColor, &m_vColor, &m_pInfo->vEndColor, _float(TimeDelta) * m_pInfo->fColorSpeed);
	}
}

void CTexEffect::Check_CreateDelay(_double TimeDelta)
{
	m_fCreateDelay -= _float(TimeDelta);

	if (m_fCreateDelay < 0.f)
		m_fCreateDelay = 0.f;
}

HRESULT CTexEffect::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_LinePoint", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_GradientTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Gradient", L"Com_GradientTexture", (CComponent**)&m_pGradientTextureCom)))
		return E_FAIL;

	// For.Com_ColorTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Colors", L"Com_ColorTexture", (CComponent**)&m_pColorTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CTexEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_vColor", &m_vColor, sizeof(_v4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Bool("g_bUseColorTex", m_pInfo->bUseColorTex)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Bool("g_bReverseColor", m_pInfo->bRevColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Bool("g_bUseRGBA", m_pInfo->bUseRGBA)))
		return E_FAIL;

	_float fMaskIndex = 0.f;
	if (FAILED(m_pShaderCom->Set_Bool("g_bUseMaskTex", (m_pInfo->fMaskIndex != -1.f))))
		return E_FAIL;

	if((m_pInfo->fMaskIndex != -1.f))
		fMaskIndex = m_pInfo->fMaskIndex;

	//D3DXHANDLE g_HandleTexture = g_pEffect->GetParameterBySemantic(NULL, "TEXTURE");

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(m_fFrame))))
		return E_FAIL;
	if (FAILED(m_pGradientTextureCom->SetUp_OnShader("g_GradientTexture", m_pShaderCom, _uint(fMaskIndex))))
		return E_FAIL;
	if (FAILED(m_pColorTextureCom->SetUp_OnShader("g_ColorTexture", m_pShaderCom, _uint(m_pInfo->fColorIndex))))
		return E_FAIL;

	m_pShaderCom->Set_Texture("g_DepthTexture", pManagement->Get_Target_Texture(L"Target_Depth"));

	Safe_Release(pManagement);

	return NOERROR;
}

void CTexEffect::Change_EffectTexture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CTexEffect::Change_GradientTexture(const _tchar * _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_GradientTexture");

	// 둘 다 해제
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pGradientTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

void CTexEffect::Change_ColorTexture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_ColorTexture");

	Safe_Release(m_pColorTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pColorTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(iter->second);
}

CTexEffect* CTexEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTexEffect*	pInstance = new CTexEffect(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CTexEffect* CTexEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo)
{
	CTexEffect*	pInstance = new CTexEffect(pGraphic_Device);

	pInstance->m_pInfo = pInfo;

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTexEffect::Clone_GameObject(void* pArg)
{
	CTexEffect*	pInstance = new CTexEffect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexEffect::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColorTextureCom);
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(m_pRendererCom);

	//Safe_Release(m_pManagement);

	Safe_Delete(m_pDesc);
	CEffect::Free();
}
