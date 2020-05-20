#include "stdafx.h"
#include "..\Headers\NPC_Yakumo.h"

#include "Player.h"
#include "UI_Manager.h"

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

	m_pTransformCom->Set_Axis(WORLD_LOOK, AXIS_Z);
	m_pTransformCom->Set_Pos(pInfo.vPos);
	m_pTransformCom->Set_Scale(_v3(1.2f, 1.2f, 1.2f));
	m_pTransformCom->Set_Angle(V3_NULL);
	m_pTransformCom->Set_Angle(AXIS_Y, pInfo.fYAngle);

	
	m_pPlayer = (CPlayer*)g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	m_eState = Idle;
	m_bByeCheck = true; // 시작하자마자 소리 방지

	m_dAniPlayMul = 1.f;

	Ready_BoneMatrix();
	Ready_Collider();

	m_pBattleAgentCom->Set_OriginRimAlpha(0.25f);
	m_pBattleAgentCom->Set_OriginRimValue(7.f);
	m_pBattleAgentCom->Set_RimAlpha(0.25f);
	m_pBattleAgentCom->Set_RimValue(7.f);

	m_pUIManager = CUI_Manager::Get_Instance();
	Safe_AddRef(m_pUIManager);

	return S_OK;
}

HRESULT CNPC_Yakumo::LateInit_GameObject()
{
	// UI
	m_pWeaponShoUI = m_pUIManager->Get_WeaponShopUI();
	Safe_AddRef(m_pWeaponShoUI);
	m_pWeaponShoUI->Set_Target(this);
	m_pWeaponShoUI->Setup_AfterClone();

	m_pInteractionButton = m_pUIManager->Get_NPC_InteractionUI();
	Safe_AddRef(m_pInteractionButton);
	m_pInteractionButton->Set_Active(false);

	m_pScriptUI = CUI_Manager::Get_Instance()->Get_ScriptUI();
	Safe_AddRef(m_pScriptUI);
	m_pScriptUI->Set_Active(false);

	return S_OK;
}

