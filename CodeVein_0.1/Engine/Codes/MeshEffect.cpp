#include "..\Headers\MeshEffect.h"

CMeshEffect::CMeshEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect(pGraphic_Device)
{

}

CMeshEffect::CMeshEffect(const CMeshEffect & rhs)
	: CEffect(rhs.m_pGraphic_Dev)
{
	CEffect::m_pInfo = rhs.m_pInfo;
	m_bClone = true;
}

HRESULT CMeshEffect::Ready_GameObject_Prototype()
{
	// 생성 시, 오래 걸릴 수 있느 작업들을 수행한다.

	// Set Default Info
	if (!m_pInfo)
	{
		m_pInfo = new EFFECT_INFO;
		ZeroMemory(m_pInfo, sizeof(EFFECT_INFO));
	}

	return NOERROR;
}

HRESULT CMeshEffect::Ready_GameObject(void * pArg)
{
	// 복제해서 생성 된 후, 추가적으로 필요한 데이터들을 셋팅하낟.
	if (FAILED(Add_Component()))
		return E_FAIL;

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

	return NOERROR;
}

HRESULT CMeshEffect::LateInit_GameObject()
{
	Setup_Info();
	Change_Mesh(m_pInfo->szName);
	Change_Texture(m_pInfo->szColorName);

	return S_OK;
}

_int CMeshEffect::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(TimeDelta);

	if (m_fCreateDelay > 0.f)
	{
		Check_CreateDelay(TimeDelta);
		return S_OK;
	}

	m_fLinearMovePercent += _float(TimeDelta);

	Check_LifeTime(TimeDelta);

	Check_Move(TimeDelta);
	Check_Alpha(TimeDelta);
	Check_Color(TimeDelta);

	return S_OK;
}

_int CMeshEffect::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	if (m_bIsDead || m_fCreateDelay > 0.f)
		return S_OK;

	RENDERID eGroup = RENDERID::RENDER_ALPHA;
	if (m_iPass == 2)
		eGroup = RENDERID::RENDER_ALPHA;
	else
		eGroup = RENDERID::RENDER_DISTORTION;

	if (FAILED(m_pRendererCom->Add_RenderList(eGroup, this)))
		return E_FAIL;

	return _int();
}

HRESULT CMeshEffect::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_uint iNumSubSet = m_pMeshCom->Get_NumMaterials();

	for (size_t j = 0; j < iNumSubSet; ++j)
	{
		m_pShaderCom->Begin_Pass(m_iPass);

		if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_Texture(_ulong(j), MESHTEXTURE::TYPE_DIFFUSE_MAP))))
			return E_FAIL;

		if (FAILED(m_pTextureCom->SetUp_OnShader("g_ColorTexture", m_pShaderCom, _uint(m_pInfo->fColorIndex))))
			return E_FAIL;

		//if (FAILED(m_pTextureCom->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		//	return E_FAIL;

		m_pShaderCom->Commit_Changes();

		m_pMeshCom->Render_Mesh(_uint(j));

		m_pShaderCom->End_Pass();
	}

	m_pShaderCom->End_Shader();

	return NOERROR;
}

void CMeshEffect::Setup_Info()
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

	m_fLinearMoveSpeed = 0.f;
	m_fLinearMovePercent = 0.f;
	m_vFollowPos = { 0.f, 0.f, 0.f };

	m_bFadeOutStart = false;
	
	if (m_pInfo->bDistortion)
		m_iPass = 1;
	else
		m_iPass = 2; // For Mesh Pass

	if (m_pInfo->bFadeIn)
		m_fAlpha = 0.f;

	if (m_pInfo->bRandScale)
	{
		// rand 설정하면 x 값기준으로 동일하게 조정
		_float fScale = Engine::CCalculater::Random_Num(_int(m_pInfo->vStartScale.x * 50), _int(m_pInfo->vStartScale.x * 100)) * 0.01f;
		_v3 vSize = _v3(fScale, fScale, fScale);

		m_vLerpScale = vSize;
		m_pTransformCom->Set_Scale(vSize);
	}
	else
	{
		m_vLerpScale = m_pInfo->vStartScale;
		m_pTransformCom->Set_Scale(m_pInfo->vStartScale);
	}

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

		vPos += _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_X] * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_Y] * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
			Engine::CCalculater::Random_Num(0, _int(m_pInfo->fRandStartPosRange_Max[AXIS_Z] * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f));

		m_pTransformCom->Set_Pos(vPos + m_pDesc->vWorldPos);
		m_vLerpPos = (vPos + m_pDesc->vWorldPos);
	}
	else
	{
		m_pTransformCom->Set_Pos(m_pInfo->vStartPos + m_pDesc->vWorldPos);
		m_vLerpPos = (m_pInfo->vStartPos + m_pDesc->vWorldPos);
	}

	if (m_pInfo->bRotMove)
	{
		if (m_pInfo->bRandomRot)
		{
			_v3 vPos = _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.x * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.y * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.z * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f));

			m_vRot = vPos;
		}
	}
	else
	{
		if (m_pInfo->bRandomRot)
		{
			_v3 vPos = _v3(Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.x * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.y * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f),
				Engine::CCalculater::Random_Num(0, _int(m_pInfo->vRotDirection.z * 100)) * 0.01f * (Engine::CCalculater::Random_Num(0, 1) ? 1.f : -1.f));

			m_pTransformCom->Set_Angle(_v3(D3DXToRadian(vPos)));
		}
		else
		{
			m_pTransformCom->Set_Angle(_v3(D3DXToRadian(m_pInfo->vRotDirection.x), D3DXToRadian(m_pInfo->vRotDirection.y), D3DXToRadian(m_pInfo->vRotDirection.z)));
		}
	}
}

