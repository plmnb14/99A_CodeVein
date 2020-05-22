#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Weapon.h"
#include "Drain_Weapon.h"
#include "Dummy_Target.h"
#include "StageAgent.h"

#include "ScriptManager.h"

#include "Costume_Hair.h"
#include "Costume_Outer.h"

#include "LockOn_UI.h"

float g_OriginCamPos = 3.f;

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{

}

void CPlayer::Set_WeaponSlot(ACTIVE_WEAPON_SLOT eType, WEAPON_DATA eData)
{
	if (WEAPON_DATA::WPN_DATA_End == eData)
	{
		if (m_pWeapon[eType])
			Safe_Release(m_pWeapon[eType]);

		if(m_eActiveSlot == eType)

		m_bWeaponActive[eType] = false;
		m_bWeaponActive[!eType] = true;
		m_eActiveSlot = (ACTIVE_WEAPON_SLOT)(!eType);

		return;
	}

	// 해당 슬롯의 무기를 교체 해 줍니다.
	if (m_pWeapon[eType])
		Safe_Release(m_pWeapon[eType]);

	// 뼈를 찾고 갖다 붙힙니다.
	LPCSTR tmpChar = "RightHandAttach";
	_mat   matAttach;

	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);

	WPN_PARAM tmpParam = m_pUIManager->Get_Weapon_Inven()->Get_UseWeaponParam(eType);

	m_pWeapon[eType] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon[eType]->Set_WeaponParam(tmpParam, tmpParam.iWeaponName);
	m_pWeapon[eType]->Change_WeaponData(eData);
	m_pWeapon[eType]->Set_Friendly(true);
	m_pWeapon[eType]->Set_AttachBoneMartix(&pFrame->CombinedTransformationMatrix);
	m_pWeapon[eType]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());

	// 메인 슬롯이라면
	if (WPN_SLOT_A == eType)
	{
		m_eMainWpnState = m_pWeapon[eType]->Get_WeaponType();

		m_bOneHand = (
			m_eMainWpnState == WEAPON_SSword ? true :
			m_eMainWpnState == WEAPON_LSword ? false :
			m_eMainWpnState == WEAPON_Halberd ? true :
			m_eMainWpnState == WEAPON_Gun ? true :
			m_eMainWpnState == WEAPON_Hammer ? false : true
			);
	}

	// 메인 슬롯이라면
	else if (WPN_SLOT_B == eType)
	{
		m_eSubWpnState = m_pWeapon[eType]->Get_WeaponType();
	}
}

void CPlayer::Set_ArmorSlot(ARMOR_All_DATA eType, _float fMaxHP)
{
	//if (m_pOuter->Get_OuterType() + 1 == eType)
	//	return;

	m_pOuter->Change_OuterMesh(CClothManager::Cloth_Dynamic(eType + 1));
	m_tObjParam.fHp_Max = fMaxHP;
	if (fMaxHP < m_tObjParam.fHp_Cur)
		m_tObjParam.fHp_Cur = fMaxHP;
}

void CPlayer::Set_Target_UI(CGameObject * pGameObject)
{

}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	Ready_BoneMatrix();
	Ready_Collider();

	Ready_Weapon();
	Ready_DrainWeapon();

	Ready_Skills();

	Ready_Masks();
	Ready_Heads();

	Ready_Rigid();

	m_pBattleAgent->Set_OriginRimAlpha(0.25f);
	m_pBattleAgent->Set_OriginRimValue(7.f);
	m_pBattleAgent->Set_RimAlpha(0.25f);
	m_pBattleAgent->Set_RimValue(7.f);

	//m_pBattleAgent->Set_OriginRimAlpha(1.f);
	//m_pBattleAgent->Set_OriginRimValue(2.f);
	//m_pBattleAgent->Set_RimAlpha(1.f);
	//m_pBattleAgent->Set_RimValue(2.f);

	m_pUIManager = CUI_Manager::Get_Instance();
	Safe_AddRef(m_pUIManager);

	m_pCamManager = CCameraMgr::Get_Instance();
	Safe_AddRef(m_pCamManager);

	m_pStageAgent = CStageAgent::Get_Instance();
	Safe_AddRef(m_pStageAgent);

	m_tObjParam.sMana_Cur = 20;
	m_tObjParam.sMana_Max = 20;
	m_tObjParam.fStamina_Max = 100.f;
	m_tObjParam.fStamina_Cur = 100.f;

	m_fStaminaCost_Dodge = 20.f;
	m_fStaminaCost_WeakAttack = 7.f;
	m_fStaminaCost_HeavyAttack = 12.f;
	m_fStamina_RecoverySpeed = 15.f;

	m_pHair = (CCostume_Hair*)g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Hair", 
		&CCostume_Hair::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 0.f, 0.f, 1.f)));

	m_pOuter = (CCostume_Outer*)g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Outer", 
		&CCostume_Outer::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(0.f, 0.f, 0.f, 0.f), nullptr));

	m_pOuter->Change_OuterMesh(CClothManager::Gauntlet_01);


	m_pLockOn_UI = (CLockOn_UI*)g_pManagement->Clone_GameObject_Return(L"GameObject_LockOn_UI", nullptr);

	m_pScriptManager = CScriptManager::Get_Instance();
	Safe_AddRef(m_pScriptManager);

	m_pScreenCornerEffect = (CHitCheckUI*)g_pManagement->Clone_GameObject_Return(L"GameObject_HitCheckUI", nullptr);
	m_pScreenCornerFade = (CFadeCornerUI*)g_pManagement->Clone_GameObject_Return(L"GameObject_FadeCornerUI", nullptr);
	m_pScreenCornerFade->Set_Enable(true);

	return NOERROR;
}

_int CPlayer::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Check_Cheat();

	m_pScreenCornerFade->Update_GameObject(TimeDelta);


	if (g_pInput_Device->Key_Down(DIK_Y))
	{
		//cout << "===================================================" << endl;
		//cout << m_pTransform->Get_Pos().x << endl;
		//cout << m_pTransform->Get_Pos().y << endl;
		//cout << m_pTransform->Get_Pos().z << endl;
		//cout << "===================================================" << endl;
		//cout << D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)) << endl;
		//cout << "===================================================" << endl;

	}

	// 머리 위치 업데이트
	CGameObject::Update_GameObject(TimeDelta);

	KeyInput();

	Parameter_HitCheck();
	Parameter_YPos(TimeDelta);
	Parameter_Movement();
	Parameter_State();
	Parameter_HeavyCharging();
	Parameter_Collision();
	Parameter_Aiming();

	Check_Mistletoe();
	Check_Stamina(TimeDelta);

	IF_NOT_NULL(m_pWeapon[m_eActiveSlot])
		m_pWeapon[m_eActiveSlot]->Update_GameObject(TimeDelta);

	IF_NOT_NULL(m_pDrainWeapon)
		m_pDrainWeapon->Update_GameObject(TimeDelta);

	m_pNavMesh->Goto_Next_Subset(m_pTransform->Get_Pos(), nullptr);

	m_pScriptManager->Update_ScriptMgr(TimeDelta, m_pNavMesh->Get_SubSetIndex(), m_pNavMesh->Get_CellIndex());

	m_pScreenCornerEffect->Update_GameObject(TimeDelta);

	FootWalkTimerUpdate(TimeDelta);

	return NO_EVENT;
}

_int CPlayer::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRenderer ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;


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

	Reset_BloodSuck_Options();	

	m_pOuter->Late_Update_GameObject(TimeDelta);
	if (!m_tObjParam.bInvisible)
	{
		m_pHair->Late_Update_GameObject(TimeDelta);
		m_pHead[m_eHeadType]->Late_Update_GameObject(TimeDelta);
		m_pMask[m_eMaskType]->Late_Update_GameObject(TimeDelta);
	}

	m_pDynamicMesh->SetUp_Animation_Lower(m_eAnim_Lower , m_bOffLerp);
	m_pDynamicMesh->SetUp_Animation_Upper(m_eAnim_Upper , m_bOffLerp);
	m_pDynamicMesh->SetUp_Animation_RightArm(m_eAnim_RightArm , m_bOffLerp);
	m_pDynamicMesh->SetUp_Animation_LeftArm(m_eAnim_LeftArm, m_bOffLerp);

	m_pOuter->Set_AnimMultiply(m_fAnimMutiply);
	m_pOuter->Set_LowerAnimation(m_eAnim_Lower, m_bOffLerp);
	m_pOuter->Set_UpperAnimation(m_eAnim_Upper, m_bOffLerp);
	m_pOuter->Set_LeftArmAnimation(m_eAnim_LeftArm, m_bOffLerp);
	m_pOuter->Set_RightArmAnimation(m_eAnim_RightArm, m_bOffLerp);

	IF_NOT_NULL(m_pWeapon[m_eActiveSlot])
		m_pWeapon[m_eActiveSlot]->Late_Update_GameObject(TimeDelta);

	IF_NOT_NULL(m_pDrainWeapon)
		m_pDrainWeapon->Late_Update_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CPlayer::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	_double dDeltaTime = g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	m_pDynamicMesh->Play_Animation_Lower(dDeltaTime * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_Upper(dDeltaTime * m_fAnimMutiply);
	m_pDynamicMesh->Play_Animation_RightArm(dDeltaTime * m_fAnimMutiply, !m_bOneHand);
	m_pDynamicMesh->Play_Animation_LeftArm(dDeltaTime * m_fAnimMutiply);

	m_pHead[m_eHeadType]->Update_GameObject(dDeltaTime);
	m_pMask[m_eMaskType]->Update_GameObject(dDeltaTime);

	// 머리 위치 업데이트

	m_pHair->Update_GameObject(dDeltaTime * m_fAnimMutiply, (m_eActState != ACT_Idle) && (m_eActState != ACT_Walk) && (m_eActState != ACT_Run) && (m_eActState != ACT_MoveDelay));
	m_pOuter->Update_GameObject(dDeltaTime * m_fAnimMutiply, (m_eActState != ACT_Idle) && (m_eActState != ACT_Walk) && (m_eActState != ACT_Run) && (m_eActState != ACT_MoveDelay));

	if (m_tObjParam.bInvisible)
		return S_OK;

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;


	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

	_bool bOnToonRimLight = true;

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

		m_pDynamicMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pDynamicMesh->Get_MaterialPass(i, j);

			if (m_bDissolve)
				m_iPass = 3;

			else if (m_iPass == 13)
			{
				bOnToonRimLight = true;

				_float fRimValue = 1.5f;
				_float fRimAlpha = 1.f;

				pShader->Set_Value("g_bToonRimLight", &bOnToonRimLight, sizeof(_bool));

				pShader->Set_Value("g_fRimAlpha", &fRimAlpha, sizeof(_float));
				pShader->Set_Value("g_fRimPower", &fRimValue, sizeof(_float));
			}

			else
			{
				bOnToonRimLight = false;
				pShader->Set_Value("g_bToonRimLight", &bOnToonRimLight, sizeof(_bool));

				m_pBattleAgent->Update_RimParam_OnShader(pShader);
			}

			pShader->Begin_Pass(m_iPass);

			pShader->Set_DynamicTexture_Auto(m_pDynamicMesh, i, j);

			pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	bOnToonRimLight = false;
	pShader->Set_Value("g_bToonRimLight", &bOnToonRimLight, sizeof(_bool));

	return NOERROR;
}

HRESULT CPlayer::Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == pShader ||
		nullptr == m_pDynamicMesh)
		return E_FAIL;

	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
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

		_float fBloomPower = 20.f;
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
	_uint iNumMeshContainer = _uint(m_pDynamicMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamicMesh->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pDynamicMesh->Get_MaterialPass(i, j);

			if (_bIsForMotionBlur)
			{
				if (13 == tmpPass)
				{
					// 피부
					pShader->Begin_Pass(1);
				}

				else
				{
					pShader->Begin_Pass(iPass);
				}
			}

			else
			{
				pShader->Begin_Pass(iPass);
			}

			pShader->Commit_Changes();

			m_pDynamicMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return S_OK;
}

void CPlayer::Reset_OldAnimations()
{
	m_pDynamicMesh->Reset_OldIndx(1);
	m_pOuter->Reset_OldAniIdx(1);
}

void CPlayer::Teleport_ResetOptions(_int _eSceneID, _int _eTeleportID)
{
	m_bIsDead = false;
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.sMana_Cur = m_tObjParam.sMana_Max;
	m_tObjParam.fStamina_Cur = m_tObjParam.fStamina_Max;

	_v3 vShadowLightPos = V3_NULL;
	_v3 vPos = V3_NULL;
	_float fAngle = 0.f;
	_float fRadian = 0.f;

	// 텔레포트 할때는 항상 소환 상태
	m_eActState = ACT_Summon;

	Reset_BattleState();
	Reset_BloodSuck_Options();

	// 위치 , 방향 설정
	switch (_eSceneID)
	{
	case SCENE_STAGE_TRAINING:
	{
		vShadowLightPos = _v3(100.f, 50.f, 0.f);
		break;
	}

	case SCENE_STAGE_BASE:
	{
		vShadowLightPos = _v3(100.f, 50.f, 0.f);

		vPos = _eTeleportID == TeleportID_Tutorial ?
			V3_NULL : _v3(-0.519f, 0.120f, 23.810f);

		fAngle = _eTeleportID == TeleportID_Tutorial ?
			0.f :180.f;

		break;
	}

	case SCENE_STAGE_01:
	{
		vShadowLightPos = _v3(-100.f, 50.f, 0.f);

		vPos = _eTeleportID == TeleportID_St01_1 ? _v3(150.484f, -18.08f, 70.417f) :
			_eTeleportID == TeleportID_St01_2 ? V3_NULL : V3_NULL;

		fAngle = _eTeleportID == TeleportID_St01_1 ? 0.f :
			_eTeleportID == TeleportID_St01_2 ? 0.f : 0.f;

		break;
	}

	case SCENE_STAGE_02:
	{
		vShadowLightPos = _v3(-100.f, 50.f, 0.f);

		vPos = _eTeleportID == TeleportID_St02_1 ? _v3(-3.295f, -0.f, -193.89f) :
			_eTeleportID == TeleportID_St02_2 ? V3_NULL : V3_NULL;

		fAngle = _eTeleportID == TeleportID_St02_1 ? 0.f :
			_eTeleportID == TeleportID_St02_2 ? 0.f : 0.f;

		break;
	}

	case SCENE_STAGE_03:
	{
		vShadowLightPos = _v3(-100.f, 50.f, 0.f);

		vPos = _eTeleportID == TeleportID_St03_1 ?
			_v3(52.610f, -13.0f, 3.575f) : V3_NULL;

		fAngle = _eTeleportID == TeleportID_St03_1 ?
			0.f : 0.f;

		break;
	}

	case SCENE_STAGE_04:
	{
		vShadowLightPos = _v3(-100.f, 50.f, 0.f);

		vPos = _eTeleportID == TeleportID_St04_1 ?
			_v3(42.504f, -3.85f, 75.683f) : V3_NULL;

		fAngle = _eTeleportID == TeleportID_St04_1 ?
			0.f : 0.f;

		break;
	}
	}

	m_pRenderer->Set_ShadowLightPos(vShadowLightPos);

	fRadian = D3DXToRadian(fAngle);
	m_pTransform->Set_Pos(vPos);
	m_pTransform->Set_Angle(AXIS_Y, fRadian);

	_tchar szNavMeshName[STR_128] = L"";

	lstrcpy(szNavMeshName, 
		(_eSceneID == SCENE_STAGE_BASE ? L"Navmesh_Stage_00.dat" :
			_eSceneID == SCENE_STAGE_01 ? L"Navmesh_Stage_01.dat" : 
			_eSceneID == SCENE_STAGE_02 ? L"Navmesh_Stage_02.dat" : 
			_eSceneID == SCENE_STAGE_03 ? L"Navmesh_Stage_03.dat" : 
			_eSceneID == SCENE_STAGE_04 ? L"Navmesh_Stage_04.dat" :  L"Navmesh_Training.dat"));

	m_pNavMesh->Reset_NaviMesh();
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, szNavMeshName);
	m_pNavMesh->Check_OnNavMesh(vPos);

	CCameraMgr::Get_Instance()->Set_LockAngleX(fAngle);
	CCameraMgr::Get_Instance()->Set_CamView(BACK_VIEW);
}

void CPlayer::FootWalkTimerUpdate(_double _deltaTime)
{
	if (m_eActState == ACT_Walk || m_eActState == ACT_Run || m_eActState == ACT_Dash)
	{
		if (false == m_bCanPlayWalkSound)
		{
			m_fFootWalkTimer_Cur += _deltaTime;

			if (m_fFootWalkTimer_Cur >= m_fFootWalkTimer_Max)
			{
				m_fFootWalkTimer_Cur = m_fFootWalkTimer_Max;
				m_bCanPlayWalkSound = true;
			}
		}
	}
	else
	{
		m_bCanPlayWalkSound = true;
		m_fFootWalkTimer_Cur = 0.f;
		m_fFootWalkTimer_Max = 0.5f;
	}
}

void CPlayer::Parameter_State()
{
	switch (m_eActState)
	{
	case ACT_Summon:
	{
		Play_Summon();
		break;
	}

	case ACT_Disappear:
	{
		Play_Idle();
		break;
	}

	case ACT_Idle:
	{
		Play_Idle();
		break;
	}

	case ACT_Mistoletoe :
	{
		Play_Idle();
		break;
	}

	case ACT_Run:
	{
		Play_Run();
		break;
	}

	case ACT_Dash:
	{
		Play_Dash();
		break;
	}

	case ACT_MoveDelay:
	{
		Play_MoveDelay();
		break;
	}

	case ACT_Dodge:
	{
		Play_Dodge();
		break;
	}

	case ACT_WeakAtk:
	{
		Play_WeakAtk();
		break;
	}

	case ACT_HeavyAtk:
	{
		Play_HeavyAtk();
		break;
	}

	case ACT_Skill:
	{
		Play_Skills();
		break;
	}

	case ACT_BloodSuck:
	{
		Play_BloodSuck();
		break;
	}

	case ACT_BloodSuck_Count:
	{
		Play_BloodSuckCount();
		break;
	}

	case ACT_BloodSuck_Combo:
	{
		Play_BloodSuckCombo();
		break;
	}

	case ACT_BloodSuck_Execution:
	{
		Play_BloodSuckExecution();
		break;
	}

	case ACT_Buff:
	{
		Play_Buff();
		break;
	}

	case ACT_Hit:
	{
		Play_Hit();
		break;
	}

	case ACT_Dead:
	{
		Play_Dead();
		break;
	}

	case ACT_WeaponChange:
	{
		Play_WeaponChange();
		break;
	}

	case ACT_PickUp:
	{
		Play_PickUp();
		break;
	}

	case ACT_Down:
	{
		Play_Down();
		break;
	}
	}
}

void CPlayer::Parameter_State_Upper()
{
}

void CPlayer::Parameter_Atk()
{
}

void CPlayer::Parameter_Movement()
{
	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (m_eActState == ACT_Down)
		return;

	if (m_eActState == ACT_Hit)
		return;

	if (m_eActState == ACT_HeavyAtk)
		return;

	if (m_eActState == ACT_WeakAtk)
		return;

	if (m_eActState == ACT_Skill)
		return;

	if (m_eActState == ACT_BloodSuck_Count)
		return;

	if (m_eActState == ACT_BloodSuck_Combo)
		return;

	if (m_eActState == ACT_BloodSuck)
		return;

	if (m_bOnDodge)
	{
		if (m_pTarget != nullptr)
		{
			_float fAngle = D3DXToDegree(m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos()));
			m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(fAngle));
		}

		return;
	}

	if (m_bOnPickUp)
		return;

	_float fMoveSpeed = m_tInfo.fMoveSpeed_Cur * DELTA_60;
	_float fRadian = 0.f;
	_float fRecover = 1440.f;
	_float fAngle = 0.f;
		
	if (false == m_bOnAttack)
		fAngle = (m_pTarget != nullptr ?
			D3DXToDegree(m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos())) :
			m_pCamManager->Get_XAngle());

	else
		fAngle = D3DXToDegree(m_pTransform->Get_Angle().y);

	//if (m_bOnAiming)
	//	fMoveSpeed = 0.f;

	if (m_bOnAiming && m_pTarget != nullptr)
		Movement_Aiming(fAngle, fMoveSpeed);

	else
		Movement_NonAiming(fRecover, fAngle, fRadian, fMoveSpeed);
}

void CPlayer::Parameter_HeavyCharging()
{
	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (m_eActState == ACT_Hit)
		return;

	if (m_bCharging)
	{
		m_fChargeTimer_Cur += DELTA_60;
	}
}

void CPlayer::Parameter_YPos(_double dTimeDelta)
{
	if(m_pRigid->Get_IsFall() == false)
		m_pTransform->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransform->Get_Pos()));
	else
	{
		if (m_FallingTimer > 1.5f)
		{
			Teleport_ResetOptions(g_eSceneID_Cur, g_eSTeleportID_Cur);

			m_pRigid->Set_IsFall(false);
			m_FallingTimer = 0.f;
		}
		else
		{
			m_FallingTimer += _float(dTimeDelta);

			_float fYSpeed = m_pRigid->Set_Fall(m_pTransform->Get_Pos(), _float(dTimeDelta));

			D3DXVECTOR3 JumpLength = { 0, -fYSpeed, 0 };
			m_pTransform->Add_Pos(JumpLength);
		}
	}
}

void CPlayer::Parameter_Collision()
{
	// 콜라이더 위치 업데이트
	Update_Collider();

	// 밀리는거 관련
	OnCollisionEnter();
}

void CPlayer::Parameter_Aiming()
{
	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (true == m_bOnAiming)
	{
		if (nullptr != m_pTarget)
		{
			if (m_pTarget->Get_Target_Hp() <= 0.f)
			{
				m_pTarget = nullptr;
				m_bHaveAimingTarget = false;
				m_bOnAiming = false;

				m_pCamManager->Set_AimingTarget(nullptr);
				m_pCamManager->Set_OnAimingTarget(false);
				m_pCamManager->Set_LockAngleX(D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)));

				//==========================================================================
				Active_UI_LockOn(true);
				//==========================================================================

				return;
			}
		}

		Target_AimChasing();

		if (nullptr != m_pTarget)
		{
			m_pTransform->Set_Angle(AXIS_Y, m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(m_pTarget)->Get_Pos()));

			// 디졸브 쓸라면 수정
			m_pLockOn_UI->Update_GameObject(0);
		}
	}

	else if (false == m_bOnAiming)
	{
		if (nullptr != m_pTarget)
		{
			m_pTarget = nullptr;
			m_bHaveAimingTarget = false;

			m_pCamManager->Set_AimingTarget(m_pTarget);
			m_pCamManager->Set_OnAimingTarget(false);

			//==========================================================================
			Active_UI_LockOn(true);
			//==========================================================================
		}
	}
}

void CPlayer::Parameter_HitCheck()
{
	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (m_eActState == ACT_Down)
		return;

	// 플레이어가 외부에서 맞고
	if (false == m_tObjParam.bCanHit)
	{
		if (false == m_tObjParam.bIsHit)
		{
			if (m_tObjParam.fHp_Cur <= 0)
			{
				m_eActState = ACT_Dead;
			}

			else if (m_tObjParam.fHp_Cur > 0)
			{
				if (m_eActState != ACT_Hit)
				{
					m_bOnMoveDelay = false;

					m_eActState = ACT_Hit;
				}
			}
		}

		else if (true == m_tObjParam.bIsHit)
		{
			if (m_tObjParam.bHitAgain)
			{
				m_eActState = ACT_Hit;

				m_bOnMoveDelay = false;
				m_tObjParam.bHitAgain = false;
				m_tObjParam.bIsHit = false;

				Reset_OldAnimations();
			}
		}
	}
}

