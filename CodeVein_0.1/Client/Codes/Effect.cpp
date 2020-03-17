#include "stdafx.h"
#include "..\Headers\Effect.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs.m_pGraphic_Dev)
	, m_iPass(rhs.m_iPass)
	, m_pInfo(rhs.m_pInfo)
{
	m_bClone = true;
}

HRESULT CEffect::Ready_GameObject_Prototype()
{
	// 생성 시, 오래 걸릴 수 있는 작업들을 수행한다.
	return NOERROR;
}

HRESULT CEffect::Ready_GameObject(void* pArg)
{
	// 복제해서 생성 된 후, 추가적으로 필요한 데이터들을 셋팅하낟.
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(V3_NULL);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Angle(_v3(0.f, 0.f, 0.f));

	if (pArg)
		m_pDesc = (EFFECT_DESC*)pArg;
	else
	{
		m_pDesc = new EFFECT_DESC;
		m_pDesc->vWorldPos = { 0.f , 0.f , 0.f };
		m_pDesc->pTargetTrans = nullptr;
	}

	return NOERROR;
}

HRESULT CEffect::LateInit_GameObject()
{

	m_fLifeTime		= m_pInfo->fLifeTime;
	m_vColor		= m_pInfo->vStartColor;
	m_fAlpha		= m_pInfo->fMaxAlpha;
	m_vLerpScale	= m_pInfo->vStartScale;
	m_pTransformCom->Set_Scale(m_pInfo->vStartScale);

	if (m_pInfo->bDistortion)
		m_iPass = 1;

	if (m_pInfo->bRandomMove)
	{
		_v3 vRandDir = _v3(0.5f, 0.f, 0.5f);
		vRandDir = Engine::CCalculater::Random_Dir(180.f, 1.f, 180.f);
		D3DXVec3Normalize(&vRandDir, &vRandDir);
		m_vDir = vRandDir;
	}

	if (m_pInfo->bRandStartPos)
	{
		_v3 vPos = m_pInfo->vStartPos + _v3(m_pInfo->fRansStartPosRange_Min[AXIS_X], m_pInfo->fRansStartPosRange_Min[AXIS_Y], m_pInfo->fRansStartPosRange_Min[AXIS_Z]);
		vPos += Engine::CCalculater::Random_Dir(m_pInfo->fRansStartPosRange_Max[AXIS_X], m_pInfo->fRansStartPosRange_Max[AXIS_Y], m_pInfo->fRansStartPosRange_Max[AXIS_Z]);
		m_pTransformCom->Set_Pos(vPos + m_pDesc->vWorldPos);
	}
	else
		m_pTransformCom->Set_Pos(m_pInfo->vStartPos + m_pDesc->vWorldPos);

	return S_OK;
}

_int CEffect::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(TimeDelta);

	Check_Frame(TimeDelta);
	Check_LifeTime(TimeDelta);

	Check_Move(TimeDelta);
	Check_Alpha(TimeDelta);
	Check_Color(TimeDelta);

	Setup_Billboard();

	return S_OK;
}

_int CEffect::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (m_bIsDead)
		return S_OK;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
		return E_FAIL;

	return S_OK;
}


HRESULT CEffect::Render_GameObject()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(0);
	
	// 버퍼를 렌더링한다.
	// (인덱스버퍼(012023)에 보관하고있는 인덱스를 가진 정점을 그리낟.)
	// 삼각형 두개를 그리낟.각각의 삼각형마다 정점세개, 각각의 정점을 버텍스 셰이더의 인자로 던진다.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}

void CEffect::Setup_Billboard()
{
	_mat matBill, matView, matWorld;

	matWorld = m_pTransformCom->Get_WorldMat();

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	matView = pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixIdentity(&matBill);

	if (!m_pInfo->bOnlyYRot)
	{
		matBill = matView;
		memset(&matBill._41, 0, sizeof(_v3));
		D3DXMatrixInverse(&matBill, NULL, &matBill);
		memcpy(&matBill._41, &matWorld._41, sizeof(_v3));
	
		_float fScale[3]{ m_pTransformCom->Get_Size().x, m_pTransformCom->Get_Size().y, m_pTransformCom->Get_Size().z };
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 4; ++j)
				matBill(i, j) *= fScale[i];
		}
	
		m_pTransformCom->Set_WorldMat(matBill);
	}
	else
	{
		matBill._11 = matView._11;
		matBill._13 = matView._13;
		matBill._31 = matView._31;
		matBill._33 = matView._33;
	
		D3DXMatrixInverse(&matBill, NULL, &matBill);

		_float fScale[3]{ m_pTransformCom->Get_Size().x, m_pTransformCom->Get_Size().y, m_pTransformCom->Get_Size().z };
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 4; ++j)
				matBill(i, j) *= fScale[i];
		}
		m_pTransformCom->Set_WorldMat((matBill * matWorld));
	}

	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	Safe_Release(pManagement);
}