_int CNPC_Yakumo::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(TimeDelta);

	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================
	Check_Dist();
	Check_Anim();
	Check_Bye();

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
	// 샵이 없어도 리턴
	if (!m_pUIManager->Get_WeaponShopUI())
		return;

	// 이미 활성화 되 있으면 리턴
	if (!m_pWeaponShoUI->Get_Active() &&
		!m_pWeaponShoUI->Get_OtherPopupOn() &&
		!m_pInteractionButton->Get_ReactConversation())
	{
		// 거리젠다.
		_float fLen = D3DXVec3Length(&_v3(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos()));

		const _float MIN_DIST = 2.f;

		// 거리 이내가 아닐 경우, 
		if (fLen > MIN_DIST)
		{
			m_pInteractionButton->Set_Active(false);

			// 아이들이 아닐 경우, 아이들 만들어줌
			if (Idle != m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
					m_eState = Idle;
			}

			m_pPlayer->Set_OnNPCUI(false);
			m_pPlayer->Set_YakumoUI(false);

			m_bActive = false;

			return;
		}

		else
		{
			_v3 vPos = TARGET_TO_TRANS(m_pPlayer)->Get_Pos();

			m_fConvertAngle = m_pTransformCom->Chase_Target_Angle(&vPos);
			_float fHitAngle = D3DXToDegree(m_pTransformCom->Calc_HitTarget_Angle(vPos));

			cout << fHitAngle << endl;

			if (fHitAngle >= -30.f && fHitAngle < 30.f)
			{
				// 상호작용 유아이가 뜨고, 플레이어가 누를 수 있게 해줌.
				m_pInteractionButton->Set_Active(true);
				m_pPlayer->Set_OnNPCUI(true);
				m_pPlayer->Set_YakumoUI(true);
				m_bCanActive = true;
			}
		}
	}

	// 플레이어에서 E를 누르면, 리액트 컨버세이션을 활성화 시킨다.
	else if (m_pInteractionButton->Get_ReactConversation() && m_bCanActive == true)
	{
		// 오리진 각도 받아옴
		m_fOriginAngle = m_pTransformCom->Get_Angle(AXIS_Y);

		m_pTransformCom->Set_Angle(AXIS_Y, m_fConvertAngle);

		m_pInteractionButton->Set_Active(false);

		// 최초 1번만 말하고,
		m_bCanActive = false;

		//// 대화 활성화 되어 있고,
		//m_bActive = true;
		//
		//// 새로 들어왔으니, 인사 준비하고,
		//m_bByeCheck = false;

		// 상태 바꿔주고,
		m_eState = Shrug;

		if (0 == CCalculater::Random_Num(0, 1))
		{
			m_pScriptUI->Set_Script(CScriptUI::Talker_Yakumo, CScriptUI::Yakumo_Hello_0_WhatsUp);
			m_pScriptUI->Set_Active(true);
			m_pScriptUI->Set_LifeTime(1.f);

			g_pSoundManager->Stop_Sound(CSoundManager::NPC_Voice_01);
			g_pSoundManager->Play_Sound(L"yakumo_talk_greet01_s_WhatsUp.ogg", CSoundManager::NPC_Voice_01, CSoundManager::Voice_Sound);
		}
		else
		{
			m_pScriptUI->Set_Script(CScriptUI::Talker_Yakumo, CScriptUI::Yakumo_Hello_1_WhatsUp);
			m_pScriptUI->Set_Active(true);
			m_pScriptUI->Set_LifeTime(1.f);

			g_pSoundManager->Stop_Sound(CSoundManager::NPC_Voice_01);
			g_pSoundManager->Play_Sound(L"yakumo_talk_greet04_s_WhatsUp.ogg", CSoundManager::NPC_Voice_01, CSoundManager::Voice_Sound);
		}
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

void CNPC_Yakumo::Check_Bye()
{
	if (!m_pWeaponShoUI->Get_Active() &&
		!m_pWeaponShoUI->Get_OtherPopupOn() &&
		m_pInteractionButton->Get_ReactConversation())
	{
		m_pTransformCom->Set_Angle(AXIS_Y, m_fOriginAngle);

		m_pInteractionButton->Set_ReactConverSation(false);
		//m_bByeCheck = true;

		if (0 == CCalculater::Random_Num(0, 1))
		{
			m_eState = Gloomy;

			m_pScriptUI->Set_Script(CScriptUI::Talker_Yakumo, CScriptUI::Yakumo_Bye_0_SeeYouAround);
			m_pScriptUI->Set_Active(true);
			m_pScriptUI->Set_LifeTime(1.f);

			g_pSoundManager->Stop_Sound(CSoundManager::NPC_Voice_01);
			g_pSoundManager->Play_Sound(L"yakumo_talk_greet01_e_SeeYouAround.ogg", CSoundManager::NPC_Voice_01, CSoundManager::Voice_Sound);
		}
		else
		{
			m_eState = LookDown;

			m_pScriptUI->Set_Script(CScriptUI::Talker_Yakumo, CScriptUI::Yakumo_Bye_1_MakeSure);
			m_pScriptUI->Set_Active(true);
			m_pScriptUI->Set_LifeTime(1.f);

			g_pSoundManager->Stop_Sound(CSoundManager::NPC_Voice_01);
			g_pSoundManager->Play_Sound(L"yakumo_talk_greet02_e_MakeSureYoureReadyForAllEverComes.ogg", CSoundManager::NPC_Voice_01, CSoundManager::Voice_Sound);
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
	Safe_Release(m_pScriptUI);
	Safe_Release(m_pWeaponShoUI);
	Safe_Release(m_pInteractionButton);

	Safe_Release(m_pUIManager);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pBattleAgentCom);
	Safe_Release(m_pOptimizationCom);

	CGameObject::Free();

	return;
}