void CPlayer::Parameter_CheckActiveSkill()
{
	// 1번 스킬 가져옴
	for (_uint i = 0; i < 8; ++i)
	{
		Skill_ID eTmpSkillId = m_pUIManager->Get_Total_Inven()->Get_Registration_Skill(i);

		if (Skill_ID::SkillID_End == eTmpSkillId)
		{
			m_arrbActiveSkillOn[i] = false;
			ZeroMemory(&m_arrSkillInfo[i], sizeof(SKILL_INFO));
			continue;
		}

		_uint iSkillIdx =
			eTmpSkillId == Severing_Abyss ? 0 :
			eTmpSkillId == Phantom_Assault ? 1 :
			eTmpSkillId == Circulating_Pulse ? 2 :
			eTmpSkillId == Shadow_Assault ? 3 :
			eTmpSkillId == Legion_Punisher ? 4 :
			eTmpSkillId == Triple_Annihilator ? 5 :
			eTmpSkillId == Dragon_Lunge ? 6 :
			eTmpSkillId == Tormenting_Blast ? 7 :
			eTmpSkillId == Chariot_Rush ? 8 :
			eTmpSkillId == Fusillade_Rondo ? 9 : 999;

		if(999 == eTmpSkillId)
			continue;

		m_arrbActiveSkillOn[i] = true;

		m_arrSkillInfo[i].bOneHand = m_vecFullSkillInfo[iSkillIdx]->bOneHand;
		m_arrSkillInfo[i].dwSkillCost = m_vecFullSkillInfo[iSkillIdx]->dwSkillCost;
		m_arrSkillInfo[i].dwAnimationIdx = m_vecFullSkillInfo[iSkillIdx]->dwAnimationIdx;
	}
}

void CPlayer::Parameter_CheckActiveWeapon()
{
	
}

void CPlayer::Movement_Aiming(_float _fAngle, _float _fMovespeed)
{
	if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
	{
		_v3 tmpLook;

		if (m_bMove[MOVE_Front])
		{
			if (m_bMove[MOVE_Left])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Right])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = m_pTransform->Get_Axis(AXIS_Z);
			}
		}

		else if (m_bMove[MOVE_Back])
		{
			if (m_bMove[MOVE_Left])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Right])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = -m_pTransform->Get_Axis(AXIS_Z);
			}
		}

		else if (m_bMove[MOVE_Left])
		{
			if (m_bMove[MOVE_Front])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Back])
			{
				V3_NORMAL(&tmpLook, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = -m_pTransform->Get_Axis(AXIS_X);
			}
		}

		else if (m_bMove[MOVE_Right])
		{
			if (m_bMove[MOVE_Front])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
			}

			else if (m_bMove[MOVE_Back])
			{
				V3_NORMAL(&tmpLook, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
			}

			else
			{
				tmpLook = m_pTransform->Get_Axis(AXIS_X);
			}
		}

		D3DXVec3Normalize(&tmpLook, &tmpLook);

		m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(_fAngle));
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &tmpLook, _fMovespeed)));
	}
}

void CPlayer::Movement_NonAiming(_float _fRecover, _float _fAngle, _float _fRadian, _float _fMoveSpeed)
{
	if (m_bMove[MOVE_Front])
	{
		if (m_bMove[MOVE_Left] == true)
		{
			if (m_fAngle_Recover > -45.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= -45.f)
				{
					m_fAngle_Recover = -45.f;
				}
			}

			else if (m_fAngle_Recover < -45.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= -45.f)
				{
					m_fAngle_Recover = -45.f;
				}
			}
		}

		else if (m_bMove[MOVE_Right] == true)
		{
			if (m_fAngle_Recover < 45.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 45.f)
				{
					m_fAngle_Recover = 45.f;
				}
			}

			else if (m_fAngle_Recover > 45.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 45.f)
				{
					m_fAngle_Recover = 45.f;
				}
			}
		}

		else
		{
			if (m_fAngle_Recover > 180.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 360.f)
				{
					m_fAngle_Recover -= 360.f;
				}
			}

			else if (m_fAngle_Recover > 0.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 0.f)
				{
					m_fAngle_Recover = 0.f;
				}
			}

			else if (m_fAngle_Recover < 0.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 0.f)
				{
					m_fAngle_Recover = 0.f;
				}
			}
		}

		if(false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Left])
	{
		if (m_fAngle_Recover < -270.f)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -360.f)
			{
				m_fAngle_Recover += 360.f;
			}
		}

		else if (m_fAngle_Recover <= 0.f)
		{
			m_fAngle_Recover -= _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -90.f)
			{
				m_fAngle_Recover = -90.f;
			}
		}

		else if (m_fAngle_Recover > 90.f)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 270.f)
			{
				m_fAngle_Recover = 270.f;
			}
		}

		else if (m_fAngle_Recover > 0.f)
		{
			m_fAngle_Recover -= _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -90.f)
			{
				m_fAngle_Recover = -90.f;
			}
		}

		if (false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0.f, _fRadian, 0.f });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Right])
	{
		if (m_fAngle_Recover < -90.f)
		{
			m_fAngle_Recover -= _fRecover * DELTA_60;

			if (m_fAngle_Recover <= -270.f)
			{
				m_fAngle_Recover = 90.f;
			}
		}

		else if (m_fAngle_Recover < 0)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 180.f)
			{
				m_fAngle_Recover = 180.f;
			}
		}

		else if (m_fAngle_Recover > 270.f)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 360.f)
			{
				m_fAngle_Recover -= 360.f;
			}
		}

		else if (m_fAngle_Recover >= 0)
		{
			m_fAngle_Recover += _fRecover * DELTA_60;

			if (m_fAngle_Recover >= 90.f)
			{
				m_fAngle_Recover = 90.f;
			}
		}

		if (false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}

	else if (m_bMove[MOVE_Back])
	{
		if (m_bMove[MOVE_Left] == true)
		{
			if (m_fAngle_Recover < 225.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 225.f)
				{
					m_fAngle_Recover = 225.f;
				}
			}
		}

		else if (m_bMove[MOVE_Right] == true)
		{
			if (m_fAngle_Recover > 135.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 135.f)
				{
					m_fAngle_Recover = 135.f;
				}
			}
		}

		else
		{
			if (m_fAngle_Recover > 180.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= 180.f)
				{
					m_fAngle_Recover = 180.f;
				}
			}

			else if (m_fAngle_Recover < 0.f)
			{
				m_fAngle_Recover -= _fRecover * DELTA_60;

				if (m_fAngle_Recover <= -180.f)
				{
					m_fAngle_Recover = -180.f;
				}
			}

			else if (m_fAngle_Recover < 180.f)
			{
				m_fAngle_Recover += _fRecover * DELTA_60;

				if (m_fAngle_Recover >= 180.f)
				{
					m_fAngle_Recover = 180.f;
				}
			}
		}

		if (false == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle + m_fAngle_Recover);

		else if (true == m_bOnAttack)
			_fRadian = D3DXToRadian(_fAngle);

		m_pTransform->Set_Angle({ 0, _fRadian, 0 });

		_v3 tmpLook = m_pTransform->Get_Axis(AXIS_Z);
		D3DXVec3Normalize(&tmpLook, &tmpLook);

		// 네비 메쉬 추가되면 바꿔야함
		//m_pTransform->Add_Pos(fMoveSpeed);
		m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &tmpLook, _fMoveSpeed)));
	}
}

void CPlayer::Target_AimChasing()
{
	if (m_bHaveAimingTarget)
		return;

	if (false == g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE).empty())
	{
		_float fOldLength = 9999.f;
		CGameObject* pOldTarget = nullptr;

		_v3 pTargetTransPos = V3_NULL;

		for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE))
		{
			if (false == iter->Get_Enable())
				continue;

			if (true == iter->Get_Dead())
				continue;

			if (iter->Get_Target_Hp() <= 0.f)
				continue;

			pTargetTransPos = TARGET_TO_TRANS(iter)->Get_Pos();

			_float fLength = D3DXVec3Length(&(pTargetTransPos - m_pTransform->Get_Pos()));

			if (fLength > m_fAmingRange)
				continue;

			// 기존에 OldLength 보다 작을 경우
			if (fOldLength > fLength)
				fOldLength = fLength;

			else
				continue;

			pOldTarget = iter;
		}


		if (nullptr != pOldTarget)
		{
			m_bHaveAimingTarget = true;

			m_pTarget = pOldTarget;

			m_pCamManager->Set_AimingTarget(m_pTarget);
			m_pCamManager->Set_OnAimingTarget(true);

			m_pTransform->Set_Angle(AXIS_Y, m_pTransform->Chase_Target_Angle(&pTargetTransPos));

			m_bOnAiming = true;

			//==========================================================================
			Active_UI_LockOn();
			//==========================================================================

			return;
		}
	}

	if (false == g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE).empty())
	{
		_float fOldLength = 9999.f;
		CGameObject* pOldTarget = nullptr;

		_v3 pTargetTransPos = V3_NULL;

		for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE))
		{
			if (false == iter->Get_Enable())
				continue;

			if (true == iter->Get_Dead())
				continue;

			pTargetTransPos = TARGET_TO_TRANS(iter)->Get_Pos();

			_float fLength = D3DXVec3Length(&(pTargetTransPos - m_pTransform->Get_Pos()));

			if (fLength > m_fAmingRange)
				continue;
			
			// 기존에 OldLength 보다 작을 경우
			if (fOldLength > fLength)
				fOldLength = fLength;

			else
				continue;

			pOldTarget = iter;
		}


		if (nullptr != pOldTarget)
		{
			m_bHaveAimingTarget = true;

			m_pTarget = pOldTarget;

			m_pCamManager->Set_AimingTarget(m_pTarget);
			m_pCamManager->Set_OnAimingTarget(true);

			m_pTransform->Set_Angle(AXIS_Y, m_pTransform->Chase_Target_Angle(&pTargetTransPos));

			m_bOnAiming = true;

			//==========================================================================
			Active_UI_LockOn();
			//==========================================================================

			return;
		}
	}

	//==========================================================================
	Active_UI_LockOn(true);
	//==========================================================================

	m_bOnAiming = false;
	m_pCamManager->Set_OnAimingTarget(false);

	return;
}

void CPlayer::KeyInput()
{
	// 디버그 할때만 사용됩니다.
	//=====================================================================
	if (m_pCamManager->Get_CamView() == TOOL_VIEW)
		return;
	//=====================================================================

	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (m_eActState == ACT_Down)
		return;

	if (m_eActState == ACT_Disappear)
		return;

	if (m_eActState == ACT_Summon)
		return;

	if (m_eActState == ACT_Hit)
		return;

	if (m_eActState == ACT_Dead)
		return;

	//cout << "키를 눌러 주세요" << endl;

	KeyDown();
	KeyUp();
}

void CPlayer::KeyDown()
{
	if (m_bOnCustomMode)
	{
		_ulong dwIdx = m_pUIManager->Get_CustomCategory()->Get_ActiveSlotUI();

		if (m_dwOldx != dwIdx)
		{
			m_dwOldx = dwIdx;

			switch (dwIdx)
			{
			case TYPE_HAIR:
			{
				m_pCamManager->Set_CustomizeCamIdx(1);
				m_pCamManager->Set_MidDistance(1.3f);
				break;
			}

			case TYPE_FACE:
			{
				m_pCamManager->Set_CustomizeCamIdx(1);
				m_pCamManager->Set_MidDistance(1.f);
				break;
			}

			case TYPE_EYE:
			{
				m_pCamManager->Set_CustomizeCamIdx(1);
				m_pCamManager->Set_MidDistance(0.8f);
				break;
			}

			case TYPE_MASK:
			{
				m_pCamManager->Set_CustomizeCamIdx(2);
				m_pCamManager->Set_MidDistance(1.2f);
				break;
			}

			case TYPE_INNER:
			{
				m_pCamManager->Set_CustomizeCamIdx(3);
				m_pCamManager->Set_MidDistance(2.f);
				break;
			}
			}
		}

		if (g_pInput_Device->Key_Down(DIK_T))
		{
			m_pCamManager->Set_CustomizeCamIdx(0);
			m_pCamManager->Set_CustomizeCamMode(false);

			m_pUIManager->Get_CustomCategory()->Active_CustomUIs();
			m_bOnCustomMode = false;

			m_pCamManager->Set_MidDistance(g_OriginCamPos);

			m_pCamManager->Set_MouseCtrl(true);
			g_pInput_Device->Set_MouseLock(true);

			m_pRenderer->DOF_On(false);
		}

		return;
	}

	else
	{
		if (g_pInput_Device->Key_Down(DIK_T))
		{
			if (false == m_pUIManager->Get_CustomCategory()->Get_Active())
			{
				m_pCamManager->Set_CustomizeCamIdx(0);
				m_pCamManager->Set_CustomizeCamMode(true);

				m_pUIManager->Get_CustomCategory()->Active_CustomUIs();
				m_bOnCustomMode = true;

				m_pCamManager->Set_MouseCtrl(false);
				g_pInput_Device->Set_MouseLock(false);

				m_pRenderer->DOF_On(true);
			}
		}
	}

	if (m_bActiveUI)
	{
		Key_UI_n_Utiliy(m_bActiveUI);
	}

	else
	{
		if (false == m_bStopMovementKeyInput)
		{
			//cout << "오냐" << endl;

			// 이동관련
			Key_Movement_Down();

			// 공격관련
			Key_Attack();

			// 무기 변경
			Key_ChangeWeapon();

			// 스킬
			Key_Skill();

			// 흡혈
			Key_BloodSuck();

			// 상호작용
			Key_InterAct();

			// 유틸리티
			Key_Utility();
		}

		// 특수동작
		Key_Special();

		// UI , 유틸
		Key_UI_n_Utiliy(m_bActiveUI);
	}
}

void CPlayer::KeyUp()
{
	// 이동관련
	Key_Movement_Up();

	//
	if (false == m_bCharging)
		return;

	if (g_pInput_Device->Get_DIMouseState_Up(CInput_Device::DIM_RB))
	{
		m_bCharging = false;
		m_fChargeTimer_Cur = 0.f;
	}
}

void CPlayer::Key_Movement_Down()
{
	if (m_eActState == ACT_Disappear)
		return;

	if (m_eActState == ACT_Summon)
		return;

	if (m_eActState == ACT_Hit)
		return;

	if (m_eActState == ACT_Skill)
		return;

	if (m_eActState == ACT_BloodSuck_Count)
		return;

	if (m_eActState == ACT_BloodSuck_Combo)
		return;

	if (m_eActState == ACT_BloodSuck)
		return;

	if (g_pInput_Device->Key_Down(DIK_W))
	{
		m_bMove[MOVE_Front] = true;
		m_bOldMoveDierection[MOVE_Front] = true;
	}

	else if (g_pInput_Device->Key_Down(DIK_S))
	{
		m_bMove[MOVE_Back] = true;
		m_bOldMoveDierection[MOVE_Back] = true;
	}

	if (g_pInput_Device->Key_Down(DIK_A))
	{
		m_bMove[MOVE_Left] = true;
		m_bOldMoveDierection[MOVE_Left] = true;
	}

	else if (g_pInput_Device->Key_Down(DIK_D))
	{
		m_bMove[MOVE_Right] = true;
		m_bOldMoveDierection[MOVE_Right] = true;
	}

	if (g_pInput_Device->Key_Down(DIK_LSHIFT))
	{
		m_bSprint = true;
	}


	if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
	{
		if (true == m_bOnDodge)
			return;

		if (true == m_bOnAttack)
			return;

		// 두 상태가 아니라면, 일단 키가 눌리면 상태가 바뀜.
		if (false == m_bChangeWeapon && false == m_bOnBuff)
		{
			m_eActState = (m_bSprint ? ACT_Dash : ACT_Run);
		}

		else
		{
			if (m_tInfo.fMoveSpeed_Max * 0.25f > m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur += 0.1f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
			}

			else
			{
				m_tInfo.fMoveSpeed_Cur = m_tInfo.fMoveSpeed_Max * 0.25f;
			}

			return;
		}

		if (false == m_bOnAiming)
		{
			_float fMaxSpeedMultiply = m_eActState == ACT_Run ? 1.f : 1.2f;

			if (m_tInfo.fMoveSpeed_Max * fMaxSpeedMultiply > m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur += 1.f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
			}

			if (m_tInfo.fMoveSpeed_Max * fMaxSpeedMultiply < m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur = m_tInfo.fMoveSpeed_Max * fMaxSpeedMultiply;
			}
		}

		else if (true == m_bOnAiming)
		{
			_float tmpSpeedValue = 0.8f;

			if (m_bMove[MOVE_Back])
				tmpSpeedValue = 0.6f;

			if (m_tInfo.fMoveSpeed_Max * tmpSpeedValue > m_tInfo.fMoveSpeed_Cur)
			{
				m_tInfo.fMoveSpeed_Cur += 0.1f + m_tInfo.fMoveAccel_Cur * m_tInfo.fMoveAccel_Cur * DELTA_60;
				m_tInfo.fMoveAccel_Cur += DELTA_60;
			}

			else
			{
				m_tInfo.fMoveSpeed_Cur = m_tInfo.fMoveSpeed_Max * tmpSpeedValue;
			}
		}
	}

	else
	{
		if (m_eActState == ACT_Skill)
			return;

		if (m_eActState == ACT_Dead)
			return;

		if (m_eActState == ACT_Hit)
			return;

		if (m_eActState == ACT_Dodge)
			return;

		if (m_eActState == ACT_WeakAtk)
			return;

		if (m_eActState == ACT_HeavyAtk)
			return;

		if (m_eActState == ACT_MoveDelay)
			return;

		if (m_eActState == ACT_WeaponChange)
			return;

		if (m_eActState == ACT_Buff)
			return;

		if (m_eActState == ACT_BloodSuck)
			return;

		if (m_eActState == ACT_BloodSuck_Count)
			return;

		if (m_eActState == ACT_PickUp)
			return;

		if (m_eActState == ACT_Idle)
			return;

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.001f))
		{
			if (m_eActState != ACT_Idle)
				Reset_BattleState();

			m_eActState = m_tInfo.fMoveSpeed_Cur > 0.f ? ACT_MoveDelay : ACT_Idle;

			m_bOnMoveDelay = false;
			m_bOnDodge = false;
		}
	}


}

void CPlayer::Key_Movement_Up()
{
	if (g_pInput_Device->Key_Up(DIK_W))
	{
		m_bMove[MOVE_Front] = false;

		if (false == m_bOnAiming)
			m_bOldMoveDierection[MOVE_Front] = false;
	}

	else if (g_pInput_Device->Key_Up(DIK_S))
	{
		m_bMove[MOVE_Back] = false;

		if(false == m_bOnAiming)
			m_bOldMoveDierection[MOVE_Back] = false;
	}

	if (g_pInput_Device->Key_Up(DIK_A))
	{
		m_bMove[MOVE_Left] = false;

		if (false == m_bOnAiming)
			m_bOldMoveDierection[MOVE_Left] = false;
	}

	else if (g_pInput_Device->Key_Up(DIK_D))
	{
		m_bMove[MOVE_Right] = false;

		if (false == m_bOnAiming)
			m_bOldMoveDierection[MOVE_Right] = false;
	}

	if (g_pInput_Device->Key_Up(DIK_LSHIFT))
	{
		m_bSprint = false;
	}
}

void CPlayer::Key_ChangeWeapon()
{
	if (g_pInput_Device->Key_Down(DIK_UPARROW))
	{
		Change_Weapon();
	}
}

void CPlayer::Key_Special()
{
	if (g_pInput_Device->Key_Down(DIK_SPACE))
	{
		if (m_bCanDodge &&
			m_fStaminaCost_Dodge <= m_tObjParam.fStamina_Cur)
		{
			m_bOnDodge = false;

			m_tObjParam.fStamina_Cur -= m_fStaminaCost_Dodge;

			Reset_OldAnimations();

			m_eActState = ACT_Dodge;

			Reset_BattleState();
		}
	}

	if (g_pInput_Device->Key_Down(DIK_Q))
	{
		m_bOnAiming = (m_bOnAiming == false ? true : false);

		if (false == m_bOnAiming)
		{
			m_pCamManager->Set_LockAngleX(D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)));
		}
	}
}

void CPlayer::Key_Attack()
{
	if (m_eActState == ACT_BloodSuck)
		return;

	if (m_eActState == ACT_BloodSuck_Combo)
		return;

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_bReadyChargeSuck == true)
		{
			if (m_bOnAttack)
			{
				m_eActState = ACT_BloodSuck_Combo;
			}

			else if (false == m_bOnAttack)
			{
				m_eActState = ACT_BloodSuck;
			}

			return;
		}

		if (false == m_bOnCombo)
		{
			if (m_bCanAttack == false)
			{
				if (m_pDynamicMesh->Is_Finish_Animation_Lower(m_fAtkEndTime))
				{
					m_bCanAttack = (m_sWeakAtkCnt < m_sWeakAtkCnt_Max ? true : false);
					m_bOnAttack = (m_sWeakAtkCnt < m_sWeakAtkCnt_Max ? false : true);
				}
			}

			if (m_bCanAttack == true &&
				m_fStaminaCost_WeakAttack <= m_tObjParam.fStamina_Cur)
			{
				LOOP(16)
					m_bEventTrigger[i] = false;

				if (m_tInfo.fMoveSpeed_Cur >= m_tInfo.fMoveSpeed_Max)
				{
					m_bCanSprintAtk = true;
				}

				else
					m_bCanSprintAtk = false;

				m_tInfo.fMoveSpeed_Cur = 0.f;
				m_tInfo.fMoveAccel_Cur = 0.f;

				m_tObjParam.fStamina_Cur -= m_fStaminaCost_WeakAttack;

				if (m_bSprint)
				{
					m_eActState = ACT_WeakAtk;
					m_bCanAttack = false;

					m_fAtkEndTime = 0.75f;
				}

				else if (m_sWeakAtkCnt < m_sWeakAtkCnt_Max)
				{
					m_eActState = ACT_WeakAtk;
					m_bCanAttack = false;

					m_sWeakAtkCnt += 1;
					switch (m_eMainWpnState)
					{
					case WEAPON_None:
					{
						m_fAtkEndTime = (m_sWeakAtkCnt == 3 ? 0.18f : 0.2f);
						break;
					}

					case WEAPON_SSword:
					{
						m_fAtkEndTime = (m_sWeakAtkCnt == 3 ? 0.18f : 0.2f);
						break;
					}

					case WEAPON_LSword:
					{
						m_fAtkEndTime = 0.3f;
						break;
					}

					case WEAPON_Halberd:
					{
						m_fAtkEndTime = 0.3f;
						break;
					}

					case WEAPON_Hammer:
					{
						m_fAtkEndTime = 0.35f;
						break;
					}

					case WEAPON_Gun:
					{
						m_fAtkEndTime = 0.2f;
						break;
					}
					}
				}
			}
		}
	}

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
	{
		if (false == m_bOnCombo)
		{
			if (m_bCanAttack == false)
			{
				if (m_pDynamicMesh->Is_Finish_Animation_Lower(m_fAtkEndTime))
				{
					m_bCanAttack = (m_sHeavyAtkCnt < m_sHeavyAtkCnt_Max ? true : false);
					m_bOnAttack = (m_sHeavyAtkCnt < m_sHeavyAtkCnt_Max ? false : true);
				}
			}

			if (m_bCanAttack == true &&
				m_fStaminaCost_HeavyAttack <= m_tObjParam.fStamina_Cur)
			{
				LOOP(16)
					m_bEventTrigger[i] = false;

				m_tObjParam.fStamina_Cur -= m_fStaminaCost_HeavyAttack;

				if (m_bSprint)
				{
					m_eActState = ACT_HeavyAtk;
					m_bCanAttack = false;

					m_fAtkEndTime = 0.75f;
				}

				else if (m_sHeavyAtkCnt < m_sHeavyAtkCnt_Max)
				{
					m_eActState = ACT_HeavyAtk;
					m_bCanAttack = false;

					m_sHeavyAtkCnt += 1;
					m_fAtkEndTime = 0.75f;

					m_bCharging = true;

					LPCSTR tmpChar = "RightHandAttach";
					_mat   matAttach;
					D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);
					matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
					_v3 vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43);

					g_pManagement->Create_Effect_Delay(L"Player_ChargeSpark_Particle", 0.f	, vEffPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_ChargeSpark_Small"	 , 0.f	, vEffPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_ChargeSpark_Big"	 , 0.f	, vEffPos, nullptr);
				}
			}
		}
	}
}