void CEffect::Check_Frame(_double TimeDelta)
{
	m_fFrame += m_pInfo->fMaxFrame * m_pInfo->fAnimSpeed * _float(TimeDelta);
	if (m_pInfo->fMaxFrame != 0.f && m_pInfo->fMaxFrame <= m_fFrame)
	{
		m_bIsDead = true;
	}
}

void CEffect::Check_Move(_double TimeDelta)
{
	if (m_pInfo->bDirMove)
	{
		m_pTransformCom->Add_Pos(m_pInfo->vMoveDirection * m_pInfo->fMoveSpeed * _float(TimeDelta));
	}
	
	if (m_pInfo->bRandomMove)
	{
		_v3 vMove = m_vDir * m_pInfo->fMoveSpeed * _float(TimeDelta);
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
		D3DXVec3Lerp(&m_vLerpScale, &m_vLerpScale, &m_pInfo->vMoveScale, _float(TimeDelta) * m_pInfo->fMoveScaleSpeed);
		m_pTransformCom->Set_Scale(m_vLerpScale);
	}

	if (m_pInfo->bRotMove)
	{
		m_pTransformCom->Add_Angle(AXIS_X, (m_pInfo->vRotDirection.x * _float(TimeDelta) * m_pInfo->fRotSpeed));
		m_pTransformCom->Add_Angle(AXIS_Y, (m_pInfo->vRotDirection.y * _float(TimeDelta) * m_pInfo->fRotSpeed));
		m_pTransformCom->Add_Angle(AXIS_Z, (m_pInfo->vRotDirection.z * _float(TimeDelta) * m_pInfo->fRotSpeed));
	}
}

void CEffect::Check_LifeTime(_double TimeDelta)
{
	m_fLifeTime -= _float(TimeDelta);

	if (0.f >= m_fLifeTime)
		m_bIsDead = true;
}

void CEffect::Check_Alpha(_double TimeDelta)
{
	m_fAlpha -= _float(TimeDelta) * m_pInfo->fAlphaSpeed;

	if (m_fAlpha < 0.f)
		m_fAlpha = 0.f;
	if (m_pInfo->fMaxAlpha > m_fAlpha)
		m_fAlpha = m_pInfo->fMaxAlpha;
}

void CEffect::Check_Color(_double TimeDelta)
{
	if (m_pInfo->bColorMove)
	{
		D3DXVec4Lerp(&m_vColor, &m_vColor, &m_pInfo->vEndColor, _float(TimeDelta) * m_pInfo->fColorSpeed);
	}
}

HRESULT CEffect::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_pInfo->szName, L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_GradientTexture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_pInfo->szGradientName, L"Com_GradientTexture", (CComponent**)&m_pGradientTextureCom)))
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

HRESULT CEffect::SetUp_ConstantTable()
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

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(m_fFrame))))
		return E_FAIL;
	if (FAILED(m_pGradientTextureCom->SetUp_OnShader("g_GradientTexture", m_pShaderCom)))
		return E_FAIL;
	
	m_pShaderCom->Set_Texture("g_DepthTexture", pManagement->Get_Target_Texture(L"Target_Depth"));

	Safe_Release(pManagement);

	return NOERROR;
}

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo)
{
	CEffect*	pInstance = new CEffect(pGraphic_Device);

	pInstance->m_pInfo = pInfo;

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CEffect::Clone_GameObject(void * pArg)
{
	CEffect*	pInstance = new CEffect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pGradientTextureCom);
	Safe_Release(m_pRendererCom);

	Safe_Delete(m_pDesc);
	if(!m_bClone) Safe_Delete(m_pInfo);
	CGameObject::Free();
}
