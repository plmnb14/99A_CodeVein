#include "stdafx.h"
#include "..\Headers\NPC_Yakumo.h"

#include "WeaponShopUI.h"

CNPC_Yakumo::CNPC_Yakumo(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CNPC_Yakumo::CNPC_Yakumo(const CNPC_Yakumo & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNPC_Yakumo::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Yakumo::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	NPC_INFO pInfo = *(NPC_INFO*)pArg;

	m_pTransformCom->Set_Pos(pInfo.vPos);
	m_pTransformCom->Set_Scale(V3_ONE);
	m_pTransformCom->Set_Angle(AXIS_Y, pInfo.fYAngle);
	
	m_pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	m_eState = Idle;
	
	m_dAniPlayMul = 1.f;

	Ready_BoneMatrix();
	Ready_Collider();

	m_pBattleAgentCom->Set_OriginRimAlpha(0.25f);
	m_pBattleAgentCom->Set_OriginRimValue(7.f);
	m_pBattleAgentCom->Set_RimAlpha(0.25f);
	m_pBattleAgentCom->Set_RimValue(7.f);

	return S_OK;
}

HRESULT CNPC_Yakumo::LateInit_GameObject()
{
	// UI
	m_pWeaponShopUI = static_cast<CWeaponShopUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponShopUI", nullptr));
	m_pWeaponShopUI->Set_Target(this);
	m_pWeaponShopUI->Setup_AfterClone();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pWeaponShopUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	return S_OK;
}

_int CNPC_Yakumo::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::LateInit_GameObject();
	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================

	CGameObject::Update_GameObject(TimeDelta);

	//========================
	Check_Dist();
	Check_Anim();

	m_pMeshCom->SetUp_Animation(m_eState);

	return S_OK;
}

_int CNPC_Yakumo::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);
	
	if (!m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}
	
	else
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}
	
	if (m_bInFrustum)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
	}

	return NO_EVENT;
}

HRESULT CNPC_Yakumo::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
			return E_FAIL;

		_mat matveiwView = g_pManagement->Get_Transform(D3DTS_VIEW);
		_mat matPro = g_pManagement->Get_Transform(D3DTS_PROJECTION);

		m_pShaderCom->Set_Value("g_matView", &matveiwView, sizeof(_mat));
		m_pShaderCom->Set_Value("g_matProj", &matPro, sizeof(_mat));

		if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				m_pShaderCom->Begin_Pass(m_iPass);

				m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				m_pShaderCom->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				m_pShaderCom->End_Pass();
			}
		}

		m_pShaderCom->End_Shader();
	}

	Update_Collider();

	return S_OK;
}

HRESULT CNPC_Yakumo::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				pShader->Begin_Pass(m_iPass);

				pShader->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				pShader->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				pShader->End_Pass();
			}
		}

	}

	Update_Collider();

	return S_OK;
}

HRESULT CNPC_Yakumo::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

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
		_float fBloomPower = 0.5f;
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
	// 쉐이더 실행
	//============================================================================================
	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pMeshCom->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return S_OK;
}

void CNPC_Yakumo::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));

	return;
}

void CNPC_Yakumo::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CNPC_Yakumo::Check_Dist()
{
	if (!m_pWeaponShopUI)
		return;

	_float fLen = D3DXVec3Length(&_v3(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos()));

	const _float MIN_DIST = 1.5f;
	if (fLen <= MIN_DIST && !m_pWeaponShopUI->Get_Active())
		m_bCanActive = true;
	else
	{
		m_bCanActive = false;
		m_bActive = false;

		m_eState = Idle;
	}

	if (!m_bActive &&
		m_bCanActive &&
		g_pInput_Device->Key_Down(DIK_R))
	{
		m_bActive = true;

		m_pWeaponShopUI->Set_Active(true);

		if (0 == CCalculater::Random_Num(0, 1))
			m_eState = Gloomy;
		else
			m_eState = LookDown;
	}

	return;
}

void CNPC_Yakumo::Check_Anim()
{
	_double dAniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eState)
	{
	case Client::CNPC_Yakumo::Idle:
		break;
	case Client::CNPC_Yakumo::Gloomy:
	case Client::CNPC_Yakumo::LookDown:
	case Client::CNPC_Yakumo::Negative:
	case Client::CNPC_Yakumo::Positive:
	case Client::CNPC_Yakumo::Shrug:
	{
		if (Idle != m_eState && m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_eState = Idle;
		}
		break;
	}
	}
}

HRESULT CNPC_Yakumo::Add_Component(void* pArg)
{
	_tchar MeshName[STR_128] = L"";

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Yakumo", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgentCom)))
		return E_FAIL;

	m_pColliderCom->Set_Radius(_v3{ 0.6f, 0.6f, 0.6f });
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CNPC_Yakumo::SetUp_ConstantTable(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 0.1f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.2f;	// ID_B	: B채널 ID 값 , 1이 최대

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 림라이트 값들을 쉐이더에 등록시킴
	//=============================================================================================
	m_pBattleAgentCom->Update_RimParam_OnShader(m_pShaderCom);
	//=============================================================================================

	return S_OK;
}

HRESULT CNPC_Yakumo::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(2);

	_float fRadius;
	CCollider* pCollider = nullptr;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 1.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CNPC_Yakumo::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine1", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CNPC_Yakumo* CNPC_Yakumo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNPC_Yakumo* pInstance = new CNPC_Yakumo(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CNPC_Yakumo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Yakumo::Clone_GameObject(void * pArg)
{
	CNPC_Yakumo* pInstance = new CNPC_Yakumo(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CNPC_Yakumo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Yakumo::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pBattleAgentCom);
	Safe_Release(m_pOptimizationCom);

	CGameObject::Free();

	Safe_Release(m_pOptimizationCom);
	Safe_Release(m_pBattleAgentCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	return;
}