void CPlayer::Key_Skill()
{
	// 1번 스킬
	if (g_pInput_Device->Key_Down(DIK_1))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[0])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[0].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[0].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[0].dwAnimationIdx;
		m_eActState = ACT_Skill;

		//===================================================================================================================
		//m_eActState = ACT_Skill0
		//
		//if (true == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[0]->dwAnimationIdx);
		//
		//else if (false == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[4]->dwAnimationIdx);
		//===================================================================================================================
	}

	// 2번 스킬
	else if (g_pInput_Device->Key_Down(DIK_2))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[1])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[1].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[1].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[1].dwAnimationIdx;
		m_eActState = ACT_Skill;

		//m_eActState = ACT_Skill;
		//
		//if (true == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[1]->dwAnimationIdx);
		//
		//else if (false == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[5]->dwAnimationIdx);
		//
		//_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 1.5f;
	}

	// 3번 스킬
	else if (g_pInput_Device->Key_Down(DIK_3))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[2])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[3].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[2].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[2].dwAnimationIdx;
		m_eActState = ACT_Skill;

		//m_eActState = ACT_Skill;
		//
		//if(true == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[2]->dwAnimationIdx);
		//
		//else if (false == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[6]->dwAnimationIdx);

		Reset_BattleState();
	}

	// 4번 스킬
	else if (g_pInput_Device->Key_Down(DIK_4))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[3])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[3].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[3].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[3].dwAnimationIdx;
		m_eActState = ACT_Skill;

		//m_eActState = ACT_Skill;
		//
		//if (true == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[3]->dwAnimationIdx);
		//
		//else if (false == m_bOneHand)
		//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[7]->dwAnimationIdx);
	}

	// 5번 스킬
	else if (g_pInput_Device->Key_Down(DIK_5))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[4])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[4].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[4].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[4].dwAnimationIdx;
		m_eActState = ACT_Skill;
	}


	// 6번 스킬
	else if (g_pInput_Device->Key_Down(DIK_6))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[5])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[5].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[5].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[5].dwAnimationIdx;
		m_eActState = ACT_Skill;
	}


	// 7번 스킬
	else if (g_pInput_Device->Key_Down(DIK_7))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[6])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[6].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[6].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[6].dwAnimationIdx;
		m_eActState = ACT_Skill;
	}

	// 7번 스킬
	else if (g_pInput_Device->Key_Down(DIK_8))
	{
		if (true == m_bOnSkill)
			return;

		if (false == m_arrbActiveSkillOn[7])
			return;

		if (m_tObjParam.sMana_Cur < (_short)m_arrSkillInfo[7].dwSkillCost)
			return;

		Reset_BattleState();

		m_tObjParam.sMana_Cur -= (_short)m_arrSkillInfo[7].dwSkillCost;
		m_eAnim_Lower = (P_NEWANI)m_arrSkillInfo[7].dwAnimationIdx;
		m_eActState = ACT_Skill;
	}

	// 1번 버프
	//else if (g_pInput_Device->Key_Down(DIK_5))
	//{
	//	if (true == m_bOnSkill)
	//		return;
	//
	//	m_eActState = ACT_Buff;
	//
	//	m_eAnim_Lower = P_NEWANI(m_vecFullSkillInfo[8]->dwAnimationIdx);
	//
	//	LPCSTR tmpChar = "Head"; // 왼손 뼈로 바꿔야함
	//	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 1);
	//	_mat* pLeftHand= &pFamre->CombinedTransformationMatrix;
	//
	//	g_pManagement->Create_Effect_Delay(L"Player_Buff_Particle"							, 0.4f, _v3(0.f, 1.25f, 0.f), m_pTransform);
	//	g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Blaster"				, 0.6f, m_pTransform, pLeftHand);
	//	g_pManagement->Create_Effect_Delay(L"Player_Buff_HandLight"							, 0.4f, m_pTransform, pLeftHand);
	//	g_pManagement->Create_ParticleEffect_Delay(L"Player_Buff_Flash"				, 0.1f	, 0.6f, m_pTransform, pLeftHand);
	//	g_pManagement->Create_ParticleEffect_Delay(L"Player_Buff_HandSmoke"		, 0.65f	, 0.6f, m_pTransform, pLeftHand);
	//}
}

void CPlayer::Key_Utility()
{
	if (g_pInput_Device->Key_Down(DIK_DOWNARROW))
	{
		//m_eActState = ACT_PickUp;

		//===========================================
		// 아이템 사용
		//===========================================

		LPCSTR tmpChar = "RightHandAttach"; //왼손 뼈 몰라서 임시.
		_mat   matAttach;
		D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);
		matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
		_v3 vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43);

		g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle"			, 0.2f, vEffPos, nullptr);
		g_pManagement->Create_Effect_Delay(L"Player_Heal_RedLight"						, 0.2f, vEffPos, nullptr);
		g_pManagement->Create_Effect_Delay(L"Player_Heal_Particle"						, 0.2f, vEffPos, nullptr);
		g_pManagement->Create_ParticleEffect_Delay(L"Player_Buff_HandSmoke"		, 0.2f	, 0.2f, vEffPos, nullptr);
	}
}

void CPlayer::Key_InterAct()
{
	//if (g_pInput_Device->Key_Down(DIK_E))
	//{
	//	m_eActState = ACT_PickUp;
	//
	//	//===========================================
	//	// 아이템 줍기
	//	//===========================================
	//}
}

void CPlayer::Key_BloodSuck()
{
	if (g_pInput_Device->Key_Down(DIK_LCONTROL))
	{
		if (m_bReadyChargeSuck == false)
			m_bReadyChargeSuck = true;
	}

	else if (g_pInput_Device->Key_Up(DIK_LCONTROL))
	{
		m_bReadyChargeSuck = false;
	}

	if (g_pInput_Device->Key_Down(DIK_C))
	{
		m_eActState = ACT_BloodSuck_Count;
	}

	// 임시 처형 , 원래는 공격을 카운터 치거나 뒤잡기 해야됨.
	if (g_pInput_Device->Key_Down(DIK_V))
	{
		if (m_bCanExecution)
		{
			if (Check_CunterTarget())
			{
				m_eActState = ACT_BloodSuck_Execution;
			}

			//else
			//	cout << "처형할 대상이 없습니다." << endl;
		}
	}
}

void CPlayer::Key_UI_n_Utiliy(_bool _bActiveUI)
{
	if (true == _bActiveUI)
	{
		if (g_pInput_Device->Key_Down(DIK_ESCAPE))
		{
			if (true == m_pUIManager->Get_Skill_Inven()->Get_Active())
			{
				m_pUIManager->Get_Skill_Inven()->Set_Active(false);
			}

			else if (m_bOnUI_Inventory)
			{
				m_pCamManager->Set_MouseCtrl(true);
				g_pInput_Device->Set_MouseLock(true);
				m_pRenderer->DOF_On(false);

				m_bActiveUI = false;
				m_pUIManager->Get_Total_Inven()->Set_Active(false);
				m_pUIManager->Get_StatusUI()->Set_Active(false);

				Parameter_CheckActiveSkill();
				Parameter_CheckActiveWeapon();
			}

			else
			{
				if (m_bOnUI_StageSelect)
				{
					// 예는 하위라 안꺼도됨
					Active_UI_StageSelect(true);
				}

				else if (m_bOnUI_BloodCode)
				{
					// 예도 하위라 안꺼도됨
					Active_UI_BloodCode(true);
				}

				else if (m_bOnUI_Mistletoe)
				{
					// 예는 상위라 꺼야됨
					m_bActiveUI = false;
					Active_UI_Mistletoe(true);
				}

				else if (m_bOnUI_Inventory)
				{
					// 상위
					m_bActiveUI = false;
					Active_UI_Inventory(true);
				}

				else if (m_bOnUI_NPCTalk)
				{
					// 상위
					m_bActiveUI = false;
					Active_UI_NPC(true);
				}
			}
		}

		// 임시로 넣어둔거라, 이거 빼야함
		else if (g_pInput_Device->Key_Down(DIK_E))
		{
			if(m_bOnUI_NPCTalk)
			{
				if (m_bOnYakumo_UI)
				{
					Active_UI_WeaponShop_Yakumo(true);
				}

				else if (m_bOnYokumo_UI)
				{
					Active_UI_MaterialShop_Yokumo(true);
				}
			}

			if (m_bOnUI_StageSelect)
			{
				Active_UI_StageSelect(true);
			}

			else if (m_bOnUI_BloodCode)
			{
				Active_UI_BloodCode(true);
			}

			else if (m_bOnUI_Mistletoe)
			{
				m_bActiveUI = false;
				Active_UI_Mistletoe(true);
			}

			else if (m_bOnUI_Inventory)
				Active_UI_Inventory(true);

			else if (m_bOnUI_NPCTalk)
				Active_UI_NPC(true);
		}

		if (m_bOnUI_StageSelect)
		{
			if (g_pInput_Device->Key_Down(DIK_RIGHT))
			{
				m_pUIManager->Get_StageSelectUI()->Move_Right();
			}

			else if (g_pInput_Device->Key_Down(DIK_LEFT))
			{
				m_pUIManager->Get_StageSelectUI()->Move_Left();
			}

			if (g_pInput_Device->Key_Down(DIK_UP))
			{
				m_pUIManager->Get_StageSelectUI()->MoveUp_SubStage();
			}

			else if (g_pInput_Device->Key_Down(DIK_DOWN))
			{
				m_pUIManager->Get_StageSelectUI()->MoveDown_SubStage();
			}

			if (g_pInput_Device->Key_Down(DIK_SPACE))
			{
				m_pUIManager->Get_StageSelectUI()->Teleport_Stage();

				m_pStageAgent->Reserve_ChangeStage();

				m_bActiveUI = false;
				Active_UI_StageSelect(true);
				Active_UI_Mistletoe(true);
			}
		}
	}

	else if (false == _bActiveUI)
	{
		if (g_pInput_Device->Key_Down(DIK_ESCAPE))
		{
			g_pInput_Device->Set_MouseLock(false);
			m_pCamManager->Set_MouseCtrl(false);
			m_pRenderer->DOF_On(true);

			m_bActiveUI = true;
			m_bOnUI_Inventory = true;
			m_pUIManager->Get_Total_Inven()->Set_Active(true);
			m_pUIManager->Get_StatusUI()->Set_Active(true);
		}

		else if (g_pInput_Device->Key_Down(DIK_E))
		{
			if(m_bOnUI_Mistletoe)
				Active_UI_Mistletoe();

			else if (m_bOnUI_NPCTalk)
			{
				m_pUIManager->Get_NPC_InteractionUI()->Set_Interaction(true);
				m_pUIManager->Get_NPC_InteractionUI()->Set_ReactConverSation(true);

				if (m_bOnYakumo_UI)
				{
					Active_UI_WeaponShop_Yakumo();
				}

				else if (m_bOnYokumo_UI)
				{
					Active_UI_MaterialShop_Yokumo();
				}

				else if (m_bOnJack_UI)
				{

				}
			}
		}
	}

	if (g_pInput_Device->Key_Up(DIK_E))
	{
		m_pUIManager->Get_NPC_InteractionUI()->Set_Interaction(false);
	}


	return;

	if (!_bActiveUI)
	{
		// 상호작용도 하고, 아이템도 줍고, 겨우살이도 활성화 시키고
		if (g_pInput_Device->Key_Down(DIK_E))
		{
			// 겨우살이가 활성화 가능하면
			if (m_bOnUI_Mistletoe)
			{
				Active_UI_Mistletoe();
			}

			// 상호작용 가능
			else if (m_bCanInterAct)
			{
				// 아직 추가안함.
			}

			// 대화 가능
			else if (m_bCanDialouge)
			{
				// 아직 추가안함.
			}

			// 아이템 획득 가능
			else if (m_bCanPickUp)
			{
				// 아직 추가안함.
			}
		}

		// 유아이가 비활성화 되어 있을 때는 , 메인 UI 를 킨다.
		else if (g_pInput_Device->Key_Down(DIK_ESCAPE))
		{
			m_pUIManager->Get_Total_Inven()->Set_Active(true);
		}
	}

	// UI 가 활성화 되어 있을 경우

	else if (_bActiveUI)
	{
		// 유아이가 활성화 되어 있을 때는 , 닫아주는 기능을 한다.
		if (g_pInput_Device->Key_Down(DIK_ESCAPE))
		{
			if(m_bOnUI_StageSelect)
				Active_UI_StageSelect(true);

			else if (m_bOnUI_Mistletoe)
				Active_UI_Mistletoe(true);

			else if (m_bOnUI_BloodCode)
				Active_UI_BloodCode(true);

			else if (m_bOnUI_Inventory)
				Active_UI_Inventory(true);

			else if (m_bOnUI_NPCTalk)
				Active_UI_NPC(true);
		}
	}
}

void CPlayer::Play_Idle()
{
	switch (m_eMainWpnState)
	{
	case WEAPON_None:
	{
		m_eAnim_Lower = Cmn_Idle;
		break;
	}

	case WEAPON_SSword:
	{
		m_eAnim_Lower = Cmn_Idle;
		break;
	}

	case WEAPON_LSword:
	{
		m_eAnim_Lower = LSword_BlendIdle;
		break;
	}

	case WEAPON_Hammer:
	{
		m_eAnim_Lower = Hammer_BlendIdle;
		break;
	}

	case WEAPON_Halberd:
	{
		m_eAnim_Lower = Halberd_BlendIdle;
		break;
	}
	case WEAPON_Gun:
	{
		m_eAnim_Lower = Gun_BlendIdle;
		break;
	}
	}

	m_eAnim_Upper = m_eAnim_Lower;
	m_eAnim_RightArm = m_eAnim_Lower;
	m_eAnim_LeftArm = m_eAnim_Lower;
}

void CPlayer::Play_Run()
{
	// 에임 중이 아닐 경우 : 전방으로 달림
	if (false == m_bOnAiming)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_SSword:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_LSword:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Hammer:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}

		case WEAPON_Halberd:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}
		case WEAPON_Gun:
		{
			m_eAnim_Lower = Cmn_Run_F;
			break;
		}
		}
	}

	// 에임 중일 경우 : 키 입력에 따른 방향
	else if (true == m_bOnAiming)
	{
		if (m_bMove[MOVE_Front])
		{
			if (m_bMove[MOVE_Left])
			{
				m_eAnim_Lower = Cmn_Run_FL;
			}

			else if (m_bMove[MOVE_Right])
			{
				m_eAnim_Lower = Cmn_Run_FR;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_F;
			}
		}

		else if (m_bMove[MOVE_Back])
		{
			if (m_bMove[MOVE_Left])
			{
				m_eAnim_Lower = Cmn_Run_FL;
			}

			else if (m_bMove[MOVE_Right])
			{
				m_eAnim_Lower = Cmn_Run_FR;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_B;
			}
		}

		else if (m_bMove[MOVE_Left])
		{
			if (m_bMove[MOVE_Front])
			{
				m_eAnim_Lower = Cmn_Run_FL;
			}

			else if (m_bMove[MOVE_Back])
			{
				m_eAnim_Lower = Cmn_Run_BL;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_L;
			}
		}

		else if (m_bMove[MOVE_Right])
		{
			if (m_bMove[MOVE_Front])
			{
				m_eAnim_Lower = Cmn_Run_FR;
			}

			else if (m_bMove[MOVE_Back])
			{
				m_eAnim_Lower = Cmn_Run_BR;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_R;
			}
		}
	}

	if (false == m_bChangeWeapon)
	{
		m_eAnim_Upper = m_eAnim_Lower;

		m_eAnim_RightArm =
			(m_eMainWpnState == WEAPON_Hammer ? Hammer_BlendRun :
				m_eMainWpnState == WEAPON_Gun ? Gun_BlendRun :
				m_eMainWpnState == WEAPON_Halberd ? Halberd_BlendRun :
				m_eMainWpnState == WEAPON_LSword ? Lsword_BlendRun : m_eAnim_Lower);

		m_eAnim_LeftArm = 
			(m_eMainWpnState == WEAPON_Gun ? Gun_BlendRun :
			m_eMainWpnState == WEAPON_Halberd ? Halberd_BlendRun : m_eAnim_Lower);
	}

	if (false == m_bOnAiming)
	{
		m_fFootWalkTimer_Max = 0.4f;

		if (m_bCanPlayWalkSound)
		{
			m_fFootWalkTimer_Cur = 0.f;
			m_bCanPlayWalkSound = false;

			_int iSwitchNum = CALC::Random_Num(0, 2);

			switch (iSwitchNum)
			{
			case 0:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_01.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);

				break;
			}
			case 1:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_02.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			case 2:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_03.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			}
		}

		g_pManagement->Create_Effect_Offset(L"Player_FootSmoke", 0.5f, m_pTransform->Get_Pos());

		if (SCENE_STAGE_02 == g_eSceneID_Cur)
		{
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_Snow", 0.5f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowChunk", 0.5f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowUp", 0.5f, m_pTransform->Get_Pos());
		}
	}

	else
	{
		m_fFootWalkTimer_Max = 0.4f;

		if (m_bCanPlayWalkSound)
		{
			m_fFootWalkTimer_Cur = 0.f;
			m_bCanPlayWalkSound = false;

			_int iSwitchNum = CALC::Random_Num(0, 2);

			switch (iSwitchNum)
			{
			case 0:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_01.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);

				break;
			}
			case 1:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_02.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			case 2:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_03.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			}
		}

		g_pManagement->Create_Effect_Offset(L"Player_FootSmoke", 0.5f, m_pTransform->Get_Pos());

		if (SCENE_STAGE_02 == g_eSceneID_Cur)
		{
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_Snow", 0.5f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowChunk", 0.5f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowUp", 0.5f, m_pTransform->Get_Pos());
		}
	}
}

void CPlayer::Play_Dash()
{
	// 에임 중이 아닐 경우 : 전방으로 달림
	if(false == m_bOnAiming)
		m_eAnim_Lower = Cmn_Dash;

	// 에임 중일 경우 : 키 입력에 따른 방향
	else if (true == m_bOnAiming)
	{
		if (m_bMove[MOVE_Front])
		{
			if (m_bMove[MOVE_Left])
			{
				m_eAnim_Lower = Cmn_Run_FL;
			}

			else if (m_bMove[MOVE_Right])
			{
				m_eAnim_Lower = Cmn_Run_FR;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_F;
			}
		}

		else if (m_bMove[MOVE_Back])
		{
			if (m_bMove[MOVE_Left])
			{
				m_eAnim_Lower = Cmn_Run_FL;
			}

			else if (m_bMove[MOVE_Right])
			{
				m_eAnim_Lower = Cmn_Run_FR;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_B;
			}
		}

		else if (m_bMove[MOVE_Left])
		{
			if (m_bMove[MOVE_Front])
			{
				m_eAnim_Lower = Cmn_Run_FL;
			}

			else if (m_bMove[MOVE_Back])
			{
				m_eAnim_Lower = Cmn_Run_BL;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_L;
			}
		}

		else if (m_bMove[MOVE_Right])
		{
			if (m_bMove[MOVE_Front])
			{
				m_eAnim_Lower = Cmn_Run_FR;
			}

			else if (m_bMove[MOVE_Back])
			{
				m_eAnim_Lower = Cmn_Run_BR;
			}

			else
			{
				m_eAnim_Lower = Cmn_Run_R;
			}
		}
	}


	if (false == m_bChangeWeapon)
	{
		m_eAnim_Upper = m_eAnim_Lower;

		m_eAnim_RightArm =
			(m_eMainWpnState == WEAPON_Hammer ? Hammer_BlendDash :
				m_eMainWpnState == WEAPON_Gun ? Gun_BlendDash :
				m_eMainWpnState == WEAPON_Halberd ? Halberd_BlendDash :
				m_eMainWpnState == WEAPON_LSword ? Lsword_BlendDash : m_eAnim_Lower);

		m_eAnim_LeftArm = (
			m_eMainWpnState == WEAPON_Gun ? Gun_BlendDash :
			m_eMainWpnState == WEAPON_Halberd ? Halberd_BlendDash : m_eAnim_Lower);
	}

	if (false == m_bOnAiming)
	{
		m_fFootWalkTimer_Max = 0.35f;

		if (m_bCanPlayWalkSound)
		{
			m_fFootWalkTimer_Cur = 0.f;
			m_bCanPlayWalkSound = false;

			_int iSwitchNum = CALC::Random_Num(0, 2);

			switch (iSwitchNum)
			{
			case 0:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_01.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);

				break;
			}
			case 1:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_02.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			case 2:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_03.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			}
		}

		g_pManagement->Create_Effect_Offset(L"Player_FootSmoke", 0.35f, m_pTransform->Get_Pos());

		if (SCENE_STAGE_02 == g_eSceneID_Cur)
		{
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_Snow", 0.35f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowChunk", 0.35f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowUp", 0.35f, m_pTransform->Get_Pos());
		}
	}

	else
	{
		m_fFootWalkTimer_Max = 0.4f;

		if (m_bCanPlayWalkSound)
		{
			m_fFootWalkTimer_Cur = 0.f;
			m_bCanPlayWalkSound = false;

			_int iSwitchNum = CALC::Random_Num(0, 2);

			switch (iSwitchNum)
			{
			case 0:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_01.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);

				break;
			}
			case 1:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_02.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			case 2:
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_FootStep);
				g_pSoundManager->Play_Sound(L"FootHeel_03.wav", CSoundManager::Player_FootStep, CSoundManager::Effect_Sound);


				break;
			}
			}
		}

		g_pManagement->Create_Effect_Offset(L"Player_FootSmoke", 0.5f, m_pTransform->Get_Pos());

		if (SCENE_STAGE_02 == g_eSceneID_Cur)
		{
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_Snow", 0.5f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowChunk", 0.5f, m_pTransform->Get_Pos());
			g_pManagement->Create_Effect_Offset(L"Player_FootSmoke_SnowUp", 0.5f, m_pTransform->Get_Pos());
		}
	}
}

void CPlayer::Play_MoveDelay()
{
	if (false == m_bOnMoveDelay)
	{
		m_bOnMoveDelay = true;

		if (m_bOnAiming)
		{
			if (m_bOldMoveDierection[MOVE_Front])
			{
				if (m_bOldMoveDierection[MOVE_Left])
				{
					m_eAnim_Lower = Cmn_Run_FL_End;
				}

				else if (m_bOldMoveDierection[MOVE_Right])
				{
					m_eAnim_Lower = Cmn_Run_FR_End;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_F_End_L;
				}
			}

			else if (m_bOldMoveDierection[MOVE_Back])
			{
				if (m_bOldMoveDierection[MOVE_Left])
				{
					m_eAnim_Lower = Cmn_Run_FL_End;
				}

				else if (m_bOldMoveDierection[MOVE_Right])
				{
					m_eAnim_Lower = Cmn_Run_FR_End;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_B_End;
				}
			}

			else if (m_bOldMoveDierection[MOVE_Left])
			{
				if (m_bOldMoveDierection[MOVE_Front])
				{
					m_eAnim_Lower = Cmn_Run_FL_End;
				}

				else if (m_bOldMoveDierection[MOVE_Back])
				{
					m_eAnim_Lower = Cmn_Run_BL_End;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_L_LEnd;
				}
			}

			else if (m_bOldMoveDierection[MOVE_Right])
			{
				if (m_bOldMoveDierection[MOVE_Front])
				{
					m_eAnim_Lower = Cmn_Run_FR_End;
				}

				else if (m_bOldMoveDierection[MOVE_Back])
				{
					m_eAnim_Lower = Cmn_Run_BR_End;
				}

				else
				{
					m_eAnim_Lower = Cmn_Run_R_End;
				}
			}
		}

		else
		{
			m_eAnim_Lower = Cmn_Run_F_End_L;
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		// 한손 일때
		if (m_bOneHand)
		{
			m_eAnim_LeftArm =
				m_eAnim_RightArm =
				(m_eMainWpnState == WEAPON_Gun ? Gun_BlendWalk :
					m_eMainWpnState == WEAPON_Halberd ? Halberd_BlendWalk: m_eAnim_Lower);
		}
		// 양손 일때
		else
		{
			m_eAnim_RightArm = 
				(m_eMainWpnState == WEAPON_Hammer ? Hammer_BlendWalk :
					m_eMainWpnState == WEAPON_LSword ? Lsword_BlendWalk : Lsword_BlendWalk);
		}

		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveSpeed_Cur = m_tInfo.fMoveSpeed_Cur;
		m_fSkillMoveMultiply = 0.75f;

		m_bOldMoveDierection[MOVE_Front] = false;
		m_bOldMoveDierection[MOVE_Back] = false;
		m_bOldMoveDierection[MOVE_Left] = false;
		m_bOldMoveDierection[MOVE_Right] = false;

	}

	else if (true == m_bOnMoveDelay)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.8f))
		{
			m_eActState = ACT_Idle;

			m_bOnMoveDelay = false;
		}

		_v3 vDelayDir = WORLD_LOOK; 
		
		vDelayDir =
			m_eAnim_Lower == Cmn_Run_F_End_L ? m_pTransform->Get_Axis(AXIS_Z) :
			m_eAnim_Lower == Cmn_Run_FL_End ? m_pTransform->Get_Axis(AXIS_Z) - m_pTransform->Get_Axis(AXIS_X):
			m_eAnim_Lower == Cmn_Run_FR_End ? m_pTransform->Get_Axis(AXIS_Z) + m_pTransform->Get_Axis(AXIS_X) :
			m_eAnim_Lower == Cmn_Run_L_LEnd ? -m_pTransform->Get_Axis(AXIS_X) :
			m_eAnim_Lower == Cmn_Run_R_End ? m_pTransform->Get_Axis(AXIS_X) :
			m_eAnim_Lower == Cmn_Run_B_End ? -m_pTransform->Get_Axis(AXIS_Z) :
			m_eAnim_Lower == Cmn_Run_BL_End ? -m_pTransform->Get_Axis(AXIS_Z) - m_pTransform->Get_Axis(AXIS_X) :
			m_eAnim_Lower == Cmn_Run_BR_End ? -m_pTransform->Get_Axis(AXIS_Z) + m_pTransform->Get_Axis(AXIS_X) : 
			m_pTransform->Get_Axis(AXIS_Z);

		V3_NORMAL_SELF(&vDelayDir);

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, vDelayDir);
	}
}

