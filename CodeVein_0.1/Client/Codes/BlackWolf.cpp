#include "stdafx.h"
#include "..\Headers\BlackWolf.h"

CBlackWolf::CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBlackWolf::CBlackWolf(const CBlackWolf & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackWolf::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CBlackWolf::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	//체력 0이하인 경우 사망
	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", 10);
	pBlackBoard->Set_Value(L"MAXHP", 10);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);

	//CBT_Selector* Start_Sel = Node_Selector("행동 시작");
	CBT_Sequence* Start_Sel = Node_Sequence("행동 시작"); //원래 공격 해야합니다

	CBT_UpdatePos* UpdatePlayerPosService = Node_UpdatePos("Update_Player_Pos", L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE)), 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdatePlayerHPservice = Node_UpdateGageRatio("Update_Player_Pos", L"HPRatio", L"MaxHP", L"HP", 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("체력 비율", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	CBT_RotationDir* Attack_After_Chase = Node_RotationDir("추적기능 노드", L"Player_Pos", 3);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdatePlayerPosService);
	Start_Sel->Add_Service(UpdatePlayerHPservice);
	Start_Sel->Add_Service(UpdateHPRatioService);

	Start_Sel->Add_Child(Bite_LeftRightLeft());
	Start_Sel->Add_Child(Attack_After_Chase);

	return NOERROR;
}

_int CBlackWolf::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	// 뼈 위치 업데이트
	Update_Bone_Of_BlackBoard();
	// BB 직접 업데이트
	Update_Value_Of_BB();

	return NOERROR;
}

_int CBlackWolf::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NOERROR;
}

HRESULT CBlackWolf::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_pShaderCom->Begin_Pass(0);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CBlackWolf::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_BlackWolf", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CBlackWolf::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);

	CManagement*		pManagement = CManagement::Get_Instance();
	IF_NULL_VALUE_RETURN(pManagement, E_FAIL);

	Safe_AddRef(pManagement);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CBlackWolf::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head_Jaw");
	m_vHeadTeeth = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Head_Jaw", m_vHeadTeeth);

	return S_OK;
}

HRESULT CBlackWolf::Update_Value_Of_BB()
{
	_v3 vSelfDir = *(_v3*)&m_pTransformCom->Get_WorldMat().m[2];

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir0", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir1", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(12.5f))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir2", vSelfDir);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir3", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-12.5f))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir4", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25))));

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());

	return S_OK;
}

CBT_Composite_Node * CBlackWolf::Frisbee()
{
	CBT_Simple_Parallel* FrisbeeParallel = Node_Parallel_Immediate("프리스비 병렬");

	CBT_Sequence* MainSq = Node_Sequence("공격");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("프리스비", 30, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.3f);

	CBT_Sequence* SubSq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("FrisbeeWait0", 0.7, 0); //점프 시작
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("Frisbee 이동될 값", 20, 0.6, 0); //낙하까지 진행

	FrisbeeParallel->Set_Main_Child(MainSq);
	MainSq->Add_Child(Show_Ani30);
	MainSq->Add_Child(Show_Ani15);

	FrisbeeParallel->Set_Sub_Child(SubSq);
	SubSq->Add_Child(Wait0);
	SubSq->Add_Child(Move0);

	//원하는 방향
	//치아에 맞춰서 0.5f~0.8f 트레일
	//충돌 했을 경우 번쩍임, 왜곡

	return FrisbeeParallel;
}

CBT_Composite_Node * CBlackWolf::JumpDodgeL_And_Bite()
{
	CBT_Simple_Parallel* Parallel = Node_Parallel_Immediate("회피 후 물기 병렬");

	CBT_Sequence* MainSq = Node_Sequence("공격");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("좌회피 후 물기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.3f);

	CBT_Sequence* SubSq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("Wait1", 0.2, 0); //0.2 ~ 0.6 회피로 이동될 값
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("좌회피 이동", 13, 0.4, 0);
	CBT_Wait* Wait1 = Node_Wait("Wait2", 0.15, 0); //0.75 ~ 1.3 또는 1.23 물기로 이동될 값
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("물기 이동", 13, 0.55, 0);

	Parallel->Set_Main_Child(MainSq);
	MainSq->Add_Child(Show_Ani29);
	MainSq->Add_Child(Show_Ani15);

	Parallel->Set_Sub_Child(SubSq);
	SubSq->Add_Child(Wait0);
	SubSq->Add_Child(Move0);
	SubSq->Add_Child(Wait1);
	SubSq->Add_Child(Move1);

	return Parallel;
}

CBT_Composite_Node * CBlackWolf::JumpDodgeR_And_Bite()
{
	CBT_Simple_Parallel* Parallel = Node_Parallel_Immediate("회피 후 물기 병렬");

	CBT_Sequence* MainSq = Node_Sequence("공격");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("우회피 후 물기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.3f);

	CBT_Sequence* SubSq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("Wait1", 0.2, 0); //0.2 ~ 0.6 회피로 이동될 값
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("우회피 이동", 13, 0.4, 0); //낙하까지 진행
	CBT_Wait* Wait1 = Node_Wait("Wait2", 0.15, 0); //0.75 ~ 1.3 또는 1.23 물기로 이동될 값
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("물기 이동", 13, 0.55, 0); //낙하까지 진행

	Parallel->Set_Main_Child(MainSq);
	MainSq->Add_Child(Show_Ani28);
	MainSq->Add_Child(Show_Ani15);

	Parallel->Set_Sub_Child(SubSq);
	SubSq->Add_Child(Wait0);
	SubSq->Add_Child(Move0);
	SubSq->Add_Child(Wait1);
	SubSq->Add_Child(Move1);

	return Parallel;
}

CBT_Composite_Node * CBlackWolf::Bite_LeftRightLeft()
{
	CBT_Simple_Parallel* Parallel = Node_Parallel_Immediate("회피 후 물기 병렬");

	CBT_Sequence* MainSq = Node_Sequence("공격");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("좌 우 좌", 27, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.3f);

	CBT_Sequence* SubSq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("Wait1", 0.8, 0); //0.8 ~ 1.2 좌
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("좌 이동", 13, 0.4, 0);
	CBT_Wait* Wait1 = Node_Wait("Wait2", 0.4, 0); //1.6 ~ 1.8 우
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("우 이동", 15, 0.2, 0);
	CBT_Wait* Wait2 = Node_Wait("Wait1", 0.3, 0); //2.1 ~ 2.3 좌
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("우 이동", 10, 0.2, 0);

	Parallel->Set_Main_Child(MainSq);
	MainSq->Add_Child(Show_Ani27);
	MainSq->Add_Child(Show_Ani15);

	Parallel->Set_Sub_Child(SubSq);
	SubSq->Add_Child(Wait0);
	SubSq->Add_Child(Move0);
	SubSq->Add_Child(Wait1);
	SubSq->Add_Child(Move1);
	SubSq->Add_Child(Wait2);
	SubSq->Add_Child(Move2);

	return Parallel;
}

CBlackWolf * CBlackWolf::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlackWolf* pInstance = new CBlackWolf(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlackWolf::Clone_GameObject(void * pArg)
{
	CBlackWolf* pInstance = new CBlackWolf(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackWolf::Free()
{
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
