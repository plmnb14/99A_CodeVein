#include "stdafx.h"
#include "Costume_Hair.h"


CCostume_Hair::CCostume_Hair(_Device pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CCostume_Hair::CCostume_Hair(const CCostume_Hair& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCostume_Hair::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCostume_Hair::Ready_GameObject(void * pArg)
{
	_INFO pInfo = *(_INFO*)pArg;

	m_pmatParent = pInfo.pmatParent;
	m_pmatBone = pInfo.pmatBone;
	m_vColorValue = pInfo.vColorValue;
	m_eHairType = pInfo.eHairType;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Setup_Default()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCostume_Hair::Add_Components()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Hair01", L"Com_StaticMesh", (CComponent**)&m_pStaticMesh)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCostume_Hair::Setup_Default()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Angle(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	return E_NOTIMPL;
}

HRESULT CCostume_Hair::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShader)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 3.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 5.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 0.5f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fRimLightPower = 0.f;	// 림		: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).

	if (FAILED(m_pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================


	return NOERROR;
}

void CCostume_Hair::Calc_AttachBoneTransform()
{
	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	memcpy(&tmpMat._41, &(*m_pmatBone)._41, sizeof(_v3));

	m_pTransform->Calc_ParentMat(&(tmpMat * *m_pmatParent));
}

void CCostume_Hair::Change_HairMesh(HairType _eHairType)
{
	if (_eHairType == m_eHairType)
		return;

	_tchar szMeshName[STR_128] = L"";

	switch (_eHairType)
	{
	case CCostume_Hair::Hair_01:
	{
		lstrcpy(szMeshName, L"Mesh_Hair01");
		break;
	}
	case CCostume_Hair::Hair_02:
	{
		lstrcpy(szMeshName, L"Mesh_Hair02");
		break;
	}
	case CCostume_Hair::Hair_03:
	{
		lstrcpy(szMeshName, L"Mesh_Hair03");
		break;
	}
	case CCostume_Hair::Hair_04:
	{
		lstrcpy(szMeshName, L"Mesh_Hair04");
		break;
	}
	case CCostume_Hair::Hair_05:
	{
		lstrcpy(szMeshName, L"Mesh_Hair05");
		break;
	}
	case CCostume_Hair::Hair_06:
	{
		lstrcpy(szMeshName, L"Mesh_Hair06");
		break;
	}
	case CCostume_Hair::Hair_07:
	{
		lstrcpy(szMeshName, L"Mesh_Hair07");
		break;
	}
	case CCostume_Hair::Hair_08:
	{
		lstrcpy(szMeshName, L"Mesh_Hair08");
		break;
	}
	}

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_StaticMesh");

	// Static 멤버변수는 처음에 Clone 할때 AddRef 해준다., 
	Safe_Release(m_pStaticMesh);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pStaticMesh = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, szMeshName));
	Safe_AddRef(iter->second);

	m_eHairType = _eHairType;
}

_int CCostume_Hair::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);
	Calc_AttachBoneTransform();

	return NO_EVENT;
}

_int CCostume_Hair::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (!m_tObjParam.bInvisible)
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
	}

	else if (m_tObjParam.bInvisible)
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (false == m_bDissolve)
	{
		if (!m_tObjParam.bInvisible)
		{
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;

			if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
				return E_FAIL;
		}
	}

	return NO_EVENT;
}

HRESULT CCostume_Hair::Render_GameObject()
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == m_pShader ||
		nullptr == m_pStaticMesh)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumSubSet = (_uint)m_pStaticMesh->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_iPass = m_pStaticMesh->Get_MaterialPass(i);

		if (m_bDissolve)
			m_iPass = 3;

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pStaticMesh, i);

		m_pShader->Commit_Changes();

		m_pStaticMesh->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	return S_OK;
}

HRESULT CCostume_Hair::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (nullptr == pShader ||
		nullptr == m_pStaticMesh)
		return E_FAIL;

	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_ulong dwNumSubSet = m_pStaticMesh->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pStaticMesh->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================


	return NOERROR;
}

CCostume_Hair * CCostume_Hair::Create(_Device pGraphicDev)
{
	CCostume_Hair* pInstance = new CCostume_Hair(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Create Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCostume_Hair::Clone_GameObject(void * pArg)
{
	CCostume_Hair* pInstance = new CCostume_Hair(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned Hair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCostume_Hair::free()
{
}