void CPlayer::Play_Dodge()
{
	if (false == m_bOnDodge)
	{
		g_pManagement->Create_AutoFindEffect(L"Player_SpaceBar_StepParticle", 1.f, m_pTransform, _v3(0.f, 1.f, 0.f));
		g_pManagement->Create_Effect(L"Player_FootSmoke_Jump", m_pTransform->Get_Pos());
		g_pManagement->Create_Effect(L"Player_FootSmoke_DodgeBack", V3_NULL, m_pTransform);

		m_bDodgeBack = false;
		
		if (true == m_bOnAiming)
		{
			switch (m_eMainWpnState)
			{
			case WEAPON_None:
			case WEAPON_SSword:
			{
				if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Ssword_Dodge_FL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Ssword_Dodge_FR;
						}

						else
						{
							m_eAnim_Lower = Ssword_Dodge_F;
						}
					}

					else if (m_bMove[MOVE_Back])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Ssword_Dodge_BL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Ssword_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Ssword_Dodge_B;
						}
					}

					else if (m_bMove[MOVE_Left])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Ssword_Dodge_FL;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Ssword_Dodge_BL;
						}

						else
						{
							m_eAnim_Lower = Ssword_Dodge_L;
						}
					}

					else if (m_bMove[MOVE_Right])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Ssword_Dodge_FR;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Ssword_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Ssword_Dodge_R;
						}
					}
				}
				else
				{
					m_bDodgeBack = true;
					m_eAnim_Lower = Ssword_Dodge_B;
				}

				break;
			}

			case WEAPON_LSword:
			{
				if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Lsword_Dodge_FL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Lsword_Dodge_FR;
						}

						else
						{
							m_eAnim_Lower = Lsword_Dodge_F;
						}
					}

					else if (m_bMove[MOVE_Back])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Lsword_Dodge_BL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Lsword_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Lsword_Dodge_B;
						}
					}

					else if (m_bMove[MOVE_Left])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Lsword_Dodge_FL;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Lsword_Dodge_BL;
						}

						else
						{
							m_eAnim_Lower = Lsword_Dodge_L;
						}
					}

					else if (m_bMove[MOVE_Right])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Lsword_Dodge_FR;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Lsword_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Lsword_Dodge_R;
						}
					}

				}
				else
				{
					m_bDodgeBack = true;
					m_eAnim_Lower = Lsword_Dodge_B;
				}
				break;
			}

			case WEAPON_Hammer:
			{
				if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Hammer_Dodge_FL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Hammer_Dodge_FR;
						}

						else
						{
							m_eAnim_Lower = Hammer_Dodge_F;
						}
					}

					else if (m_bMove[MOVE_Back])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Hammer_Dodge_BL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Hammer_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Hammer_Dodge_B;
						}
					}

					else if (m_bMove[MOVE_Left])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Hammer_Dodge_FL;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Hammer_Dodge_BL;
						}

						else
						{
							m_eAnim_Lower = Hammer_Dodge_L;
						}
					}

					else if (m_bMove[MOVE_Right])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Hammer_Dodge_FR;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Hammer_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Hammer_Dodge_R;
						}
					}
				}

				else
				{
					m_bDodgeBack = true;
					m_eAnim_Lower = Hammer_Dodge_B;
				}
				break;
			}

			case WEAPON_Halberd:
			{
				if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
				{
					if (m_bMove[MOVE_Front])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Halberd_Dodge_F;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Halberd_Dodge_FR;
						}

						else
						{
							m_eAnim_Lower = Halberd_Dodge_FL;
						}
					}

					else if (m_bMove[MOVE_Back])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Halberd_Dodge_BL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Halberd_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Halberd_Dodge_B;
						}
					}

					else if (m_bMove[MOVE_Left])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Halberd_Dodge_FL;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Halberd_Dodge_BL;
						}

						else
						{
							m_eAnim_Lower = Halberd_Dodge_L;
						}
					}

					else if (m_bMove[MOVE_Right])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Halberd_Dodge_FR;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Halberd_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Halberd_Dodge_R;
						}
					}
				}

				else
				{
					m_bDodgeBack = true;
					m_eAnim_Lower = Halberd_Dodge_B;
				}
				break;
			}

			case WEAPON_Gun:
			{
				if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
				{
					m_bDodgeBack = false;

					if (false == m_bOnAiming)
					{
						m_eAnim_Lower = Gun_Dodge_F;
						break;
					}

					if (m_bMove[MOVE_Front])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Gun_Dodge_FL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Gun_Dodge_FR;
						}

						else
						{
							m_eAnim_Lower = Gun_Dodge_F;
						}
					}

					else if (m_bMove[MOVE_Back])
					{
						if (m_bMove[MOVE_Left])
						{
							m_eAnim_Lower = Gun_Dodge_BL;
						}

						else if (m_bMove[MOVE_Right])
						{
							m_eAnim_Lower = Gun_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Gun_Dodge_B;
						}
					}

					else if (m_bMove[MOVE_Left])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Gun_Dodge_FL;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Gun_Dodge_BL;
						}

						else
						{
							m_eAnim_Lower = Gun_Dodge_L;
						}
					}

					else if (m_bMove[MOVE_Right])
					{
						if (m_bMove[MOVE_Front])
						{
							m_eAnim_Lower = Gun_Dodge_FR;
						}

						else if (m_bMove[MOVE_Back])
						{
							m_eAnim_Lower = Gun_Dodge_BR;
						}

						else
						{
							m_eAnim_Lower = Gun_Dodge_R;
						}
					}
				}

				else
				{
					m_bDodgeBack = true;
					m_eAnim_Lower = Gun_Dodge_B;
				}
				break;
			}
			}
		}

		else if (false == m_bOnAiming)
		{
			// 만약 에이밍 중이 아니라면,

			// 아무 방향키도 누르지 않을 시,
			if (!m_bMove[MOVE_Front] && !m_bMove[MOVE_Back] && !m_bMove[MOVE_Right] && !m_bMove[MOVE_Left])
			{
				m_bDodgeBack = true;

				m_eAnim_Lower =
					m_eMainWpnState == WEAPON_SSword ? Ssword_Dodge_B :
					m_eMainWpnState == WEAPON_LSword ? Lsword_Dodge_B :
					m_eMainWpnState == WEAPON_Halberd ? Halberd_Dodge_B :
					m_eMainWpnState == WEAPON_Gun ? Gun_Dodge_B : Hammer_Dodge_B;
			}

			else if (!m_bMove[MOVE_Front] && m_bMove[MOVE_Back] && !m_bMove[MOVE_Right] && !m_bMove[MOVE_Left])
			{
				m_bDodgeBack = true;

				m_eAnim_Lower =
					m_eMainWpnState == WEAPON_SSword ? Ssword_Dodge_B :
					m_eMainWpnState == WEAPON_LSword ? Lsword_Dodge_B :
					m_eMainWpnState == WEAPON_Halberd ? Halberd_Dodge_B :
					m_eMainWpnState == WEAPON_Gun ? Gun_Dodge_B : Hammer_Dodge_B;
			}

			else
			{
				m_eAnim_Lower =
					m_eMainWpnState == WEAPON_SSword ? Ssword_Dodge_F :
					m_eMainWpnState == WEAPON_LSword ? Lsword_Dodge_F :
					m_eMainWpnState == WEAPON_Halberd ? Halberd_Dodge_F :
					m_eMainWpnState == WEAPON_Gun ? Gun_Dodge_F : Hammer_Dodge_F;
			}
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_tObjParam.bIsDodge = true;
		m_bOnDodge = true;
		m_bCanDodge = false;
		m_bStopMovementKeyInput = true;

		m_fSkillMoveMultiply = 1.f;

		m_fSkillMoveSpeed_Cur =
			(m_eMainWpnState == WEAPON_LSword ? 12.f :
				m_eMainWpnState == WEAPON_Hammer ? 12.f :
				m_eMainWpnState == WEAPON_Halberd ? 12.f :
				m_eMainWpnState == WEAPON_SSword ? 15.f :
				m_eMainWpnState == WEAPON_Gun ? 12.f : 15.f);

		m_fSkillMoveMultiply =
			(m_eMainWpnState == WEAPON_LSword ? 0.8f :
				m_eMainWpnState == WEAPON_Hammer ? 0.8f :
				m_eMainWpnState == WEAPON_Halberd ? 0.8f :
				m_eMainWpnState == WEAPON_SSword ? 1.2f :
				m_eMainWpnState == WEAPON_Gun ? 0.6f : 1.2f);

		m_fSkillMoveAccel_Cur = 0.f;
	}

	else
	{
		_v3 vDir = WORLD_LOOK;

		if (true == m_bOnAiming)
		{
			m_bDodgeBack = false;

			if (m_bMove[MOVE_Front])
			{
				if (m_bMove[MOVE_Right])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Left])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = m_pTransform->Get_Axis(AXIS_Z);
			}

			else if (m_bMove[MOVE_Back])
			{
				if (m_bMove[MOVE_Right])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Left])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = -m_pTransform->Get_Axis(AXIS_Z);
			}

			else if (m_bMove[MOVE_Right])
			{
				if (m_bMove[MOVE_Front])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Back])
				{
					V3_NORMAL(&vDir, &(m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = m_pTransform->Get_Axis(AXIS_X);
			}

			else if (m_bMove[MOVE_Left])
			{
				if (m_bMove[MOVE_Front])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + m_pTransform->Get_Axis(AXIS_Z)));
				}

				else if (m_bMove[MOVE_Back])
				{
					V3_NORMAL(&vDir, &(-m_pTransform->Get_Axis(AXIS_X) + -m_pTransform->Get_Axis(AXIS_Z)));
				}

				else
					vDir = -m_pTransform->Get_Axis(AXIS_X);
			}

			else
			{
				m_bDodgeBack = true;
				vDir = -WORLD_LOOK;
			}
		}

		else if (false == m_bOnAiming)
			vDir = (m_bDodgeBack ? -m_pTransform->Get_Axis(AXIS_Z) : m_pTransform->Get_Axis(AXIS_Z));

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
		{
			m_bDodgeBack = false;
			m_bAFK = false;
			m_bOnDodge = false;
			m_bCanDodge = true;
			m_bStopMovementKeyInput = false;

			m_tObjParam.bIsDodge = false;

			if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Right] || m_bMove[MOVE_Left])
			{
				m_eActState = (m_bSprint ? ACT_Dash : ACT_Run);
			}

			else
				m_eActState = ACT_Idle;

			Reset_BattleState();

			return;
		}

		else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.3f))
		{
			m_bCanDodge = true;
			m_bStopMovementKeyInput = false;
		}

		switch (m_eAnim_Lower)
		{
		case Ssword_Dodge_F:
		{
			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f && m_pDynamicMesh->Get_TrackInfo().Position < 2.f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveMultiply = 0.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.267f && m_pDynamicMesh->Get_TrackInfo().Position < 1.1f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveMultiply = 1.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 14.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.267f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveMultiply = 0.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.3f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			return;
		}

		case Ssword_Dodge_B:
		{
			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.333f && m_pDynamicMesh->Get_TrackInfo().Position < 2.867f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_fSkillMoveMultiply = 0.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.3f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.933f && m_pDynamicMesh->Get_TrackInfo().Position < 2.167f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveMultiply = 0.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.3f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.567f && m_pDynamicMesh->Get_TrackInfo().Position < 1.867f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveMultiply = 0.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.3f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.267f && m_pDynamicMesh->Get_TrackInfo().Position < 1.467f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveMultiply = 1.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 12.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.267f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveMultiply = 0.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.3f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			return;
		}
		}

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, vDir);
	}
}

void CPlayer::Play_WeakAtk()
{
	switch (m_eMainWpnState)
	{
	case WEAPON_None:
	{
		Play_Ssword_WeakAtk();
		break;
	}

	case WEAPON_SSword:
	{
		Play_Ssword_WeakAtk();
		break;
	}

	case WEAPON_LSword:
	{
		Play_Lsword_WeakAtk();
		break;
	}

	case WEAPON_Hammer:
	{
		Play_Hammer_WeakAtk();
		break;
	}

	case WEAPON_Halberd:
	{
		Play_Halverd_WeakAtk();
		break;
	}
	case WEAPON_Gun:
	{
		Play_Gun_WeakAtk();
		break;
	}
	}
}

void CPlayer::Play_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_SSword:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_LSword:
		{
			Play_Lsword_HeavyAtk();
			break;
		}

		case WEAPON_Hammer:
		{
			Play_Hammer_HeavyAtk();
			break;
		}

		case WEAPON_Halberd:
		{
			Play_Halverd_HeavyAtk();
			break;
		}
		case WEAPON_Gun:
		{
			Play_Gun_HeavyAtk();
			break;
		}
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eMainWpnState)
		{
		case WEAPON_None:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_SSword:
		{
			Play_Ssword_HeavyAtk();
			break;
		}

		case WEAPON_LSword:
		{
			Play_Lsword_HeavyAtk();
			break;
		}

		case WEAPON_Hammer:
		{
			Play_Hammer_HeavyAtk();
			break;
		}

		case WEAPON_Halberd:
		{
			Play_Halverd_HeavyAtk();
			break;
		}
		case WEAPON_Gun:
		{
			Play_Gun_HeavyAtk();
			break;
		}
		}
	}
}

void CPlayer::Play_Buff()
{
	if (false == m_bOnBuff)
	{
		m_bOnBuff = true;

		m_eAnim_Upper = Cmn_UseItem;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_RightArm;

		m_fAnimMutiply = 1.f;
	}

	else if (true == m_bOnBuff)
	{
		if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
		{
			m_eAnim_Lower = Cmn_Walk_F;
			m_eAnim_Upper = m_eAnim_Lower;
		}

		else
		{
			m_eAnim_Lower = Cmn_UseItemEnchant;
			m_eAnim_Upper = m_eAnim_Lower;
		}

		if (m_pDynamicMesh->Is_Finish_Animation_RightArm(0.95f))
		{
			m_bOnBuff = false;
			m_eActState = ACT_Idle;
		}
	}
}

void CPlayer::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		_int iSwitchNum = CALC::Random_Num(0, 3);

		switch (iSwitchNum)
		{
		case 0:
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Player_Hit);
			g_pSoundManager->Play_Sound(L"Player_Hit_01.wav", CSoundManager::Player_Hit, CSoundManager::Effect_Sound);

			break;
		}
		case 1:
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Player_Hit);
			g_pSoundManager->Play_Sound(L"Player_Hit_02.wav", CSoundManager::Player_Hit, CSoundManager::Effect_Sound);


			break;
		}
		case 2:
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Player_Hit);
			g_pSoundManager->Play_Sound(L"Player_Hit_03.wav", CSoundManager::Player_Hit, CSoundManager::Effect_Sound);


			break;
		}

		case 3:
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Player_Hit);
			g_pSoundManager->Play_Sound(L"Player_Hit_04.wav", CSoundManager::Player_Hit, CSoundManager::Effect_Sound);


			break;
		}
		}

		m_pScreenCornerEffect->Set_Enable(true);
		m_pScreenCornerEffect->Set_LifeTime(0.5f);
		m_pScreenCornerEffect->Set_OnLifeTime(true);

		m_tObjParam.bIsHit = true;

		// 피격 림라이트
		m_pBattleAgent->Set_RimChangeData();

		// 방향별 피격
		_int iHitDir = Check_HitDirection();

		LOOP(16)
		{
			m_bEventTrigger[i] = false;
		}

		m_eAnim_Upper =
			(iHitDir == 0 ? Cmn_Damage_01_F : 
				iHitDir == 1 ? Cmn_Damage_01_B :
				iHitDir == 2 ? Cmn_Damage_01_R : Cmn_Damage_01_L);

		//m_eAnim_Upper =
		//	(iHitDir == 0 ? Cmn_Hit02_F :
		//		iHitDir == 1 ? Cmn_Hit02_B :
		//		iHitDir == 2 ? Cmn_Hit02_R : Cmn_Hit02_L);

		//m_eAnim_Upper =
		//	(iHitDir == 0 ? Cmn_Hit03_F :
		//		iHitDir == 1 ? Cmn_Hit03_B :
		//		iHitDir == 2 ? Cmn_Hit03_R : Cmn_Hit03_L);

		//m_eAnim_Upper =
		//	(iHitDir == 0 ? Cmn_Hit04_F :
		//		iHitDir == 1 ? Cmn_Hit04_B :
		//		iHitDir == 2 ? Cmn_Hit04_R : Cmn_Hit04_L);

		//m_eAnim_Upper =
		//	(iHitDir == 0 ? Cmn_HitBlow_F :
		//		iHitDir == 1 ? Cmn_HitBlow_B :
		//		iHitDir == 2 ? Cmn_HitBlow_R : Cmn_HitBlow_L);

		// 임시 다운 테스트용
		m_tObjParam.bDown = true;

		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_RightArm;


		m_tObjParam.vHitDir = 
			(iHitDir == 0 ? -m_pTransform->Get_Axis(AXIS_Z) :
			iHitDir == 1 ? m_pTransform->Get_Axis(AXIS_Z) :
			iHitDir == 2 ? m_pTransform->Get_Axis(AXIS_X) : -m_pTransform->Get_Axis(AXIS_X));
	}

	else if (true == m_tObjParam.bIsHit)
	{
		_double dAniTime = m_pDynamicMesh->Get_TrackInfo().Position;

		if (m_eAnim_Upper == Cmn_Damage_Blow_F || m_eAnim_Upper == Cmn_Damage_Blow_B ||
			m_eAnim_Upper == Cmn_Damage_Blow_R || m_eAnim_Upper == Cmn_Damage_Blow_L)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				m_eActState = ACT_Down;

				m_tObjParam.bIsHit = false;
				m_tObjParam.bCanHit = true;

				Reset_BattleState();

				m_bOffLerp = true;

				if (m_eAnim_Upper == Cmn_Damage_Blow_R)
					m_pTransform->Add_Angle(AXIS_Y, -90.f);

				else if (m_eAnim_Upper == Cmn_Damage_Blow_L)
					m_pTransform->Add_Angle(AXIS_Y, 90.f);

				m_pTransform->Update_Component();

				return;
			}
		}

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
		{
			_bool	bBlow = false;

			bBlow = (m_eAnim_Upper == Cmn_Damage_Blow_F ? true :
				m_eAnim_Upper == Cmn_Damage_Blow_B ? true :
				m_eAnim_Upper == Cmn_Damage_Blow_R ? true :
				m_eAnim_Upper == Cmn_Damage_Blow_L ? true : false);

			m_eActState = (true == bBlow ? ACT_Down : ACT_Idle);

			m_tObjParam.bIsHit = false;
			m_tObjParam.bCanHit = true;

			Reset_BattleState();

			return;
		}

		else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.25f))
		{
			if (m_tObjParam.bCanHit == false)
			{
				//m_tObjParam.bCanHit = true;
			}
		}

		switch (m_eAnim_Upper)
		{
		case Cmn_Damage_01_F:
		{
			if (dAniTime >= 1.867 && dAniTime < 2.367)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.4f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 0.6)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_01_B:
		{
			if (dAniTime >= 0.967f && dAniTime < 2.f)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.4f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 0.6)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_01_L:
		case Cmn_Damage_01_R:
		{
			if (dAniTime >= 1.2 && dAniTime < 2.333)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.65f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 0.867)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}
		
		//=======================================================================================

		case Cmn_Damage_02_F:
		{
			if (dAniTime >= 2.833 && dAniTime < 3.767)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0.933 && dAniTime < 2.833)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.15f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 0.933)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_02_B:
		{
			if (dAniTime >= 2.067 && dAniTime < 3.6)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.03f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 2.067)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.18f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_02_L:
		{
			if (dAniTime >= 2.700 && dAniTime < 3.467)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 2.067 && dAniTime < 2.600)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 1.3)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.7f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_02_R:
		{
			if (dAniTime >= 3.167 && dAniTime < 3.8)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 1.267 && dAniTime < 3.167)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 1.267)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.7f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		//=======================================================================================

		case Cmn_Damage_03_F:
		{
			if (dAniTime >= 1.733 && dAniTime < 3.733)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.25f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 1.733)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.18f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_03_B:
		{
			if (dAniTime >= 2.300 && dAniTime < 3.6)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.03f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 2.300)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.18f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_03_L:
		{
			if (dAniTime >= 3.667 && dAniTime < 4.367)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.3f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 2.4 && dAniTime < 3.667)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.25f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 2.4)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.18f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_03_R:
		{
			if (dAniTime >= 3.0 && dAniTime < 4.033)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.25f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 2.5 && dAniTime < 3.0)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.15f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 2.5)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.18f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		//=======================================================================================

		case Cmn_Damage_04_F:
		{
			if (dAniTime >= 1.167 && dAniTime < 2.933)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.1f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0.567 && dAniTime < 1.167)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.1f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 0.567)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_04_B:
		{
			if (dAniTime >= 1.833 && dAniTime < 2.233)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.1f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 0.967)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_04_L:
		{
			if (dAniTime >= 1.233 && dAniTime < 2.233)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.1f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (dAniTime >= 0 && dAniTime < 1.233)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		case Cmn_Damage_04_R:
		{
			if (dAniTime >= 0.0 && dAniTime < 1.233)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}

		//=======================================================================================

		case Cmn_Damage_Blow_F:
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				m_fSkillMoveSpeed_Cur = 6.0f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.3f;
			}

			Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
			Decre_Skill_Movement(m_fSkillMoveMultiply);

			break;
		}

		case Cmn_Damage_Blow_B:
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				m_fSkillMoveSpeed_Cur = 6.0f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.3f;
			}

			Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
			Decre_Skill_Movement(m_fSkillMoveMultiply);

			break;
		}

		case Cmn_Damage_Blow_L:
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				m_fSkillMoveSpeed_Cur = 6.0f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.3f;
			}

			Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
			Decre_Skill_Movement(m_fSkillMoveMultiply);

			break;
		}

		case Cmn_Damage_Blow_R:
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				m_fSkillMoveSpeed_Cur = 6.0f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.3f;
			}

			Skill_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
			Decre_Skill_Movement(m_fSkillMoveMultiply);

			break;
		}

		}
	}
}

void CPlayer::Play_Dead()
{
	if (false == m_bIsDead)
	{
		m_bIsDead = true;

		m_eAnim_Upper = Cmn_Dying_Loop;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_RightArm;
	}

	else if (true == m_bIsDead)
	{
		if (m_eAnim_Upper == Cmn_Dying_Loop)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
			{
				m_eAnim_Upper = Cmn_Dying_Death;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_RightArm;

				m_eActState = ACT_Dead;
			}
		}

		else if (m_eAnim_Upper == Cmn_Dying_Death)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.98f))
			{
				m_fAnimMutiply = 0.f;
				m_eActState = ACT_Dead;

				Teleport_ResetOptions(g_eSceneID_Cur, g_eSTeleportID_Cur);
				m_bIsDead = false;
			}
		}
	}
}