void CMeshEffect::Check_Move(_double TimeDelta)
{
	if (m_pInfo->bSlowly)
	{
		m_fMoveSpeed -= m_fMoveSpeed * _float(TimeDelta);
		if (m_fMoveSpeed <= 0.f)
			m_fMoveSpeed = 0.f;
	}

	if (m_pInfo->bDirMove)
	{
		if (m_pInfo->bLinearMove)
		{
			D3DXVec3Lerp(&m_vLerpPos, &m_vLerpPos, &m_pInfo->vMoveDirection, m_fLinearMovePercent);

			m_pTransformCom->Set_Pos(m_vLerpPos);
		}
		else
		{
			_v3 vMove = m_pInfo->vMoveDirection * m_fMoveSpeed * _float(TimeDelta);
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

	if (m_pInfo->bGravity)
	{
		m_fAccel += _float(TimeDelta);
		_float fY = (-GRAVITY * m_fAccel * m_fAccel * 0.5f) *  _float(TimeDelta);
		_v3 vPos = m_pTransformCom->Get_Pos();
		vPos.y += fY;
		m_pTransformCom->Set_Pos(vPos);
	}

	if (m_pInfo->bScaleMove)
	{
		D3DXVec3Lerp(&m_vLerpScale, &m_vLerpScale, &m_pInfo->vMoveScale, m_fLinearMovePercent * m_pInfo->fMoveScaleSpeed);
		m_pTransformCom->Set_Scale(m_vLerpScale);
	}

	if (m_pInfo->bRotMove)
	{
		if (m_pInfo->bRandomRot)
		{
			m_pTransformCom->Add_Angle(AXIS_X, ((m_vRot.x) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Y, ((m_vRot.y) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Z, ((m_vRot.z) * _float(TimeDelta) * m_fRotSpeed));
		}
		else
		{
			m_pTransformCom->Add_Angle(AXIS_X, ((m_pInfo->vRotDirection.x) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Y, ((m_pInfo->vRotDirection.y) * _float(TimeDelta) * m_fRotSpeed));
			m_pTransformCom->Add_Angle(AXIS_Z, ((m_pInfo->vRotDirection.z) * _float(TimeDelta) * m_fRotSpeed));
		}
	}

	if (m_pInfo->bMoveWithRot)
	{
		_mat matRotX, matRotY, matRotZ;
		_v3 vAngle = m_pTransformCom->Get_Angle();
		_v3 vDir = vAngle;
		D3DXMatrixIdentity(&matRotX);
		D3DXMatrixIdentity(&matRotY);
		D3DXMatrixIdentity(&matRotZ);

		D3DXMatrixRotationX(&matRotX, D3DXToRadian(vAngle.x));
		D3DXMatrixRotationY(&matRotY, D3DXToRadian(vAngle.y));
		D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(vAngle.z));
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotX);
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
		D3DXVec3TransformNormal(&vDir, &vDir, &matRotZ);

		D3DXVec3Normalize(&vDir, &vDir);

		m_pTransformCom->Add_Pos(m_fMoveSpeed * _float(TimeDelta), vDir);
	}
}

void CMeshEffect::Check_LifeTime(_double TimeDelta)
{
	m_fLifeTime -= _float(TimeDelta);

	if (0.f >= m_fLifeTime)
		m_bIsDead = true;
}

void CMeshEffect::Check_Alpha(_double TimeDelta)
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

void CMeshEffect::Check_Color(_double TimeDelta)
{
	if (m_pInfo->bColorMove)
	{
		D3DXVec4Lerp(&m_vColor, &m_vColor, &m_pInfo->vEndColor, _float(TimeDelta) * m_pInfo->fColorSpeed);
	}
}

void CMeshEffect::Check_CreateDelay(_double TimeDelta)
{
	m_fCreateDelay -= _float(TimeDelta);

	if (m_fCreateDelay < 0.f)
		m_fCreateDelay = 0.f;
}

HRESULT CMeshEffect::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Effect", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_LineRing_0", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_LinePoint", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMeshEffect::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);
	
	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat	ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	_float	fDistortion = 0.2f;

	if (FAILED(m_pShaderCom->Set_Value("g_fDistortion", &fDistortion, sizeof(_float))))
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

	Safe_Release(pManagement);

	return NOERROR;
}

void CMeshEffect::Change_Texture(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Texture");

	Safe_Release(m_pTextureCom);
	Safe_Release(iter->second);

	iter->second = m_pTextureCom = static_cast<CTexture*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(m_pTextureCom);
}

void CMeshEffect::Change_Mesh(const _tchar* _Name)
{
	auto& iter = m_pmapComponents.find(L"Com_Mesh");

	Safe_Release(m_pMeshCom);
	Safe_Release(iter->second);

	iter->second = m_pMeshCom = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _Name));
	Safe_AddRef(m_pMeshCom);
}

CMeshEffect * CMeshEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMeshEffect*	pInstance = new CMeshEffect(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CMeshEffect* CMeshEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo)
{
	CMeshEffect*	pInstance = new CMeshEffect(pGraphic_Device);

	pInstance->m_pInfo = pInfo;

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMeshEffect::Clone_GameObject(void * pArg)
{
	CMeshEffect*	pInstance = new CMeshEffect(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect::Free()
{
	CEffect::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