void CPlayer::Play_WeaponChange()
{
	if (false == m_bChangeWeapon)
	{
		m_bChangeWeapon = true;

		m_eAnim_Upper = Cmn_WeaponChange;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_fAnimMutiply = 1.f;
	}

	else if (true == m_bChangeWeapon)
	{
		if (m_bMove[MOVE_Front] || m_bMove[MOVE_Back] || m_bMove[MOVE_Left] || m_bMove[MOVE_Right])
		{
			m_eAnim_Lower = Cmn_Walk_F;
		}

		else
			m_eAnim_Lower = Cmn_WeaponChange;

		if (m_pDynamicMesh->Is_Finish_Animation_Upper(0.95f))
		{
			m_eActiveSlot = (m_eActiveSlot == WPN_SLOT_A) ? WPN_SLOT_B : WPN_SLOT_A;

			m_bChangeWeapon = false;

			m_eMainWpnState = m_pWeapon[m_eActiveSlot]->Get_WeaponType();

			m_bOneHand = (
				m_eMainWpnState == WEAPON_SSword ? true :
				m_eMainWpnState == WEAPON_LSword ? false :
				m_eMainWpnState == WEAPON_Halberd ? true :
				m_eMainWpnState == WEAPON_Gun ? true :
				m_eMainWpnState == WEAPON_Hammer ? false : true
				);
			
			m_bOnInvenChange = false;

			m_eActState = ACT_Idle;
		}
	}
}

void CPlayer::Play_Spawn()
{
	const _float SPEED = 0.3f;
	Start_Dissolve(SPEED, true);

	for (auto& iter : m_pWeapon)
		iter->Start_Dissolve(SPEED, true);
	//m_pDrainWeapon->Start_Dissolve(0.7f, true);

	const _float END_VALUE = 0.6f;
	if (m_iPass == 3 && m_fFXAlpha > END_VALUE)
	{
		_v3 vPos = m_pTransform->Get_Pos();
		vPos.y += m_fDissolveY;
		//g_pManagement->Create_Spawn_Effect(vPos);
		m_fDissolveY += DELTA_60 * 1.3f;
	}
	else
		m_fDissolveY = 0.f;
}


void CPlayer::Play_PickUp()
{
	if (false == m_bOnPickUp)
	{
		// 픽업일 땐 못움직임
		m_bOnPickUp = true;
		m_bStopMovementKeyInput = true;

		m_eAnim_Upper = Cmn_PickItemStand;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Upper;

		// 이펙트 ==============================
		LPCSTR tmpChar = "RightHandAttach"; //왼손 뼈 몰라서 임시.
		_mat   matAttach;
		D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);
		matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
		_v3 vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43);
		g_pManagement->Create_ParticleEffect_Delay(L"ItemGet_Particle", 0.3f, 0.15f, vEffPos, nullptr);
	}

	else if (true == m_bOnPickUp)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower())
		{
			m_eActState = ACT_Idle;

			m_bStopMovementKeyInput = false;
			m_bOnPickUp = false;

			// 아이템 줍는 이벤트가 있다면 여기 추가
		}
	}
}

void CPlayer::Play_Down()
{
	if (false == m_bOnDown)
	{
		//Reset_BattleState();

		m_bOnDown = true;

		//if (m_eAnim_Upper == Cmn_HitBlow_R)
		//	m_pTransform->Add_Angle(AXIS_Y, 270.f);

		//else if (m_eAnim_Upper == Cmn_HitBlow_L)
		//	m_pTransform->Add_Angle(AXIS_Y, -270.f);

		//m_pTransform->Update_Component();

		m_eAnim_Upper = m_eAnim_Upper == Cmn_Damage_Blow_F ? Cmn_Down_F_Loop : Cmn_Down_B_Loop;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Upper;

		Reset_BattleState();
	}

	else if (m_bOnDown)
	{
		switch (m_eAnim_Upper)
		{
			//앞으로 자빠져욤
		case Cmn_Down_F_Loop:
		{
			if (m_pDynamicMesh->Is_Finish_Animation(0.85f))
			{
				m_eAnim_Upper = Cmn_Down_F_End;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_Upper;

				return;
			}

			break;
		}

		// 뒤로 자빠져욤
		case Cmn_Down_B_Loop:
		{
			m_fAnimMutiply = 1.f;

			if (m_pDynamicMesh->Is_Finish_Animation(0.9f))
			{
				m_eAnim_Upper = Cmn_Down_B_End;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_Upper;

				m_bOffLerp = false;

				m_fAnimMutiply = 1.f;

				return;
			}

			break;
		}

		//앞으로 자빠졌다가 일어나요
		case Cmn_Down_F_End:
		{
			if (m_pDynamicMesh->Is_Finish_Animation(0.85f))
			{
				m_bOnDown = false;

				m_eActState = ACT_Idle;

				m_eAnim_Upper = Cmn_Idle;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_Upper;

				m_tObjParam.bDown = false;

				Reset_BattleState();

				return;
			}

			break;
		}

		// 뒤로 자빠졌다가 일어나요
		case Cmn_Down_B_End:
		{
			if (m_pDynamicMesh->Is_Finish_Animation(0.85f))
			{
				m_bOnDown = false;

				m_eActState = ACT_Idle;

				m_eAnim_Upper = Cmn_Idle;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_Upper;

				m_tObjParam.bDown = false;

				Reset_BattleState();

				return;
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Summon()
{
	if (false == m_bOnSummon)
	{
		g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
		g_pSoundManager->Play_Sound(L"Go_To_Next_01.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);

		m_bOnSummon = true;

		m_eAnim_Upper = Cmn_CheckPoint_End;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Upper;

		Reset_BattleState();

		Start_Dissolve(0.3f, true, false);

		m_pOuter->Start_Dissolve(0.3f, true, false);
		m_pHair->Start_Dissolve(0.3f, true, false);
		m_pMask[m_eMaskType]->Start_Dissolve(0.3f, true, false);
		m_pHead[m_eHeadType]->Start_Dissolve(0.3f, true, false);
	}

	else if (true == m_bOnSummon)
	{
		if (m_pDynamicMesh->Is_Finish_Animation(0.9f))
		{
			m_eActState = ACT_Idle;

			m_bOnSummon = false;
			m_bCanSummon = false;

			m_fAnimMutiply = 1.f; 
		}
	}
}

void CPlayer::Play_Disappear()
{
}

void CPlayer::Play_BloodSuck()
{
	if (false == m_bOnBloodSuck)
	{
		m_bOnBloodSuck = true;

		m_eAnim_Upper = LongCoat_Charge_Start;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Lower;

		Reset_BattleState();

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_Active(true);
		}

		// 이펙트 ============================================
		LPCSTR tmpChar = "Hips";
		_mat   matAttach;
		D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);
		matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
		_v3 vEffPos = V3_NULL;

		// 시전 스모크
		for (_int i = 1; i < 7; i++)
		{
			_tchar szBuff[256] = L"";
			wsprintf(szBuff, L"Player_Drain_Ink_%d", i);

			vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43)
				+ m_pTransform->Get_Axis(AXIS_Z) * -0.8f
				+ _v3(0.f, -0.25f + _float(CCalculater::Random_Num_Double(0.0, 1.7)), 0.f);

			g_pManagement->Create_Effect_Delay(szBuff, 0.3f, vEffPos);
		}
	}

	else if (true == m_bOnBloodSuck)
	{
		if (false == m_bOnChargeSuck)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
			{
				g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
				g_pSoundManager->Play_Sound(L"Swing_Fast_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

				g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
				g_pSoundManager->Play_Sound(L"Whoosh_Heavy_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);


				m_eAnim_Upper = LongCoat_Charge_End;
				m_eAnim_Lower = m_eAnim_Upper;
				m_eAnim_RightArm = m_eAnim_Upper;
				m_eAnim_LeftArm = m_eAnim_Lower;

				m_bOnChargeSuck = true;

				IF_NOT_NULL(m_pDrainWeapon)
				{
					m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
					m_pDrainWeapon->Set_ActiveCollider(true);
					//m_pDrainWeapon->Set_Active(true);

					m_pDrainWeapon->Set_Target_CanAttack(true);
					m_pDrainWeapon->Set_Enable_Record(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.6f && m_pDynamicMesh->Get_TrackInfo().Position < 1.533f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 0.7f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (false == m_bReadyChargeSuck)
			{
				if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.f)
				{
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Fast_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Whoosh_Heavy_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);


					m_eAnim_Upper = LongCoat_Charge_End;
					m_eAnim_Lower = m_eAnim_Upper;
					m_eAnim_RightArm = m_eAnim_Upper;
					m_eAnim_LeftArm = m_eAnim_Lower;

					m_bOnChargeSuck = true;

					IF_NOT_NULL(m_pDrainWeapon)
					{
						m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
						m_pDrainWeapon->Set_ActiveCollider(true);
						//m_pDrainWeapon->Set_Active(true);

						m_pDrainWeapon->Set_Target_CanAttack(true);
						m_pDrainWeapon->Set_Enable_Record(true);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position < 3.f)
				{
					m_eActState = ACT_Idle;

					m_bOnBloodSuck = false;
					m_bOnChargeSuck = false;
					m_bReadyChargeSuck = false;

					IF_NOT_NULL(m_pDrainWeapon)
					{
						m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
						m_pDrainWeapon->Set_ActiveCollider(false);
						m_pDrainWeapon->Set_Active(false);
						m_pDrainWeapon->Set_ResetOldAnimIdx();

						m_pDrainWeapon->Set_Target_CanAttack(false);
						m_pDrainWeapon->Set_Enable_Record(false);
					}
				}
			}
		}

		else if (true == m_bOnChargeSuck)
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				m_eActState = ACT_Idle;

				m_bOnChargeSuck = false;
				m_bReadyChargeSuck = false;

				IF_NOT_NULL(m_pDrainWeapon)
				{
					m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
					m_pDrainWeapon->Set_ActiveCollider(false);
					m_pDrainWeapon->Set_Active(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.433f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pDrainWeapon->Set_Target_CanAttack(false);
					m_pDrainWeapon->Set_Enable_Record(false);

					// 이펙트 ============================================
					LPCSTR tmpChar = "Hips";
					_mat   matAttach;
					D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);
					matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
					_v3 vEffPos = V3_NULL;
					// 끝날때 스모크
					for (_int i = 1; i < 7; i++)
					{
						_tchar szBuff[256] = L"";
						wsprintf(szBuff, L"Player_Drain_Ink_%d", i);

						vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43)
							+ m_pTransform->Get_Axis(AXIS_Z) * -0.8f
							+ _v3(0.f, _float(CCalculater::Random_Num_Double(0.0, 0.5)), 0.f);

						g_pManagement->Create_Effect_Delay(szBuff, 1.f, vEffPos);
					}
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.933f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pDrainWeapon->Set_Target_CanAttack(true);
					m_pDrainWeapon->Set_Enable_Record(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.3f && m_pDynamicMesh->Get_TrackInfo().Position < 4.333f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 1.4f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.467f && m_pDynamicMesh->Get_TrackInfo().Position < 1.067f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}
}

void CPlayer::Play_BloodSuckCount()
{
	if (false == m_bOnBloodSuck)
	{
		m_bOnBloodSuck = true;

		m_eAnim_Upper = LongCoat_Parry;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Lower;

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_ActiveCollider(true);
			m_pDrainWeapon->Set_Active(true);
			m_pDrainWeapon->Set_ResetOldAnimIdx();
		}
	}

	else if (true == m_bOnBloodSuck)
	{
		//if (m_tObjParam.bIsCounter)
		//	cout << "카운터에효" << endl;

		_double dAniTime = m_pDynamicMesh->Get_TrackInfo().Position;

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
		{
			m_eActState = ACT_Idle;

			m_bOnBloodSuck = false;

			m_pDrainWeapon->Set_Active(false);

			m_pDrainWeapon->Set_ActiveCollider(false);

			m_pDrainWeapon->Set_Target_CanAttack(false);
			m_pDrainWeapon->Set_Enable_Record(false);

			Reset_BattleState();

			return;
		}

		if (dAniTime > 2.833)
		{
			if (false == m_tObjParam.bCanExecution)
			{
				m_eActState = ACT_BloodSuck_Execution;

				m_tObjParam.bCanCounter = false;
				m_tObjParam.bIsCounter = false;

				if (nullptr != pCounterTarget)
				{
					m_pCunterTarget = pCounterTarget;
					pCounterTarget = nullptr;
				}

				m_pCunterTarget->Set_Target_IsRepel(false);
				m_pCunterTarget->Set_Target_CanRepel(false);

				if (ACT_BloodSuck_Execution != m_eActState)
				{
					m_pDrainWeapon->Set_Active(false);
				}

				m_pDrainWeapon->Set_ActiveCollider(false);

				m_pDrainWeapon->Set_Target_CanAttack(false);
				m_pDrainWeapon->Set_Enable_Record(false);

				Reset_BattleState();

				return;
			}
		}

		else if (dAniTime > 2.533)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;

				m_tObjParam.bCanCounter = false;
				m_tObjParam.bIsCounter = false;

				// 이펙트 ============================================
				LPCSTR tmpChar = "Hips";
				_mat   matAttach;
				D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);
				matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
				_v3 vEffPos = V3_NULL;
				// 끝날때 스모크
				for (_int i = 1; i < 7; i++)
				{
					_tchar szBuff[256] = L"";
					wsprintf(szBuff, L"Player_Drain_Ink_%d", i);

					vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43)
						//+ m_pTransform->Get_Axis(AXIS_Z) * -1.f
						+ _v3(0.f, _float(CCalculater::Random_Num_Double(0.0, 0.3)), 0.f);

					g_pManagement->Create_Effect_Delay(szBuff, 0.8f, vEffPos);
				}
			}
		}

		else if (dAniTime > 2.1)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				
			}
		}

		else if (dAniTime > 0.1)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				// 카운터가 가능할 경우
				if (m_tObjParam.bCanCounter)
				{
					m_tObjParam.bCanCounter = false;
					m_tObjParam.bIsCounter = true;
				}

				// 이펙트 ============================================
				LPCSTR tmpChar = "Hips";
				_mat   matAttach;
				D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);
				matAttach = pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat();
				_v3 vEffPos = V3_NULL;

				// 시전 스모크
				for (_int i = 1; i < 7; i++)
				{
					_tchar szBuff[256] = L"";
					wsprintf(szBuff, L"Player_Drain_Ink_%d", i);

					vEffPos = _v3(matAttach._41, matAttach._42, matAttach._43)
						//+ m_pTransform->Get_Axis(AXIS_Z) * -0.8f
						+ _v3(0.f, -0.25f + _float(CCalculater::Random_Num_Double(0.0, 1.7)), 0.f);

					g_pManagement->Create_Effect_Delay(szBuff, 0.1f, vEffPos);
				}
			}
		}
	}
}

void CPlayer::Play_BloodSuckCombo()
{
	if (false == m_bOnBloodSuck)
	{
		m_bOnBloodSuck = true;

		m_eAnim_Upper = LongCoat_Combo;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Upper;

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_Active(true);

			m_pDrainWeapon->Set_Target_CanAttack(true);
			m_pDrainWeapon->Set_Enable_Record(true);
		}
	}

	else if (true == m_bOnBloodSuck)
	{
		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
		{
			m_eActState = ACT_Idle;

			m_bOnBloodSuck = false;
			m_bReadyChargeSuck = false;

			Reset_BattleState();

			IF_NOT_NULL(m_pDrainWeapon)
			{
				m_pDrainWeapon->Set_ActiveCollider(false);
				m_pDrainWeapon->Set_Active(false);

				m_pDrainWeapon->Set_Target_CanAttack(false);
				m_pDrainWeapon->Set_Enable_Record(false);
			}
		}
	}
}

void CPlayer::Play_BloodSuckExecution()
{
	if (false == m_bIsExecution)
	{
		Reset_BattleState();

		m_bIsExecution = true;
		//m_bCanExecution = false;

		// 2번은 컷씬 아닌 처형
		m_eAnim_Upper = LongCoat_Exicution;
		m_eAnim_Lower = m_eAnim_Upper;
		m_eAnim_RightArm = m_eAnim_Upper;
		m_eAnim_LeftArm = m_eAnim_Upper;

		IF_NOT_NULL(m_pDrainWeapon)
		{
			m_pDrainWeapon->Set_AnimIdx(m_eAnim_Upper);
			m_pDrainWeapon->Set_Active(true);
		}

		// 타겟의 처형여부
		m_pCunterTarget->Set_Target_CanExicution(false);
		m_pCunterTarget->Set_Target_Execution_Type(EXE_TYPE::EXECUTION_Back);
		m_pCunterTarget->Set_Target_Execution_Wpn(EXE_WPN::EXECUTION_Wpn_Stinger);
	}

	else if (true == m_bIsExecution)
	{
		_double dAniTime = m_pDynamicMesh->Get_TrackInfo().Position;

		if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.95f))
		{
			m_pCunterTarget = nullptr;
			pCounterTarget = nullptr;

			m_eActState = ACT_Idle;

			m_bIsExecution = false;
			m_bCanExecution = true;

			m_tObjParam.bCanExecution = true;
			m_tObjParam.bIsExecution = false;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 0.f;
			m_fSkillMoveMultiply = 0.f;

			m_tInfo.fMoveAccel_Cur = 0.f;
			m_tInfo.fMoveSpeed_Cur = 0.f;

			Reset_BattleState();

			if(m_pDrainWeapon)
			{
				m_pDrainWeapon->Set_ActiveCollider(false);
				m_pDrainWeapon->Set_Active(false);

				m_pDrainWeapon->Set_Target_CanAttack(false);
				m_pDrainWeapon->Set_Enable_Record(false);
				m_pDrainWeapon->Set_OnExecution(false);
			}
		}

		if (dAniTime > 4.733)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;

				m_pDrainWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (dAniTime > 4.6)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				m_pDrainWeapon->Set_Target_CanAttack(true);
				m_pDrainWeapon->Set_Enable_Record(true);
				m_pDrainWeapon->Set_OnExecution(true);
			}
		}
	}
}

void CPlayer::Play_Skills()
{
	if (false == m_bOnSkill)
	{
		m_bOnSkill = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_pWeapon[m_eActiveSlot]->Set_SkillMode(true); // RedColor
		m_pWeapon[m_eActiveSlot]->Set_TrailUseMask(true, 6);
	}

	else
	{
		switch (m_eAnim_Lower)
		{
		case Renketsu_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if(m_pDynamicMesh->Get_TrackInfo().Position >= 4.866f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.866f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.6f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.2f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 6.7f && m_pDynamicMesh->Get_TrackInfo().Position < 7.1f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					m_fSkillMoveSpeed_Cur = 0.15f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			// 1차 후방 이동
			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.4f && m_pDynamicMesh->Get_TrackInfo().Position < 6.333f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			// 전방 이동 2차
			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.266f && m_pDynamicMesh->Get_TrackInfo().Position < 3.866f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.f;

					m_fAnimMutiply = 1.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			// 전방 이동 1차
			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.06f && m_pDynamicMesh->Get_TrackInfo().Position < 2.7f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.f;

					for (_int i = 0; i < 10; i++)
						g_pManagement->Create_Effect_Delay(L"Player_Skill_RedOnion", 0.f, m_pTransform->Get_Pos());

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_PLAYER_JUMP002.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			// 폭파 이펙트
			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.7f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 2.7f;
					_v3 vEffGroundPos = m_pTransform->Get_Axis(AXIS_Z) * 2.7f;
					_v3 vPlayerPos = m_pTransform->Get_Pos();

					g_pManagement->Create_Effect_Delay(L"Player_Skill_DarkRedOnion_Explosion", 0.f, vPlayerPos + vEffPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_SplitAssert_LaserBefore", 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_SplitAssert_LaserAfter_RingLine", 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_SplitAssert_LaserBody", 0.05f, 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_Blood_Explosion", 0.5f, 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RedParticle_Upper", 0.5f, 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_SplitAssert_LaserAfter_RingLine", 0.f, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_SplitAssert_LaserAfter_Smoke", 0.3f, 0.f, vPlayerPos + vEffGroundPos, nullptr);

					SHAKE_CAM_lv4

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_WEAPON_HIT_009.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_WEAPON_IMPACT_001.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.5f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_JUMP_001.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			// 시작 시 나오는 이펙트
			else if (m_pDynamicMesh->Get_TrackInfo().Position <= 0.1f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					_v3 vEffPos = m_pTransform->Get_Pos() + _v3(0.f, 1.3f, 0.f);
					
					CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(m_pTransform->Get_Pos(), vEffPos);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RedParticle_Explosion", 0.15f, 1.f, m_pTransform->Get_Pos(), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodConeMesh_Explosion", 1.f, m_pTransform->Get_Pos());

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_DIGGING_GROUND_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			break;
		}
		case Renketsu_02:
		{
			_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 1.5f;

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.233f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.033f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.887f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Ring_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Ring_Ver", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedCircle_Flash", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vEffPos, m_pTransform);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.867f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Ver", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Sub_Ver", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Scratch_Ver", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_002.ogg", CSoundManager::Player_SFX_04, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.767f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.967f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Sub_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Scratch_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vEffPos, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_000.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.867f && m_pDynamicMesh->Get_TrackInfo().Position < 3.333f)
			{
				if (m_bEventTrigger[12] == false)
				{
					m_bEventTrigger[12] = true;

					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;

					m_fAnimMutiply = 1.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_tObjParam.bInvisible = false;
					m_pWeapon[m_eActiveSlot]->Set_Invisible(false);
					m_pOuter->Set_Invisible(false);
					m_fAnimMutiply = 1.f;
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.9f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					_v3 vEffPos_Dis = _v3(0.f, 1.5f, 0.f) - m_pTransform->Get_Axis(AXIS_Z) * 1.f;
					m_tObjParam.bCanHit = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.f, vEffPos_Dis, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_JUMP_000.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.167f && m_pDynamicMesh->Get_TrackInfo().Position < 2.1f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 30.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 5.f;

					m_fAnimMutiply = 1.5f;

					m_tObjParam.bInvisible = true;
					m_pWeapon[m_eActiveSlot]->Set_Invisible(true);
					m_pOuter->Set_Invisible(true);
				}

				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					_v3 vEffPos_Dis = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 1.5f;
					//m_tObjParam.bCanHit = false;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.f, vEffPos_Dis, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_QUEENS_KNIGHTS_WARP_001.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.5f)
			{
				if (m_bEventTrigger[11] == false)
				{
					m_bEventTrigger[11] = true;

					// 디졸브 잠깐 빼둠
					// Start_Dissolve(1.f, false);
				}
			}

			break;
		}
		case Renketsu_03:
		{
			_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 2.3f;
			_v3 vEffWindPos = _v3(0.f, 1.5f, 0.f);
			_v3 vPlayerAngleDeg = D3DXToDegree(m_pTransform->Get_Angle());

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}
			else if (m_pDynamicMesh->Get_TrackInfo().Position > 4.267f)
			{
				if (m_bEventTrigger[11] == false)
				{
					m_bEventTrigger[11] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}
			else if (m_pDynamicMesh->Get_TrackInfo().Position > 4.067f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 3.8f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Sub_Hor", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -65.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Hor", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -65.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Scratch_Hor", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -65.f));
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vEffPos + _v3(0, 0.3f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos + _v3(0, 0.3f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -65.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_005.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 3.233f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 3.033f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Sub_Hor", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Hor", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Scratch_Hor", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f, vEffWindPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.2f, vEffWindPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_002.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_002.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 2.5f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 2.2f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Sub_Hor", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Hor", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Scratch_Hor", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.2f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 45.f));
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_001.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_001.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 2.033f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.6f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Distortion_Circle", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Sub_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ScratchBlur_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Scratch_Hor", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f, vEffWindPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.2f, vEffWindPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.3f, vEffWindPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.4f, vEffWindPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion", 0.f, vEffPos + _v3(0, 0.4f, 0.f), m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_000.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.2f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.867f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);

					_v3 vDir = *D3DXVec3Normalize(&_v3(), &-m_pTransform->Get_Axis(AXIS_Z));
					g_pManagement->Create_Effect(L"Player_Skill_WindTornadeMesh", m_pTransform->Get_Pos() + vEffPos, nullptr, vDir, vPlayerAngleDeg);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_JUMP_000.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}
			else if (m_pDynamicMesh->Get_TrackInfo().Position <= 0.1f)
			{
				if (m_bEventTrigger[12] == false)
				{
					m_bEventTrigger[12] = true;

					_v3 vEffPos = m_pTransform->Get_Pos() + _v3(0.f, 1.3f, 0.f);
					CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(m_pTransform->Get_Pos(), vEffPos);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_DIGGING_GROUND_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.367f && m_pDynamicMesh->Get_TrackInfo().Position < 4.633f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}
		case Renketsu_12:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.7f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.5f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.167f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.0f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f && m_pDynamicMesh->Get_TrackInfo().Position < 3.367f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.366f && m_pDynamicMesh->Get_TrackInfo().Position < 2.067f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveMultiply = 0.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.366f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 1.5f;

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.333f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_ShadowAssault_ScratchBlur", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_ShadowAssault_Scratch", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_1", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_2", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_3", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_Effect_Delay(L"Hit_Slash_Particle_0", 0.f, vEffPos, m_pTransform);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RedParticle_Explosion", 0.1f, 0.f, vEffPos, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_WEAPON_IMPACT_001.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					SHAKE_CAM_lv2;
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, m_pTransform->Get_Pos() + _v3(0, 0.5f, 0.f), nullptr);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_005.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					_v3 vEffPos = m_pTransform->Get_Pos() + _v3(0.f, 1.3f, 0.f);
					CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(V3_NULL, vEffPos, m_pTransform);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_DarkSmokeAura"	, 0.5f	, 0.f	, _v3(0, 1.1f, 0.f), m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_DIGGING_GROUND_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			break;
		}

		case Renketsu_05:
		{
			//==========================================================================================================================
			// 메인 이벤트
			//=========================================================================================================================

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.633f)
			{
				if (m_bEventTrigger[11] == false)
				{
					m_bEventTrigger[11] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.433f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);

					SHAKE_CAM_lv3
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.2f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.333f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.5f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.267f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			//==========================================================================================================================
			// 이펙트
			//==========================================================================================================================
			_v3 vEffWindPos = _v3(0.f, 1.5f, 0.f);
			_v3 vPlayerAngleDeg = D3DXToDegree(m_pTransform->Get_Angle());

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.2f)
			{
				if (m_bEventTrigger[16] == false)
				{
					m_bEventTrigger[16] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -80.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f, vEffWindPos + _v3(0.f, -0.3f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.2f, vEffWindPos + _v3(0.f, -0.f, 0.f), m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_BloodConeMesh", 0.1f, V3_NULL, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_002.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_WEAPON_IMPACT_001.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}
			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
			{
				if (m_bEventTrigger[15] == false)
				{
					m_bEventTrigger[15] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.2f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -45.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_001.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
				}
			}
			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f)
			{
				if (m_bEventTrigger[14] == false)
				{
					m_bEventTrigger[14] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.f, vEffWindPos, m_pTransform, _v3(-10.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f, vEffWindPos, m_pTransform, _v3(-10.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.2f, vEffWindPos, m_pTransform, _v3(-10.f, vPlayerAngleDeg.y, 0.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_000.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}
			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
			{
				if (m_bEventTrigger[13] == false)
				{
					m_bEventTrigger[13] = true;

					CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(V3_NULL, V3_NULL, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_DIGGING_GROUND_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			//==========================================================================================================================
			// 이동
			//==========================================================================================================================

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.533f && m_pDynamicMesh->Get_TrackInfo().Position < 5.333f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.067f && m_pDynamicMesh->Get_TrackInfo().Position < 3.467f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.1f && m_pDynamicMesh->Get_TrackInfo().Position < 2.633f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.333f && m_pDynamicMesh->Get_TrackInfo().Position < 1.4f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.333f)
			{
				if (m_bEventTrigger[12] == false)
				{
					m_bEventTrigger[12] = true;

					m_fSkillMoveSpeed_Cur = 0.3f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}
		case Renketsu_06:
		{
			//==========================================================================================================================
			// 메인 이벤트
			//=========================================================================================================================

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.167f)
			{
				if (m_bEventTrigger[11] == false)
				{
					m_bEventTrigger[11] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.967f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);

					SHAKE_CAM_lv3
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.733f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.5f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);

					SHAKE_CAM_lv1
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.3f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.533f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			//==========================================================================================================================
			// 이펙트
			//==========================================================================================================================
			_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 2.3f;
			_v3 vEffWindPos = _v3(0.f, 1.5f, 0.f);
			_v3 vEffGroundPos = m_pTransform->Get_Axis(AXIS_Z) * 1.7f;
			_v3 vPlayerPos = m_pTransform->Get_Pos();
			_v3 vPlayerAngleDeg = D3DXToDegree(m_pTransform->Get_Angle());

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.967f)
			{
				if (m_bEventTrigger[15] == false)
				{
					m_bEventTrigger[15] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_DarkRedOnion_Explosion"			, 0.f			, vPlayerPos + vEffPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_SplitAssert_LaserBefore"			, 0.f			, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_SplitAssert_LaserAfter_RingLine"	, 0.f			, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_SplitAssert_LaserBody"	, 0.05f, 0.f	, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_Blood_Explosion"			, 0.5f, 0.f		, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_RedParticle_Upper"		, 0.5f, 0.f		, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_RedParticle_Explosion"			, 0.f			, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_Effect_Delay(L"Player_Skill_SplitAssert_LaserAfter_RingLine"	, 0.f			, vPlayerPos + vEffGroundPos, nullptr);
					g_pManagement->Create_ParticleEffect_Delay(L"Player_Skill_SplitAssert_LaserAfter_Smoke", 0.3f, 0.f	, vPlayerPos + vEffGroundPos, nullptr);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_WEAPON_HIT_009.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_WEAPON_IMPACT_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.5f)
			{
				if (m_bEventTrigger[14] == false)
				{
					m_bEventTrigger[14] = true;

					g_pManagement->Create_Effect(L"Player_Skill_Rush_Particle_White"			, _v3(0.f, 0.5f, 0.f), m_pTransform, -m_pTransform->Get_Axis(AXIS_Z) * 2.5f);
					g_pManagement->Create_Effect(L"Player_Skill_Rush_LaserBefore"				, vPlayerPos + vEffGroundPos, nullptr, m_pTransform->Get_Axis(AXIS_Y));
					g_pManagement->Create_Effect(L"Player_Skill_Rush_RedParticle_LaserBefore"	, vPlayerPos + vEffGroundPos, nullptr, m_pTransform->Get_Axis(AXIS_Y));
					g_pManagement->Create_Effect(L"Player_Skill_Rush_WhiteParticle_LaserBefore"	, vPlayerPos + vEffGroundPos, nullptr, m_pTransform->Get_Axis(AXIS_Y));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.1f	, vEffWindPos + _v3(0, 0.3f, 0.f), m_pTransform, _v3(-30.f, vPlayerAngleDeg.y, 30.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.15f	, vEffWindPos + _v3(0, 0.3f, 0.f), m_pTransform, _v3(30.f, vPlayerAngleDeg.y, 30.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_WIND_CUT_000.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.9f)
			{
				if (m_bEventTrigger[13] == false)
				{
					m_bEventTrigger[13] = true;

					g_pManagement->Create_AutoFindEffect(L"Player_Skill_Rush_Particle_Yellow"	, 0.9f, m_pTransform, _v3(0.f, 0.5f, 0.f));
					g_pManagement->Create_AutoFindEffect(L"Player_Skill_Rush_Particle_Orange"	, 0.9f, m_pTransform, _v3(0.f, 0.5f, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.25f	, vPlayerPos + vEffWindPos + _v3(0, -0.3f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 1.5f, nullptr, _v3(-90.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.36f	, vPlayerPos + vEffWindPos + _v3(0, -0.3f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 2.2f, nullptr, _v3(-90.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.45f	, vPlayerPos + vEffWindPos + _v3(0, -0.3f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 3.2f, nullptr, _v3(-90.f, vPlayerAngleDeg.y, 0.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.5f	, vPlayerPos + vEffWindPos + _v3(0, -0.3f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 3.8f, nullptr, _v3(-90.f, vPlayerAngleDeg.y, 0.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_MAGIC_SWORD_IMPACT_003.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
			{
				if (m_bEventTrigger[12] == false)
				{
					m_bEventTrigger[12] = true;

					_v3 vEffPos = m_pTransform->Get_Pos() + _v3(0.f, 1.3f, 0.f);
					CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(V3_NULL, vEffPos, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_DIGGING_GROUND_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			//==========================================================================================================================
			// 이동
			//==========================================================================================================================

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.1f && m_pDynamicMesh->Get_TrackInfo().Position < 3.967f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.2f && m_pDynamicMesh->Get_TrackInfo().Position < 2.867f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.433f && m_pDynamicMesh->Get_TrackInfo().Position < 2.2f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 12.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.7f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.2f && m_pDynamicMesh->Get_TrackInfo().Position < 1.433f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.75f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.4f && m_pDynamicMesh->Get_TrackInfo().Position < 1.2f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.4f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 0.17f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}
		case Renketsu_11:
		{
			//==========================================================================================================================
			// 메인 이벤트
			//=========================================================================================================================

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.167f)
			{
				if (m_bEventTrigger[11] == false)
				{
					m_bEventTrigger[11] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.967f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.733f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.5f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.3f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.533f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			//==========================================================================================================================
			// 이펙트
			//==========================================================================================================================
			_v3 vEffPos = _v3(0.f, 1.5f, 0.f) + m_pTransform->Get_Axis(AXIS_Z) * 2.3f;
			_v3 vEffWindPos = _v3(0.f, 1.5f, 0.f);
			_v3 vEffGroundPos = m_pTransform->Get_Axis(AXIS_Z) * 1.7f;
			_v3 vPlayerPos = m_pTransform->Get_Pos();
			_v3 vPlayerAngleDeg = D3DXToDegree(m_pTransform->Get_Angle());

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.2f)
			{
				if (m_bEventTrigger[14] == false)
				{
					m_bEventTrigger[14] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_Torment_Wind_L", 0.10f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -10.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Torment_Wind_R", 0.11f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 10.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Torment_Wind_Distortion_L", 0.10f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, -10.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_Torment_Wind_Distortion_R", 0.11f, vEffWindPos, m_pTransform, _v3(0.f, vPlayerAngleDeg.y, 10.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_005.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f)
			{
				if (m_bEventTrigger[13] == false)
				{
					m_bEventTrigger[13] = true;

					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.10f, vEffWindPos , m_pTransform, _v3(10.f, vPlayerAngleDeg.y, 10.f));
					g_pManagement->Create_Effect_Delay(L"Player_Skill_WindMesh", 0.15f, vEffWindPos , m_pTransform, _v3(10.f, vPlayerAngleDeg.y, 10.f));

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"SE_BLACK_KNIGHT_SWING_002.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
			{
				if (m_bEventTrigger[12] == false)
				{
					m_bEventTrigger[12] = true;

					_v3 vEffPos = m_pTransform->Get_Pos() + _v3(0.f, 1.3f, 0.f);
					CParticleMgr::Get_Instance()->Create_Skill_Start_Effect(V3_NULL, vEffPos, m_pTransform);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SE_DIGGING_GROUND_000.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			//==========================================================================================================================
			// 이동
			//==========================================================================================================================

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.1f && m_pDynamicMesh->Get_TrackInfo().Position < 3.967f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.0f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.2f && m_pDynamicMesh->Get_TrackInfo().Position < 2.867f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.433f && m_pDynamicMesh->Get_TrackInfo().Position < 2.2f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 12.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.7f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.2f && m_pDynamicMesh->Get_TrackInfo().Position < 1.433f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveSpeed_Cur = 0.75f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.4f && m_pDynamicMesh->Get_TrackInfo().Position < 1.2f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.4f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveSpeed_Cur = 0.17f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Buffs()
{
}

void CPlayer::Play_Enchants()
{
}

void CPlayer::Play_OneHandSkill_01()
{
}

void CPlayer::Play_OneHandSkill_02()
{
}

void CPlayer::Play_OneHandskill_03()
{
}

void CPlayer::Play_TwoHandSkill_01()
{
}

void CPlayer::Play_TwoHandSkill_02()
{
}

void CPlayer::Play_TwoHandSkill_03()
{
}

void CPlayer::Play_GunSkill()
{
}

void CPlayer::Play_Ssword_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_eAnim_Lower = Ssword_Down_01;

			m_fSkillMoveSpeed_Cur = 20.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 4.f;
		}
		else
		{
			m_eAnim_Lower = P_NEWANI(Ssword_WeakAtk_01 + (m_sWeakAtkCnt - 1));

			m_fSkillMoveSpeed_Cur = (m_sWeakAtkCnt == 3 ? 8.5f :
									m_sWeakAtkCnt == 4 ? 7.5f : 5.5f);
			m_fSkillMoveAccel_Cur = 0.f;

			m_fSkillMoveMultiply = (m_sWeakAtkCnt == 1 ? 1.f :
									m_sWeakAtkCnt == 3 ? 1.5f :
									m_sWeakAtkCnt == 4 ? 1.35f : 0.75f);
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_Down_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f && m_pDynamicMesh->Get_TrackInfo().Position < 2.53f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"AttackReady.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);


					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}

				if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.95f && m_pDynamicMesh->Get_TrackInfo().Position < 2.53f)
				{
					if (m_bEventTrigger[2] == false)
					{
						m_bEventTrigger[2] = true;

						m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.73f && m_pDynamicMesh->Get_TrackInfo().Position < 2.53f)
				{
					if (m_bEventTrigger[1] == false)
					{
						m_bEventTrigger[1] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					}
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Ssword_WeakAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.3f && m_pDynamicMesh->Get_TrackInfo().Position < 3.43f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.6f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.95f && m_pDynamicMesh->Get_TrackInfo().Position < 1.f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.83f && m_pDynamicMesh->Get_TrackInfo().Position < 0.95f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.53f && m_pDynamicMesh->Get_TrackInfo().Position < 0.83f)
			{
				if (m_bEventTrigger[0] == false)
				{
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SSword_Swing_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.1f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Ssword_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.8f && m_pDynamicMesh->Get_TrackInfo().Position < 2.4f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.6f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.1f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.03f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.43f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;



					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SSword_Swing_03.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			break;
		}

		case Ssword_WeakAtk_03:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f && m_pDynamicMesh->Get_TrackInfo().Position < 3.23f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_fSkillMoveSpeed_Cur = 0.8f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.95f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.2f && m_pDynamicMesh->Get_TrackInfo().Position < 0.9f)
			{
				if (m_pDynamicMesh->Get_TrackInfo().Position > 0.86f)
				{
					if (m_bEventTrigger[1] == false)
					{
						m_bEventTrigger[1] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
						m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.6f)
				{
					if (m_bEventTrigger[10] == false)
					{
						m_bEventTrigger[10] = true;

						g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
						g_pSoundManager->Play_Sound(L"SSword_Swing_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

						g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
						g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.53f)
				{
					if (m_bEventTrigger[0] == false)
					{
						m_bEventTrigger[0] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
						m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
						m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					}
				}


				if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 0.7f)
				{
					Decre_Skill_Movement(m_fSkillMoveMultiply);
					Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
				}
			}

			break;
		}

		case Ssword_WeakAtk_04:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position > 0.95f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.7f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.4f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SSword_Swing_03.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 0.3f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 0.9f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		default:
		{
			Decre_Skill_Movement();
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Ssword_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			m_fSkillMoveMultiply = 1.2f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 11.f;

			m_eAnim_Lower = Ssword_Down_02;
		}
		else
		{
			m_fSkillMoveMultiply = 2.f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 7.f;

			m_eAnim_Lower = Ssword_Charge;
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_Charge:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.65f))
			{
				m_eAnim_Lower = (m_fChargeTimer_Cur > 0.2f ? Ssword_HeavyAtk_02 : Ssword_HeavyAtk_01);
				m_eAnim_Upper = m_eAnim_Lower;
				m_eAnim_RightArm = m_eAnim_Lower;
				m_eAnim_LeftArm = m_eAnim_Lower;

				m_fChargeTimer_Cur = 0.f;

				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveSpeed_Cur = (m_eAnim_Lower == Ssword_HeavyAtk_02 ? 12.f : 8.f);

				m_fSkillMoveMultiply = 1.f;
			}
			break;
		}
		case Ssword_HeavyAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.8f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.66f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.3f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SLASH_Sword_012.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.86f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SSword_Swing_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position > 1.35f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Ssword_HeavyAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.95f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.83f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.33f && m_pDynamicMesh->Get_TrackInfo().Position < 0.83f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"SLASH_Sword_012.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);


					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

			break;
		}

		case Ssword_Down_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;

				return;
			}

			else if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.6f))
			{
				if (m_bMove[MOVE_Front] | m_bMove[MOVE_Back] | m_bMove[MOVE_Right] | m_bMove[MOVE_Left])
				{
					Reset_BattleState();

					m_eActState = ACT_Run;
				}

				return;
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f && m_pDynamicMesh->Get_TrackInfo().Position < 2.1f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
				}

				if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.95f)
				{
					if (m_bEventTrigger[2] == false)
					{
						m_bEventTrigger[2] = true;

						m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
					}
				}

				else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.866f)
				{
					if (m_bEventTrigger[1] == false)
					{
						m_bEventTrigger[1] = true;

						m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					}
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

				m_fAnimMutiply = 1.f;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.3f)
			//else if (m_pDynamicMesh->Get_TrackInfo().Position > 1.35f)
			{
				if (m_fAnimMutiply > 0.5f)
				{
					m_fAnimMutiply -= DELTA_60;

					if (m_fAnimMutiply <= 0.5f)
					{
						m_fAnimMutiply = 0.5f;
					}
				}
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Ssword_DownAtk()
{
}

void CPlayer::Play_Ssword_DashAtk()
{
}

void CPlayer::Play_Gun_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = Gun_WeakAtk_05;
			}

			else
			{
				m_eAnim_Lower = Gun_WeakAtk_10;

				m_fSkillMoveSpeed_Cur = 13.f;
				m_fSkillMoveAccel_Cur = 0.f;
			}
		}
		else
		{
			m_eAnim_Lower = P_NEWANI(Gun_WeakAtk_01 + (m_sWeakAtkCnt - 1));

			m_fSkillMoveSpeed_Cur = (
				m_sWeakAtkCnt == 1 ? 6.f :
				m_sWeakAtkCnt == 2 ? 10.f :
				m_sWeakAtkCnt == 3 ? 8.f : 12.5f);

			m_fSkillMoveAccel_Cur = 0.f;

			m_fSkillMoveMultiply = (
				m_sWeakAtkCnt == 2 ? 1.3f :
				m_sWeakAtkCnt == 2 ? 1.8f :
				m_sWeakAtkCnt == 3 ? 1.1f : 1.3f);
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{

		case Gun_WeakAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.2f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.667f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Slash_Sword_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fase_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.633f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position < 0.88f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.9f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.733f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"Slash_Sword_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_04, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.633f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.83f && m_pDynamicMesh->Get_TrackInfo().Position < 2.56f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 0.8f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_03:
		{

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.3f)
			{
				if (m_bEventTrigger[4] == true)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.667f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Slash_Sword_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.5f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position > 1.92f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position < 1.2f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_04:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.367f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.167f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.833f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"Slash_Sword_04.wav", CSoundManager::Player_SFX_04, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Whoosh_Heavy_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.7f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.55f && m_pDynamicMesh->Get_TrackInfo().Position < 3.3f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.05f && m_pDynamicMesh->Get_TrackInfo().Position < 2.f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_05 :
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.267f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.067f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.8f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.633f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.2f && m_pDynamicMesh->Get_TrackInfo().Position < 2.967f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.7f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveMultiply = 0.12f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Gun_WeakAtk_10:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.3f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.1f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.833f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.733f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.5f && m_pDynamicMesh->Get_TrackInfo().Position < 3.267f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.467f && m_pDynamicMesh->Get_TrackInfo().Position < 1.467f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 12.f;
					m_fSkillMoveMultiply = 2.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}
		}
	}
}

void CPlayer::Play_Gun_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = Gun_WeakAtk_05;
			}

			else
			{
				m_eAnim_Lower = Gun_HeavyAtk_01;
			}
		}
		else
		{
			m_eAnim_Lower = Gun_Charge;
		}

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Gun_Charge:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.65f))
			{
				m_eAnim_Lower = (m_fChargeTimer_Cur > 0.2f ? Gun_HeavyAtk_02 : Gun_HeavyAtk_01);
				m_eAnim_Upper = m_eAnim_Lower;
				m_eAnim_RightArm = m_eAnim_Lower;
				m_eAnim_LeftArm = m_eAnim_Lower;

				m_fChargeTimer_Cur = 0.f;

				m_fSkillMoveAccel_Cur = (m_eAnim_Lower == Gun_HeavyAtk_02 ? 3.f : 1.8f);
				m_fSkillMoveSpeed_Cur = 0.f;

				m_fSkillMoveMultiply = 1.f;
			}
			break;
		}

		case Gun_HeavyAtk_01:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.92f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}
			break;
		}

		case Gun_HeavyAtk_02:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.92f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}
			break;
		}

		//case Gun_HeavyAtk_03_A:
		//{
		//	if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
		//	{
		//		Reset_BattleState();
		//
		//		m_eActState = ACT_Idle;
		//	}
		//
		//	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.0f)
		//	{
		//		if (m_bEventTrigger[1] == false)
		//		{
		//			m_bEventTrigger[1] = true;
		//
		//			// 이펙트 생성
		//		}
		//	}
		//
		//
		//	if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.467f && m_pDynamicMesh->Get_TrackInfo().Position < 3.8f)
		//	{
		//		if (m_bEventTrigger[0] == false)
		//		{
		//			m_bEventTrigger[0] = true;
		//
		//			m_fSkillMoveAccel_Cur = 0.f;
		//			m_fSkillMoveSpeed_Cur = 0.1f;
		//			m_fSkillMoveMultiply = 0.0f;
		//		}
		//
		//		Decre_Skill_Movement(m_fSkillMoveMultiply);
		//		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
		//	}
		//
		//	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.2f && m_pDynamicMesh->Get_TrackInfo().Position < 0.8f)
		//	{
		//		if (m_bEventTrigger[0] == false)
		//		{
		//			m_bEventTrigger[0] = true;
		//
		//			m_fSkillMoveAccel_Cur = 0.f;
		//			m_fSkillMoveSpeed_Cur = 0.15f;
		//			m_fSkillMoveMultiply = 0.0f;
		//		}
		//
		//		Decre_Skill_Movement(m_fSkillMoveMultiply);
		//		Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
		//	}
		//
		//	break;
		//}
		}
	}
}

void CPlayer::Play_Gun_DownAtk()
{
}

void CPlayer::Play_Gun_DashAtk()
{
}

void CPlayer::Play_Halverd_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		m_fAnimMutiply = 1.2f;

		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = Halberd_DownAtk_03;
			}

			else
			{
				m_eAnim_Lower = Halberd_SpecialLaunch;

				m_fSkillMoveMultiply = 0.1f;
				m_fSkillMoveSpeed_Cur = 3.5f;
				m_fSkillMoveAccel_Cur = 0.f;
			}
		}

		else
		{
			if (m_sWeakAtkCnt == 1)
			{
				m_eAnim_Lower = Halberd_WeakAtk_06;
			}

			else if (m_sWeakAtkCnt == 2)
			{
				m_eAnim_Lower = Halberd_WeakAtk_07;
			}

			else if (m_sWeakAtkCnt == 3)
			{
				m_eAnim_Lower = Halberd_WeakAtk_03;
			}

			else if (m_sWeakAtkCnt == 4)
			{
				m_eAnim_Lower = Halberd_WeakAtk_08;
			}

			m_sWeakAtkCnt = 0;
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Halberd_SpecialLaunch:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.267f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.667f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"Monster_Blade_Swing1.ogg", CSoundManager::Player_SFX_04, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.467f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.333f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveMultiply = 0.2f;

					m_fAnimMutiply = 1.5f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Halberd_DownAtk_03:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.233f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.033f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.7f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Monster_Blade_Swing1.ogg", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.6f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.967f && m_pDynamicMesh->Get_TrackInfo().Position < 5.f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.467f && m_pDynamicMesh->Get_TrackInfo().Position < 1.6f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 8.f;
					m_fSkillMoveMultiply = 0.6f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.433f && m_pDynamicMesh->Get_TrackInfo().Position < 0.467f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Halberd_WeakAtk_06 :
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.567f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.367f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.033f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"Monster_Blade_Swing0.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.567f && m_pDynamicMesh->Get_TrackInfo().Position < 3.067f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.8f;
					m_fSkillMoveMultiply = 0.02f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.367f && m_pDynamicMesh->Get_TrackInfo().Position < 1.367f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 3.2f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Halberd_WeakAtk_07:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.333f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.9f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"Monster_Blade_Swing1.ogg", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.4f && m_pDynamicMesh->Get_TrackInfo().Position < 0.933f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Halberd_WeakAtk_08:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.8f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.6f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.067f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Fast_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.533f && m_pDynamicMesh->Get_TrackInfo().Position < 3.267f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.6f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Halberd_WeakAtk_03:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.8f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.6f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.067f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Fast_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.5f && m_pDynamicMesh->Get_TrackInfo().Position < 1.467f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveMultiply = 1.f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		}
	}
}

void CPlayer::Play_Halverd_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		m_fAnimMutiply = 1.2f;

		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = Halberd_DownAtk_03;
			}

			else
			{
				m_eAnim_Lower = Halberd_DownAtk_02;
			}
		}

		else
		{
			m_eAnim_Lower = Halberd_HeavyAtk_06;
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Halberd_HeavyAtk_06:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.767f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.567f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.967f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
					
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.9f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.5f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					// 차징 이펙트
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.4f && m_pDynamicMesh->Get_TrackInfo().Position < 5.667f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.03f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.733f && m_pDynamicMesh->Get_TrackInfo().Position < 3.567f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.267f && m_pDynamicMesh->Get_TrackInfo().Position < 0.767f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Halberd_DownAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.8f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.6f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.167f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.967f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.133f && m_pDynamicMesh->Get_TrackInfo().Position < 1.667f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 3.5f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		}
	}
}

void CPlayer::Play_Halverd_DownAtk()
{
}

void CPlayer::Play_Halverd_DashAtk()
{
}

void CPlayer::Play_Hammer_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = Hammer_WeakAtk_01;
			}
			else
			{
				m_eAnim_Lower = Hammer_WeakAtk_Down_02;

				m_fSkillMoveMultiply = 0.1f;
				m_fSkillMoveSpeed_Cur = 3.5f;
				m_fSkillMoveAccel_Cur = 0.f;
			}
		}

		else
		{
			_float fSpeed = 0.f;
			_float fMultiply = 0.f;

			if (m_sWeakAtkCnt == 1)
			{
				m_eAnim_Lower = Hammer_WeakAtk_07;
				fSpeed = 1.8f;
				fMultiply = 0.01f;
			}

			else if (m_sWeakAtkCnt == 2)
			{
				m_eAnim_Lower = Hammer_WeakAtk_04;
				fSpeed = 3.f;
				fMultiply = 0.2f;
			}

			else if (m_sWeakAtkCnt == 3)
			{
				m_eAnim_Lower = Hammer_WeakAtk_02;
				fSpeed = 2.f;
				fMultiply = 0.1f;
			}

			m_fSkillMoveMultiply = fMultiply;
			m_fSkillMoveSpeed_Cur = fSpeed;
			m_fSkillMoveAccel_Cur = 0.f;
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Hammer_WeakAtk_Down_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.533f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.333f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Metal_Smash.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"HitGround_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
					SHAKE_CAM_lv2
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.0f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.633f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.233f && m_pDynamicMesh->Get_TrackInfo().Position < 2.4f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Hammer_WeakAtk_07:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.833f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.633f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv2;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Metal_Smash.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"HitGround_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.233f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Heavy_Swing_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.33f && m_pDynamicMesh->Get_TrackInfo().Position < 1.98f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case Hammer_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.933f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.733f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Metal_Smash.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"HitGround_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv2
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Heavy_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.067f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position > 0.f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

		}

		case Hammer_WeakAtk_04:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.133f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.933f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Heavy_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.067f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position > 0.3f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Hammer_WeakAtk_01:
		{
			Play_Hammer_DashAtk();
			break;
		}
		}
	}
}

void CPlayer::Play_Hammer_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = Hammer_WeakAtk_01;
			}
			else
			{
				m_fSkillMoveMultiply = 1.f;

				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveSpeed_Cur = 7.f;

				m_eAnim_Lower = Hammer_WeakAtk_Down_01;
			}
		}

		else
		{
			m_fSkillMoveMultiply = 0.2f;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 4.f;

			m_eAnim_Lower = Hammer_HeavyAtk_02;
		}

	}
	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Hammer_HeavyAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 5.6f)
			{
				if (m_bEventTrigger[11] == false)
				{
					m_bEventTrigger[11] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 5.5f)
			{
				if (m_bEventTrigger[10] == false)
				{
					m_bEventTrigger[10] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
					
					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"HitGround_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);
					g_pSoundManager->Play_Sound(L"Hammer_Smash.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv2
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 5.2f)
			{
				if (m_bEventTrigger[9] == false)
				{
					m_bEventTrigger[9] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Heavy_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.833f)
			{
				if (m_bEventTrigger[8] == false)
				{
					m_bEventTrigger[8] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"HitGround_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv2
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.143f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Heavy_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.6f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Hammer_Ground_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_03);
					g_pSoundManager->Play_Sound(L"HitGround_02.wav", CSoundManager::Player_SFX_03, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_04);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv2
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.4f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_01);
					g_pSoundManager->Play_Sound(L"Swing_Heavy_02.wav", CSoundManager::Player_SFX_01, CSoundManager::Effect_Sound);

					g_pSoundManager->Stop_Sound(CSoundManager::Player_SFX_02);
					g_pSoundManager->Play_Sound(L"Swing_Wind_01.wav", CSoundManager::Player_SFX_02, CSoundManager::Effect_Sound);

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 8.2f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_fSkillMoveMultiply = 0.1f;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.9f;

					m_bEventTrigger[1] = true;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 5.2f && m_pDynamicMesh->Get_TrackInfo().Position < 6.05f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_fSkillMoveMultiply = 0.75f;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 6.f;

					m_bEventTrigger[0] = true;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.3f && m_pDynamicMesh->Get_TrackInfo().Position < 4.7f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}
			break;
		}

		case Hammer_WeakAtk_Down_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.2f)
			{
				if (m_bEventTrigger[7] == false)
				{
					m_bEventTrigger[7] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.0f)
			{
				if (m_bEventTrigger[6] == false)
				{
					m_bEventTrigger[6] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv3
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.767f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.25f)
			{
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));

				if (m_fAnimMutiply <= 0.5f)
				{
					m_fAnimMutiply = 2.f;
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.15f)
			{
				if (m_fAnimMutiply > 0.5f)
				{
					m_fAnimMutiply -= DELTA_60;

					if (m_fAnimMutiply <= 0.5f)
					{
						m_fAnimMutiply = 0.5f;
					}
				}
			}

			break;
		}

		case Hammer_WeakAtk_01:
		{
			Play_Hammer_DashAtk();
			break;
		}
		}
	}
}

void CPlayer::Play_Hammer_DownAtk()
{
}

void CPlayer::Play_Hammer_DashAtk()
{
	if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
	{
		Reset_BattleState();

		m_eActState = ACT_Idle;
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.8f)
	{
		if (m_bEventTrigger[6] == false)
		{
			m_bEventTrigger[6] = true;

			m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.6f)
	{
		if (m_bEventTrigger[5] == false)
		{
			m_bEventTrigger[5] = true;

			m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
			m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

			g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
			SHAKE_CAM_lv2
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.267f)
	{
		if (m_bEventTrigger[4] == false)
		{
			m_bEventTrigger[4] = true;

			m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
			m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.833f)
	{
		if (m_bEventTrigger[3] == false)
		{
			m_bEventTrigger[3] = true;

			m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
		}
	}

	if (m_pDynamicMesh->Get_TrackInfo().Position >= 5.1f && m_pDynamicMesh->Get_TrackInfo().Position < 6.467f)
	{
		if (m_bEventTrigger[2] == false)
		{
			m_bEventTrigger[2] = true;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 0.1f;
			m_fSkillMoveMultiply = 0.f;
		}

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.6f && m_pDynamicMesh->Get_TrackInfo().Position < 2.367f)
	{
		if (m_bEventTrigger[1] == false)
		{
			m_bEventTrigger[1] = true;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 0.1f;
			m_fSkillMoveMultiply = 0.f;

			m_fAnimMutiply = 1.f;
		}

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.6f)
	{
		if (m_bEventTrigger[0] == false)
		{
			m_bEventTrigger[0] = true;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 3.f;
			m_fSkillMoveMultiply = 0.2f;

			m_fAnimMutiply = 1.5f;
		}

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
	}
}

void CPlayer::Play_Lsword_WeakAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = LSword_WeakAtk_04;
			}

			else
			{
				m_eAnim_Lower = Ssword_Down_01;
			}
		}

		else
		{
			if (m_sWeakAtkCnt == 1)
			{
				m_eAnim_Lower = LSword_WeakAtk_01;
			}

			else if (m_sWeakAtkCnt == 2)
			{
				m_eAnim_Lower = LSword_WeakAtk_02;
			}

			else if (m_sWeakAtkCnt == 3)
			{
				m_eAnim_Lower = LSword_WeakAtk_03;
			}
		}

		m_bOnAttack = true;

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_Down_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}


			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.567f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.367f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.033f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.367f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.533f && m_pDynamicMesh->Get_TrackInfo().Position < 2.1f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.3f && m_pDynamicMesh->Get_TrackInfo().Position < 1.533f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case LSword_WeakAtk_01:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.833f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.633f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);

					g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon[m_eActiveSlot]->Get_HeadPos());
					SHAKE_CAM_lv2
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.367f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.2f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.433f && m_pDynamicMesh->Get_TrackInfo().Position < 5.600f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.28f;
					m_fSkillMoveMultiply = 0.03f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.633f && m_pDynamicMesh->Get_TrackInfo().Position < 1.633f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;

		}

		case LSword_WeakAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.867f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.667f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 4.267f && m_pDynamicMesh->Get_TrackInfo().Position < 5.067f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.5f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 1.633f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 3.5f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case LSword_WeakAtk_03:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.867f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.2f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}

			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.933f && m_pDynamicMesh->Get_TrackInfo().Position < 5.167f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.8f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.667f && m_pDynamicMesh->Get_TrackInfo().Position < 1.767f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case LSword_WeakAtk_04:
		{
			Play_Lsword_DashAtk();
			break;
		}
		}
	}
}

void CPlayer::Play_Lsword_HeavyAtk()
{
	if (false == m_bOnAttack)
	{
		if (m_bSprint)
		{
			if (m_bCanSprintAtk)
			{
				m_eAnim_Lower = LSword_WeakAtk_04;
			}
			else
				m_eAnim_Lower = Lsword_SpecialLaunch;
		}
		else
		{
			m_eAnim_Lower = Lsword_HeavyAtk_02;
		}

		m_eAnim_Upper = m_eAnim_Lower;
		m_eAnim_RightArm = m_eAnim_Lower;
		m_eAnim_LeftArm = m_eAnim_Lower;

		m_bOnAttack = true;
	}

	else if (true == m_bOnAttack)
	{
		switch (m_eAnim_Lower)
		{
		case Ssword_Charge:
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.65f))
			{
				m_eAnim_Lower = (m_fChargeTimer_Cur > 0.2f ? Lsword_HeavyAtk_02 : Lsword_HeavyAtk_01);
				m_eAnim_Upper = m_eAnim_Lower;
				m_eAnim_RightArm = m_eAnim_Lower;
				m_eAnim_LeftArm = m_eAnim_Lower;

				m_fChargeTimer_Cur = 0.f;

				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveSpeed_Cur = (m_eAnim_Lower == Lsword_HeavyAtk_02 ? 12.f : 8.f);

				m_fSkillMoveMultiply = 1.f;
			}
			break;
		}

		case Lsword_SpecialLaunch:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.2f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.0f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.633f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.267f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 3.4f && m_pDynamicMesh->Get_TrackInfo().Position < 4.533f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 0.9f;
					m_fSkillMoveMultiply = 0.03f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.267f && m_pDynamicMesh->Get_TrackInfo().Position < 1.567f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveMultiply = 0.2f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case Lsword_HeavyAtk_02:
		{
			if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
			{
				Reset_BattleState();

				m_eActState = ACT_Idle;
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.567f)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.367f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.933f)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;

					m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
					m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
				}
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.667f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;

					m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
				}
			}


			if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.133f && m_pDynamicMesh->Get_TrackInfo().Position < 2.533f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 5.5f;
					m_fSkillMoveMultiply = 0.05f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			else if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.f && m_pDynamicMesh->Get_TrackInfo().Position < 0.833f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;

					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveSpeed_Cur = 1.1f;
					m_fSkillMoveMultiply = 0.03f;
				}

				Decre_Skill_Movement(m_fSkillMoveMultiply);
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			}

			break;
		}

		case LSword_WeakAtk_04:
		{
			Play_Lsword_DashAtk();
			break;
		}
		}
	}
}

void CPlayer::Play_Lsword_DownAtk()
{
}

void CPlayer::Play_Lsword_DashAtk()
{
	if (m_pDynamicMesh->Is_Finish_Animation_Lower(0.9f))
	{
		Reset_BattleState();

		m_eActState = ACT_Idle;
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 2.067f)
	{
		if (m_bEventTrigger[5] == false)
		{
			m_bEventTrigger[5] = true;

			m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.867f)
	{
		if (m_bEventTrigger[4] == false)
		{
			m_bEventTrigger[4] = true;

			m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
			m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.533f)
	{
		if (m_bEventTrigger[3] == false)
		{
			m_bEventTrigger[3] = true;

			m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(true);
			m_pWeapon[m_eActiveSlot]->Set_Enable_Record(true);
		}
	}

	else if (m_pDynamicMesh->Get_TrackInfo().Position >= 1.4f)
	{
		if (m_bEventTrigger[2] == false)
		{
			m_bEventTrigger[2] = true;

			m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(true);
		}
	}


	if (m_pDynamicMesh->Get_TrackInfo().Position >= 0.3f && m_pDynamicMesh->Get_TrackInfo().Position < 1.833f)
	{
		if (m_bEventTrigger[0] == false)
		{
			m_bEventTrigger[0] = true;

			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveSpeed_Cur = 2.f;
			m_fSkillMoveMultiply = 0.03f;
		}

		Decre_Skill_Movement(m_fSkillMoveMultiply);
		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
	}
}

void CPlayer::Ready_Bodys()
{
	m_pBody[CCostume_Body::Chara_Body_01] = static_cast<CCostume_Body*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Body",
		&CCostume_Body::_INFO(&m_pTransform->Get_WorldMat(), nullptr, _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Body::Chara_Body_01)));

	m_pBody[CCostume_Body::Chara_Body_01] = static_cast<CCostume_Body*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Body",
		&CCostume_Body::_INFO(&m_pTransform->Get_WorldMat(), nullptr, _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Body::Chara_Body_02)));

	m_pBody[CCostume_Body::Chara_Body_01] = static_cast<CCostume_Body*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Body",
		&CCostume_Body::_INFO(&m_pTransform->Get_WorldMat(), nullptr, _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Body::Chara_Body_03)));

	m_pBody[CCostume_Body::Chara_Body_01] = static_cast<CCostume_Body*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Body",
		&CCostume_Body::_INFO(&m_pTransform->Get_WorldMat(), nullptr, _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Body::Chara_Body_04)));
}

void CPlayer::Ready_Heads()
{
	m_pHead[CCostume_Head::Chara_Head_01] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_01)));

	m_pHead[CCostume_Head::Chara_Head_02] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_02)));

	m_pHead[CCostume_Head::Chara_Head_03] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_03)));

	m_pHead[CCostume_Head::Chara_Head_04] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_04)));

	m_pHead[CCostume_Head::Chara_Head_05] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_05)));

	m_pHead[CCostume_Head::Chara_Head_06] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_06)));

	m_pHead[CCostume_Head::Chara_Head_07] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_07)));

	m_pHead[CCostume_Head::Chara_Head_08] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_08)));

	m_pHead[CCostume_Head::Chara_Head_09] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_09)));

	m_pHead[CCostume_Head::Chara_Head_10] = static_cast<CCostume_Head*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Head",
		&CCostume_Head::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), this, CCostume_Head::Chara_Head_10)));
}

void CPlayer::Ready_Masks()
{
	m_pMask[CCostume_Mask::Mask_06] = static_cast<CCostume_Mask*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Mask",
		&CCostume_Mask::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), CCostume_Mask::Mask_06)));

	m_pMask[CCostume_Mask::Mask_07] = static_cast<CCostume_Mask*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Mask",
		&CCostume_Mask::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), CCostume_Mask::Mask_07)));

	m_pMask[CCostume_Mask::Mask_08] = static_cast<CCostume_Mask*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Mask",
		&CCostume_Mask::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), CCostume_Mask::Mask_08)));

	m_pMask[CCostume_Mask::Mask_10] = static_cast<CCostume_Mask*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Mask",
		&CCostume_Mask::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), CCostume_Mask::Mask_10)));

	m_pMask[CCostume_Mask::Mask_11] = static_cast<CCostume_Mask*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Costume_Mask",
		&CCostume_Mask::_INFO(&m_pTransform->Get_WorldMat(), m_matBones[Bone_Head], _v4(1.f, 1.f, 1.f, 1.f), CCostume_Mask::Mask_11)));
}

void CPlayer::Ready_Weapon()
{
	LPCSTR tmpChar = "RightHandAttach";
	_mat   matAttach;

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);

	m_matHandBone = &pFamre->CombinedTransformationMatrix;

	m_pWeapon[WPN_SLOT_A] = nullptr;
	m_pWeapon[WPN_SLOT_B] = nullptr;

	//m_pWeapon[WPN_SLOT_A] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	//m_pWeapon[WPN_SLOT_A]->Change_WeaponData(Wpn_SSword);
	//m_pWeapon[WPN_SLOT_A]->Set_Friendly(true);
	//m_pWeapon[WPN_SLOT_A]->Set_AttachBoneMartix(m_matHandBone);
	//m_pWeapon[WPN_SLOT_A]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());

	m_bWeaponActive[WPN_SLOT_A] = true;
	m_bWeaponActive[WPN_SLOT_B] = false;

	//========================================================================================================================
	// 여기서 부터 디버그용 무기슬롯
	//========================================================================================================================

	//// 총검
	//m_pWeapon[WPN_SLOT_C] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	//m_pWeapon[WPN_SLOT_C]->Change_WeaponData(CWeapon::Wpn_Gun);
	//
	//m_pWeapon[WPN_SLOT_C]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	//m_pWeapon[WPN_SLOT_C]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	//m_pWeapon[WPN_SLOT_C]->Set_Friendly(true);
	//
	//// 대검
	//m_pWeapon[WPN_SLOT_D] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	//m_pWeapon[WPN_SLOT_D]->Change_WeaponData(CWeapon::Wpn_LSword);
	//
	//m_pWeapon[WPN_SLOT_D]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	//m_pWeapon[WPN_SLOT_D]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	//m_pWeapon[WPN_SLOT_D]->Set_Friendly(true);
	//
	//// 창
	//m_pWeapon[WPN_SLOT_E] = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	//m_pWeapon[WPN_SLOT_E]->Change_WeaponData(CWeapon::Wpn_Halberd);
	//
	//m_pWeapon[WPN_SLOT_E]->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	//m_pWeapon[WPN_SLOT_E]->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	//m_pWeapon[WPN_SLOT_E]->Set_Friendly(true);

	//========================================================================================================================
}

void CPlayer::Ready_DrainWeapon()
{
	LPCSTR tmpChar = "Hips";
	_mat   matAttach;

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);

	m_matTailBone = &pFamre->CombinedTransformationMatrix;

	m_pDrainWeapon = static_cast<CDrain_Weapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_DrainWeapon", NULL));
	m_pDrainWeapon->Set_ParentMatrix(&m_pTransform->Get_WorldMat());
	m_pDrainWeapon->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
}

void CPlayer::Ready_BoneMatrix()
{
	LPCSTR tmpChar = "Head";

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 1);

	m_matBones[Bone_Head] = &pFamre->CombinedTransformationMatrix;

	tmpChar = "Spine";

	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 1);

	m_matBones[Bone_Range] = &pFamre->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFamre->CombinedTransformationMatrix;
}

void CPlayer::Ready_Collider()
{
	//==============================================================================================================
	m_vecPhysicCol.reserve(10);
	//==============================================================================================================
	// 콜라이더 받아오고
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================
}

void CPlayer::Ready_Skills()
{
	SKILL_INFO* pSkillInfo = new SKILL_INFO();

	// 1 - 스플릿 어비스
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_01;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_01;
	pSkillInfo->dwSkillCost = 2; 

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 2 - 팬텀 어썰트
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_02;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_02;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 3 - 블러드 서큘러
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_03;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_03;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 4 - 쉐도우 어썰트
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_12;
	pSkillInfo->eCurSkillIdx = Skill_OneHand_Active_04;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 5 - 리전 피니쉬
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_04;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_01;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();
	
	// 6 - 삼멸살
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_05;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_02;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 7 - 강룡복호
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_06;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_03;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================
	pSkillInfo = new SKILL_INFO();

	// 8 - 토먼트 블레이드
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_11;
	pSkillInfo->eCurSkillIdx = Skill_TwoHand_Active_04;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================

	pSkillInfo = new SKILL_INFO();

	// 9 - 창 스킬
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_Spear;
	pSkillInfo->eCurSkillIdx = Skill_Halverd_Single;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================

	pSkillInfo = new SKILL_INFO();

	// 10 - 총검 스킬
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_Gun;
	pSkillInfo->eCurSkillIdx = Skill_Gun_Single;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================\

	pSkillInfo = new SKILL_INFO();

	// 11 - 버프
	pSkillInfo->bOneHand = true;
	pSkillInfo->dwAnimationIdx = Renketsu_Buff;
	pSkillInfo->eCurSkillIdx = Skill_Buff_Enchant_01;
	pSkillInfo->dwSkillCost = 2;

	LOOP(4)
		pSkillInfo->ePreSkillIdx[i] = Skill_End;

	m_vecFullSkillInfo.push_back(pSkillInfo);

	//====================================================================================

}

void CPlayer::Ready_Rigid()
{
	if (nullptr != m_pRigid)
	{
		m_pRigid->Set_UseGravity(true);							// 중력의 영향 유무

		m_pRigid->Set_IsFall(false);								// 낙하중인지 체크

		m_pRigid->Set_fPower(2.f);								// 점프 파워

		m_pRigid->Set_Speed({ 10.f , 10.f , 10.f });				// 각 축에 해당하는 속도
		m_pRigid->Set_Accel({ 1.f, 0.f, 0.f });					// 각 축에 해당하는 Accel 값
		m_pRigid->Set_MaxAccel({ 2.f , 4.f , 2.f });			// 각 축에 해당하는 MaxAccel 값
	}

}

void CPlayer::Temp_ActiveSkills()
{
}

void CPlayer::Reset_BloodSuck_Options()
{
	if (ACT_BloodSuck != m_eActState && ACT_BloodSuck_Combo != m_eActState && ACT_BloodSuck_Count != m_eActState)
	{
		if (m_bOnBloodSuck == true || m_bOnChargeSuck == true)
		{
			m_bOnBloodSuck = false;
			m_bOnChargeSuck = false;

			m_pDrainWeapon->Set_Active(false);
			m_pDrainWeapon->Set_ActiveCollider(false);
			m_pDrainWeapon->Set_AnimIdx(0);
			m_pDrainWeapon->Set_ResetOldAnimIdx();
		}
	}
}

void CPlayer::Reset_Attack_Bool()
{
	if (m_eActState != ACT_WeakAtk && m_eActState != ACT_HeavyAtk)
	{
		if (false == m_bEventTrigger[0])
		{
			LOOP(16)
				m_bEventTrigger[i] = false;
		}
	}
}

void CPlayer::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	if (fSpeed <= 0.f)
		fSpeed = 0.f;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	m_fCurMoveSpeed = _fspeed;

	// 네비게이션 적용하면 
	m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(m_pRigid, &m_pTransform->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CPlayer::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CPlayer::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CPlayer::Change_Weapon()
{
	// 무기가 없으면 교체 못함
	IF_NULL_RETURN(m_pWeapon[WPN_SLOT_A]);

	// 무기가 없으면 교체 못함
	IF_NULL_RETURN(m_pWeapon[WPN_SLOT_B]);

	if (m_eActState == ACT_WeaponChange)
		return;
	// 바꾼 무기로부터 타입을 받아옴

	// 무기 바꾸는 애니메이션
	m_eActState = ACT_WeaponChange;
	// 여기 무기 바꾸는 코드를 추후에 작성해야 합니다.
}

void CPlayer::Reset_All()
{
}

void CPlayer::Active_UI_Mistletoe(_bool _bResetUI)
{
	_bool bUIActive = false;

	// 활성 상태에 따라 On/Off 판단 , 플레이어 유아이 활성화도 바꿈
	if(!_bResetUI)
		bUIActive = m_bActiveUI = m_pUIManager->Get_MistletoeUI()->Get_Active() ? false : true;

	m_bOnUI_Mistletoe = bUIActive;

	// 스테이지 선택 UI 를 On/Off 시킨다.
	m_pUIManager->Get_MistletoeUI()->Set_Active(bUIActive);

	// 카메라 에임 상태 설정
	m_pCamManager->Set_OnAimingTarget(bUIActive);

	// 비활성화면 리턴
	if (false == bUIActive)
	{
		m_pCamManager->Set_AimXPosMulti(1.f);
		m_pCamManager->Set_AimYPos(0.f);

		// 타겟도 Null 해줘요
		m_pCamManager->Set_AimUI(false);
		m_pCamManager->Set_AimingTarget(nullptr);
		m_pCamManager->Set_MidDistance(g_OriginCamPos);
		m_pCamManager->Set_MouseCtrl(true);
		m_pCamManager->Set_LockAngleX(D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)));

		g_pInput_Device->Set_MouseLock(true);

		m_pRenderer->DOF_On(false);

		return;
	}

	// 타겟 설정
	m_pCamManager->Set_AimXPosMulti(0.2f);
	m_pCamManager->Set_AimYPos(0.f);

	m_pCamManager->Set_AimUI(true);
	m_pCamManager->Set_AimingTarget(m_pUIManager->Get_MistletoeUI());
	m_pCamManager->Set_MidDistance(1.5f);
	m_pCamManager->Set_MouseCtrl(false);
	g_pInput_Device->Set_MouseLock(false);

	m_pRenderer->DOF_On(true);
}

void CPlayer::Active_UI_Inventory(_bool _bResetUI)
{
	// 전체 UI 를 활성화 시킨다.
	m_pUIManager->Get_Total_Inven()->Set_Active(true);
}

void CPlayer::Active_UI_StageSelect(_bool _bResetUI)
{
	// 활성 상태에 따라 On/Off 판단
	_bool bUIActive = m_pUIManager->Get_StageSelectUI()->Get_Active() ? false : true;

	// 스테이지 선택 UI 를 On/Off 시킨다.
	m_pUIManager->Get_StageSelectUI()->Set_Active(bUIActive);

	// 선택이 됫는지 안됫는지
	m_bOnUI_StageSelect = bUIActive;

	// 비활성화면 리턴
	if (!bUIActive)
	{
		m_pCamManager->Set_MidDistance(1.5f);
		m_pCamManager->Set_AimXPosMulti(0.2f);
		m_pCamManager->Set_AimYPos(0.f);

		m_pUIManager->Get_MistletoeUI()->Set_Active(true);
		m_pCamManager->Set_OnAimingTarget(true);
		m_pCamManager->Set_AimingTarget(m_pUIManager->Get_MistletoeUI());

		return;
	}

	// 상위인 겨우살이 끕니다.
	m_pUIManager->Get_MistletoeUI()->Set_Active(false);

	// 카메라 에임 상태 설정
	m_pCamManager->Set_OnAimingTarget(bUIActive);
	m_pCamManager->Set_AimUI(true);
	m_pCamManager->Set_AimingTarget(m_pUIManager->Get_StageSelectUI());

	m_pCamManager->Set_MidDistance(1.5f);
	m_pCamManager->Set_AimXPosMulti(0.2f);
	m_pCamManager->Set_AimYPos(-0.5f);
}

void CPlayer::Active_UI_NPC(_bool _bResetUI)
{
	// NPC UI 를 활성화 시킨다.
	m_pUIManager->Get_MistletoeUI()->Set_Active(true);
}

void CPlayer::Active_UI_BloodCode(_bool _bResetUI)
{
	// 활성 상태에 따라 On/Off 판단
	_bool bUIActive = m_pUIManager->Get_BloodCode_Menu()->Get_Active() ? false : true;

	// 스테이지 선택 UI 를 On/Off 시킨다.
	m_pUIManager->Get_BloodCode_Menu()->Set_Active(bUIActive);

	// 선택이 됫는지 안됫는지
	m_bOnUI_BloodCode = bUIActive;

	// 비활성화면 리턴
	if (!bUIActive)
	{
		m_pCamManager->Set_MidDistance(1.5f);
		m_pCamManager->Set_AimXPosMulti(0.5f);
		m_pCamManager->Set_AimYPos(0.f);

		m_pUIManager->Get_MistletoeUI()->Set_Active(true);
		m_pCamManager->Set_OnAimingTarget(true);
		m_pCamManager->Set_AimingTarget(m_pUIManager->Get_MistletoeUI());

		return;
	}

	m_pUIManager->Get_MistletoeUI()->Set_Active(false);

	m_pCamManager->Set_AimXPosMulti(-1.5f);
	m_pCamManager->Set_AimYPos(-0.2f);

	// 카메라 에임 상태 설정
	m_pCamManager->Set_OnAimingTarget(bUIActive);
	m_pCamManager->Set_AimUI(true);
	m_pCamManager->Set_AimingTarget(m_pUIManager->Get_StageSelectUI());
	m_pCamManager->Set_MidDistance(2.5f);
}

void CPlayer::Active_UI_WeaponShop_Yakumo(_bool _bResetUI)
{
	// 활성 상태에 따라 On/Off 판단
	_bool bUIActive = m_bActiveUI = m_pUIManager->Get_WeaponShopUI()->Get_Active() ? false : true;

	// 스테이지 선택 UI 를 On/Off 시킨다.
	m_pUIManager->Get_WeaponShopUI()->Set_Active(bUIActive);

	// 선택이 됫는지 안됫는지
	m_bOnYakumo_UI = bUIActive;

	// 비활성화면 리턴
	if (!bUIActive)
	{
		//if (!m_pUIManager->Get_WeaponShopUI()->Get_OtherPopupOn())
		//{
			m_bOnUI_NPCTalk = false;

			m_pCamManager->Set_AimXPosMulti(1.f);
			m_pCamManager->Set_AimYPos(0.f);

			// 타겟도 Null 해줘요
			m_pCamManager->Set_AimUI(false);
			m_pCamManager->Set_OnAimingTarget(false);
			m_pCamManager->Set_AimingTarget(nullptr);
			m_pCamManager->Set_MidDistance(g_OriginCamPos);
			m_pCamManager->Set_MouseCtrl(true);
			m_pCamManager->Set_LockAngleX(D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)));

			g_pInput_Device->Set_MouseLock(true);

			m_pRenderer->DOF_On(false);
		//}
		return;
	}

	// 카메라 에임 상태 설정
	m_pCamManager->Set_OnAimingTarget(bUIActive);
	m_pCamManager->Set_AimUI(true);
	m_pCamManager->Set_AimingTarget(m_pUIManager->Get_WeaponShopUI());

	m_pCamManager->Set_MidDistance(1.5f);
	m_pCamManager->Set_AimXPosMulti(-0.5f);
	m_pCamManager->Set_AimYPos(0.5f);
	m_pCamManager->Set_MouseCtrl(false);
	g_pInput_Device->Set_MouseLock(false);

	m_pRenderer->DOF_On(true);
}

void CPlayer::Active_UI_MaterialShop_Yokumo(_bool _bResetUI)
{
	// 활성 상태에 따라 On/Off 판단
	_bool bUIActive = m_bActiveUI = m_pUIManager->Get_Yokumo_NPCUI()->Get_Active() ? false : true;

	// 스테이지 선택 UI 를 On/Off 시킨다.
	m_pUIManager->Get_Yokumo_NPCUI()->Set_Active(bUIActive);

	// 선택이 됫는지 안됫는지
	m_bOnYokumo_UI = bUIActive;

	// 비활성화면 리턴
	if (!bUIActive)
	{
		m_bOnUI_NPCTalk = false;

		m_pCamManager->Set_AimXPosMulti(1.f);
		m_pCamManager->Set_AimYPos(0.f);

		// 타겟도 Null 해줘요
		m_pCamManager->Set_AimUI(false);
		m_pCamManager->Set_OnAimingTarget(false);
		m_pCamManager->Set_AimingTarget(nullptr);
		m_pCamManager->Set_MidDistance(g_OriginCamPos);
		m_pCamManager->Set_MouseCtrl(true);
		m_pCamManager->Set_LockAngleX(D3DXToDegree(m_pTransform->Get_Angle(AXIS_Y)));

		g_pInput_Device->Set_MouseLock(true);

		m_pRenderer->DOF_On(false);

		return;
	}

	// 카메라 에임 상태 설정
	m_pCamManager->Set_OnAimingTarget(bUIActive);
	m_pCamManager->Set_AimUI(true);
	m_pCamManager->Set_AimingTarget(m_pUIManager->Get_Yokumo_NPCUI());

	m_pCamManager->Set_MidDistance(1.5f);
	m_pCamManager->Set_AimXPosMulti(-0.5f);
	m_pCamManager->Set_AimYPos(0.5f);
	m_pCamManager->Set_MouseCtrl(false);
	g_pInput_Device->Set_MouseLock(false);

	m_pRenderer->DOF_On(true);
}

void CPlayer::Active_UI_LockOn(_bool _bResetUI)
{
	if (false == _bResetUI)
	{
		CMesh_Dynamic*	pMeshDynamic = static_cast<Engine::CMesh_Dynamic*>((m_pTarget)->Get_Component(L"Com_Mesh"));
		CTransform*		pTtransform = TARGET_TO_TRANS(m_pTarget);

		LPCSTR tmpChar = "Hips";

		D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)pMeshDynamic->Get_BonInfo(tmpChar, 0);
		_mat* matBone = &pFamre->CombinedTransformationMatrix;
		_mat* matTarget = &pTtransform->Get_WorldMat();

		m_pLockOn_UI->Set_Enable(true);
		m_pLockOn_UI->Set_Bonmatrix(matBone);
		m_pLockOn_UI->Set_TargetWorldmatrix(matTarget);
	}

	else
	{
		m_pLockOn_UI->Set_Enable(false);
		m_pLockOn_UI->Reset_TargetMatrix();
	}
}

void CPlayer::UI_Check()
{

}

HRESULT CPlayer::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_BodyInner_01", L"Com_MeshDynamic", (CComponent**)&m_pDynamicMesh)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	// for.Com_RigidBody
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Rigidbody", L"Com_Rigidbody", (CComponent**)&m_pRigid)))
		return E_FAIL;


	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });


	return NOERROR;
}

HRESULT CPlayer::SetUp_Default()
{
	ZeroMemory(&m_tInfo, sizeof(ACTOR_INFO));

	// Transform
	//m_pTransform->Set_Pos(_v3(150.484f, -18.08f, 70.417f));
	m_pTransform->Set_Pos(_v3(0.f, 0.f, 0.f));
	m_pTransform->Set_Scale(V3_ONE);

	// Mesh
	m_pDynamicMesh->SetUp_Animation_Lower(Cmn_Idle);

	// Info
	m_tInfo.fMoveAccel_Cur = 0.f;
	m_tInfo.fMoveSpeed_Max = 1.5f;
	m_tInfo.fMoveAccel_Cur = 0.f;
	m_tInfo.fMoveSpeed_Max = 5.f;

	// Parameter
	m_tObjParam.bCanExecution = true;
	m_tObjParam.bCanRepel = true;
	m_tObjParam.bCanCounter = true;
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsDodge = false;
	m_tObjParam.fHp_Cur = 1000.f;
	m_tObjParam.fHp_Max = 1000.f;
	
	// Anim
	m_fAnimMutiply = 1.f;

	// Navi
	//m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, L"Navmesh_Stage_03.dat");
	//m_pNavMesh->Set_SubsetIndex(0);
	//m_pNavMesh->Set_Index(0);

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	//_v3 vLightPos = _v3(50.f, -100.f, 0.f);
	//V3_NORMAL_SELF(&vLightPos);

	//if (FAILED(pShader->Set_Value("g_vLightDir", &vLightPos, sizeof(_mat))))
	//	return E_FAIL;
	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 0.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 최소 단위의 빛을 더해줌.
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.1f;	// ID_B	: B채널 ID 값 , 1이 최대

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

	return NOERROR;
}

void CPlayer::OnCollisionEnter()
{
	//list<CGameObject*> tmpList =  g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	//
	//for (auto& iter : tmpList)
	//{
	//	CCollider* pCollider = TARGET_TO_COL(iter);
	//
	//	if (m_pCollider->Check_Sphere(pCollider, m_pTransform->Get_Axis(AXIS_Z), m_tInfo.fMoveSpeed_Cur * DELTA_60))
	//	{
	//		CTransform* pTrans = TARGET_TO_TRANS(iter);
	//		CNavMesh*   pNav = TARGET_TO_NAV(iter);
	//
	//		// 방향 구해주고
	//		_v3 vDir = m_pTransform->Get_Pos() - pTrans->Get_Pos();
	//		V3_NORMAL_SELF(&vDir);
	//
	//		// y축 이동은 하지말자
	//		vDir.y = 0;
	//
	//		// 네비 메쉬타게 끔 세팅
	//		pTrans->Set_Pos(pNav->Move_OnNaviMesh(m_pRigid, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
	//	}
	//}
}

void CPlayer::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = *m_matBones[matrixIdx] * m_pTransform->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	m_pCollider->Update(m_pTransform->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
}

void CPlayer::Reset_BattleState()
{
	m_fAnimMutiply = 1.f;

	m_bOnAttack = false;
	m_bCanAttack = true;
	m_bCharging = false;
	m_bOnSkill = false;
	m_bStopMovementKeyInput = false;

	m_tObjParam.bIsDodge = false;
	m_tObjParam.bCanAttack = true;
	m_tObjParam.bIsAttack = false;
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;
	m_bOnDodge = false;
	m_bCanDodge = true;

	m_sWeakAtkCnt = 0;
	m_sHeavyAtkCnt = 0;

	m_fChargeTimer_Cur = 0.f;

	if (nullptr != m_pWeapon[m_eActiveSlot])
	{
		m_pWeapon[m_eActiveSlot]->Set_Target_CanAttack(false);
		m_pWeapon[m_eActiveSlot]->Set_Enable_Trail(false);
		m_pWeapon[m_eActiveSlot]->Set_Enable_Record(false);
		m_pWeapon[m_eActiveSlot]->Set_SkillMode(false); // WhiteColor
		m_pWeapon[m_eActiveSlot]->Set_TrailUseMask(false, 6);
	}

	LOOP(32)
		m_bEventTrigger[i] = false;
}

void CPlayer::Check_NPC()
{
	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (m_bActiveUI)
		return;

	if (g_pManagement->Get_GameObjectList(L"Layer_Mistletoe", SCENE_STAGE).empty())
		return;

	for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Mistletoe", SCENE_STAGE))
	{
		if (false == iter->Get_Enable())
			continue;

		CTransform* pIterTrans = TARGET_TO_TRANS(iter);

		if (1.5f >= V3_LENGTH(&(m_pTransform->Get_Pos() - pIterTrans->Get_Pos())))
		{
			m_bOnUI_Mistletoe = true;
			return;
		}
	}

	m_bOnUI_Mistletoe = false;
}

void CPlayer::Check_Mistletoe()
{
	if (m_eActState == ACT_BloodSuck_Execution)
		return;

	if (m_bActiveUI)
		return;

	if (g_pManagement->Get_GameObjectList(L"Layer_Mistletoe", SCENE_STAGE).empty())
		return;

	for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Mistletoe", SCENE_STAGE))
	{
		if (false == iter->Get_Enable())
			continue;

		CTransform* pIterTrans = TARGET_TO_TRANS(iter);

		if (1.5f >= V3_LENGTH(&(m_pTransform->Get_Pos() - pIterTrans->Get_Pos())))
		{
			m_bOnUI_Mistletoe = true;
			return;
		}
	}

	m_bOnUI_Mistletoe = false;
}

_int CPlayer::Check_HitDirection()
{
	_float fHitAngle = D3DXToDegree(m_pTransform->Calc_HitTarget_Angle(m_tObjParam.vHitPos));

	_int eDirection = MOVE_Front;

	if (fHitAngle >= 0.f && fHitAngle < 45.f)
	{
		eDirection = MOVE_Front;
	}

	else if (fHitAngle >= 45.f && fHitAngle < 135.f)
	{
		eDirection = MOVE_Right;
	}

	else if (fHitAngle >= 135.f && fHitAngle < 180.f)
	{
		eDirection = MOVE_Back;
	}

	else if (fHitAngle >= -180.f && fHitAngle < -135.f)
	{
		eDirection = MOVE_Back;
	}

	else if (fHitAngle >= -135.f && fHitAngle < -45.f)
	{
		eDirection = MOVE_Left;
	}

	else if (fHitAngle >= -45.f && fHitAngle < 0.f)
	{
		eDirection = MOVE_Front;
	}

	return eDirection;
}

_bool CPlayer::Check_CunterAngle(CGameObject* pObj)
{
	_v3 vTargetPos = TARGET_TO_TRANS(pObj)->Get_Pos();
	_float fHitAngle = D3DXToDegree(m_pTransform->Calc_HitTarget_Angle(vTargetPos));

	_bool bOnFront = false;

	if (fHitAngle >= 0.f && fHitAngle < 15.f)
	{
		bOnFront = true;
	}

	else if (fHitAngle >= -15.f && fHitAngle < 0.f)
	{
		bOnFront = true;
	}

	return bOnFront;
}

_bool CPlayer::Check_CunterTarget()
{
	m_pCunterTarget = nullptr;

	CGameObject*	pCunterTarget = nullptr;
	_float			fOldLength = 999.f;

	for (auto& iter : g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE))
	{
		if (true == iter->Get_Dead())
			continue;

		if (false == iter->Get_Enable())
			continue;

		// 거리를 재서
		_float fLength = D3DXVec3Length(&(TARGET_TO_TRANS(iter)->Get_Pos() - m_pTransform->Get_Pos()));

		// 최소거리보다 멀면 다음으로
		if(fLength > 2.f)
			continue;

		// 만약 이전 거리보다 멀면
		if (fOldLength <= fLength)
			continue;

		// 거리 내에 있는데, 각도 안에 있는지도 체크
		if(false == Check_CunterAngle(iter))
			continue;

		// 거리와 타겟을 갱신
		fOldLength = fLength;
		pCunterTarget = iter;
	}

	// 타겟이 null이 아니라면,
	if (nullptr != pCunterTarget)
	{
		// 카운터 타겟 갱신
		m_pCunterTarget = pCunterTarget;
		return true;
	}

	return false;
}

void CPlayer::Check_Stamina(_double dTimeDelta)
{
	m_tObjParam.fStamina_Cur += m_fStamina_RecoverySpeed * _float(dTimeDelta);
	if (m_tObjParam.fStamina_Cur > m_tObjParam.fStamina_Max)
		m_tObjParam.fStamina_Cur = m_tObjParam.fStamina_Max;

}

void CPlayer::Check_Cheat()
{
	if (m_bCheat_HP)
		m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	if (m_bCheat_SP)
		m_tObjParam.sMana_Cur = m_tObjParam.sMana_Max;
	if (m_bCheat_ST)
		m_tObjParam.fStamina_Cur = m_tObjParam.fStamina_Max;

	if (g_pInput_Device->Key_Down(DIK_NUMPAD4))
	{
		m_bCheat_HP = !m_bCheat_HP;
		if (m_bCheat_HP)
		{
			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			g_pSoundManager->Play_Sound(L"SE_FIRE_ELEMENTS_000.ogg", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			g_pSoundManager->Play_Sound(L"SE_AURORA_KETSUGI_MOVE_ICE_000.ogg", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
		}
	}
	if (g_pInput_Device->Key_Down(DIK_NUMPAD5))
	{
		m_bCheat_SP = !m_bCheat_SP;

		if (m_bCheat_SP)
		{
			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			g_pSoundManager->Play_Sound(L"SE_FIRE_ELEMENTS_000.ogg", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			g_pSoundManager->Play_Sound(L"SE_AURORA_KETSUGI_MOVE_ICE_000.ogg", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
		}
	}
	if (g_pInput_Device->Key_Down(DIK_NUMPAD6)) 
	{
		m_bCheat_ST = !m_bCheat_ST;

		if (m_bCheat_ST)
		{
			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			g_pSoundManager->Play_Sound(L"SE_FIRE_ELEMENTS_000.ogg", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
			g_pSoundManager->Play_Sound(L"SE_AURORA_KETSUGI_MOVE_ICE_000.ogg", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
		}
	}
	if (g_pInput_Device->Key_Down(DIK_NUMPAD7))
	{
		m_pUIManager->Get_HazeUI()->Accumulate_Haze(10000);
		m_pUIManager->Get_Material_Inven()->Add_MultiMaterial(CMaterial::Queen_Steel, 30);
		m_pUIManager->Get_Material_Inven()->Add_MultiMaterial(CMaterial::Queen_Titanium, 30);
		m_pUIManager->Get_Material_Inven()->Add_MultiMaterial(CMaterial::Queen_Tungsten, 30);

		g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
		g_pSoundManager->Play_Sound(L"UI_Money_0.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
	}
}

void CPlayer::Change_PlayerBody(PLAYER_BODY _eBodyType)
{
	if (_eBodyType == m_ePlayerBody)
		return;

	m_ePlayerBody = _eBodyType;

	_tchar szBodyName[STR_128] = L"";

	lstrcpy(szBodyName, 
		(m_ePlayerBody == Player_Body_01 ? L"Mesh_BodyInner_01" :
			m_ePlayerBody == Player_Body_02 ? L"Mesh_BodyInner_02" :
			m_ePlayerBody == Player_Body_03 ? L"Mesh_BodyInner_03" : L"Mesh_BodyInner_04"
		));

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_MeshDynamic");
	Safe_Release(iter->second);

	// Static 멤버변수는 처음에 Clone 할때 AddRef 해준다., 
	Safe_Release(m_pDynamicMesh);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pDynamicMesh = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, szBodyName));
	Safe_AddRef(iter->second);

	Ready_BoneMatrix();

	LPCSTR tmpChar = "RightHandAttach";

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 2);

	m_matHandBone = &pFamre->CombinedTransformationMatrix;

	tmpChar = "Hips";

	pFamre = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(tmpChar, 0);

	m_matTailBone = &pFamre->CombinedTransformationMatrix;

	m_pHair->Set_AttachBoneMartix(m_matBones[Bone_Head]);
	m_pHead[m_eHeadType]->Set_AttachBoneMartix(m_matBones[Bone_Head]);
	m_pMask[m_eMaskType]->Set_AttachBoneMartix(m_matBones[Bone_Head]);
	m_pWeapon[m_eActiveSlot]->Set_AttachBoneMartix(m_matHandBone);
	m_pDrainWeapon->Set_AttachBoneMartix(m_matTailBone);
}

void CPlayer::Update_OuterAnim()
{
}

CPlayer * CPlayer::Create(_Device pGraphic_Device)
{
	CPlayer*	pInstance = new CPlayer(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CPlayer::Clone_GameObject(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	LOOP(5)
	{
		Safe_Release(m_pWeapon[i]);
	}

	for (auto& iter : m_vecFullSkillInfo)
	{
		Safe_Delete(iter);
	}

	m_vecFullSkillInfo.shrink_to_fit();
	m_vecFullSkillInfo.clear();

	for (auto& iter : m_vecActiveSkillInfo)
	{
		Safe_Delete(iter);
	}

	Safe_Release(m_pScreenCornerEffect);

	m_vecActiveSkillInfo.shrink_to_fit();
	m_vecActiveSkillInfo.clear();

	Safe_Release(m_pLockOn_UI);
	Safe_Release(m_pScreenCornerFade);

	Safe_Release(m_pDrainWeapon);
	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pDynamicMesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pBattleAgent);
	Safe_Release(m_pRigid);

	Safe_Release(m_pUIManager);
	Safe_Release(m_pCamManager);
	Safe_Release(m_pStageAgent);
	Safe_Release(m_pScriptManager);

	m_pCunterTarget = nullptr;

	for (auto& iter : m_matBones)
	{
		iter = nullptr;
	}

	Safe_Release(m_pHair);
	//Safe_Release(m_pOuter);

	for (auto& iter : m_pMask)
		Safe_Release(iter);

	for (auto& iter : m_pBody)
		Safe_Release(iter);

	for (auto& iter : m_pHead)
		Safe_Release(iter);

	CGameObject::Free();
}
