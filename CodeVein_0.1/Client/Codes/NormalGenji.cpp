#include "stdafx.h"
#include "..\Headers\NormalGenji.h"

CNormalGenji::CNormalGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CNormalGenji::CNormalGenji(const CNormalGenji & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNormalGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CNormalGenji::Ready_GameObject(void * pArg)
{

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	//pBlackBoard->Set_Value(L"Target_Pos", &_v3(-2.f, -2.f, -2.f), CBlackBoard::VALUE::VEC3);
	//pBlackBoard->Set_Value(L"A", &_bool(true), CBlackBoard::VALUE::BOOL);
	//_v3 temp = pBlackBoard->Get_V3Value(L"Target_Pos");

	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	CManagement* pManagement = CManagement::Get_Instance();


	CBT_Sequence* pSequence = Node_Sequence("Sequence");
	if (FAILED(pBehaviorTree->Set_Child(pSequence))) return E_FAIL;

	//CBT_Sequence* pSequence1 = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, &CBT_Sequence::INFO("Sequence2")));
	//CBT_Sequence* pSequence2 = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, nullptr));

	//CBT_Selector* pSelector = static_cast<CBT_Selector*>(pManagement->Clone_Node(L"Selector", CBT_Node_Manager::COMPOSITE, &CBT_Selector::INFO("Selecotr")));

	//CBT_Wait* pWait2 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait2", 2, 0)));
	//CBT_Wait* CoolWait = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("CoolWait",2)));
	//CBT_Wait* CoolWait22 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait444", 4)));
	//CBT_Move* pMove = static_cast<CBT_Move*>(pManagement->Clone_Node(L"Move", CBT_Node_Manager::TASK, &CBT_Move::INFO(m_pTransformCom, 1, 5)));

	//공격 패턴

	//// 29 -> 오발탄
	//CBT_Play_Ani* pAni29 = Node_Ani("5발쏘기", 29, 0.9f);
	//pSequence->Add_Child(pAni29);

	//방패 벗겨짐
	CBT_Play_Ani* pAni0 = Node_Ani("방패 벗겨짐 ", 0, 0.9f);
	pSequence->Add_Child(pAni0);

	//방패 풀림
	CBT_Play_Ani* pAni1 = Node_Ani("방패 풀림 ", 1, 0.9f);
	pSequence->Add_Child(pAni1);

	//방패 틱틱
	CBT_Play_Ani* pAni2 = Node_Ani("방패 틱틱 ", 2, 0.9f);
	pSequence->Add_Child(pAni2);

	//가드 시작
	CBT_Play_Ani* pAni3 = Node_Ani("가드 시작 ", 3, 0.9f);
	pSequence->Add_Child(pAni3);

	//가드 루프
	CBT_Play_Ani* pAni4 = Node_Ani("가드 루프 ", 4, 0.9f);
	pSequence->Add_Child(pAni4);

	//가드 끝
	CBT_Play_Ani* pAni5 = Node_Ani("가드 끝 ", 5, 0.9f);
	pSequence->Add_Child(pAni5);

	//가드 달리기
	CBT_Play_Ani* pAni6 = Node_Ani("가드 달리기 ", 6, 0.9f);
	pSequence->Add_Child(pAni6);

	//가드 오른쪽 걷기
	CBT_Play_Ani* pAni7 = Node_Ani("가드 뒤로 걷기 ", 7, 0.9f);
	pSequence->Add_Child(pAni7);

	//가드 왼쪽 걷기
	CBT_Play_Ani* pAni8 = Node_Ani("가드 왼쪽 걷기 ", 8, 0.9f);
	pSequence->Add_Child(pAni8);

	//가드 앞오른쪾 걷기
	CBT_Play_Ani* pAni9 = Node_Ani("가드 앞오른쪾 걷기 ", 9, 0.9f);
	pSequence->Add_Child(pAni9);

	//가드 앞왼쪽오른발 걷기
	CBT_Play_Ani* pAni10 = Node_Ani("가드 앞왼쪽오른발 걷기 ", 10, 0.9f);
	pSequence->Add_Child(pAni10);

	//가드 앞왼쪽왼발 걷기
	CBT_Play_Ani* pAni11 = Node_Ani("가드 앞왼쪽왼발 걷기 ", 11, 0.9f);
	pSequence->Add_Child(pAni11);

	//가드 앞으로 걷기
	CBT_Play_Ani* pAni12 = Node_Ani("가드 앞으로 걷기 ", 12, 0.9f);
	pSequence->Add_Child(pAni12);

	//가드 뒤오른쪽오른발 걷기
	CBT_Play_Ani* pAni13 = Node_Ani("가드 뒤오른쪽오른발 걷기 ", 13, 0.9f);
	pSequence->Add_Child(pAni13);

	//가드 뒤 오른쪽왼발 걷기
	CBT_Play_Ani* pAni14 = Node_Ani("가드 뒤 오른쪽왼발 걷기 ", 14, 0.9f);
	pSequence->Add_Child(pAni14);

	//가드 뒤 왼쪽 걷기
	CBT_Play_Ani* pAni15 = Node_Ani("가드 뒤 왼쪽 걷기 ", 15, 0.9f);
	pSequence->Add_Child(pAni15);

	//가드 뒤 걷기
	CBT_Play_Ani* pAni16 = Node_Ani("가드 뒤 걷기 ", 16, 0.9f);
	pSequence->Add_Child(pAni16);

	//가드 오른쪽180도
	CBT_Play_Ani* pAni17 = Node_Ani("가드 오른쪽180도 ", 17, 0.9f);
	pSequence->Add_Child(pAni17);

	//가드 오른쪽 90도
	CBT_Play_Ani* pAni18 = Node_Ani("가드 오른쪽 90도 ", 18, 0.9f);
	pSequence->Add_Child(pAni18);

	//가드 왼쪽 180도
	CBT_Play_Ani* pAni19 = Node_Ani("가드 왼쪽 180도 ", 19, 0.9f);
	pSequence->Add_Child(pAni19);

	//가드 왼쪽 90도
	CBT_Play_Ani* pAni20 = Node_Ani("가드 왼쪽 90도 ", 20, 0.9f);
	pSequence->Add_Child(pAni20);

	//막고 지르기
	CBT_Play_Ani* pAni21 = Node_Ani("막고 지르기 ", 21, 0.9f);
	pSequence->Add_Child(pAni21);

	//방패들고 휘돌아치기
	CBT_Play_Ani* pAni22 = Node_Ani("방패들고 휘돌아치기 ", 22, 0.9f);
	pSequence->Add_Child(pAni22);

	//방패로 후려치기
	CBT_Play_Ani* pAni23 = Node_Ani("방패로 후려치기 ", 23, 0.9f);
	pSequence->Add_Child(pAni23);

	//방패들고 지르기
	CBT_Play_Ani* pAni24 = Node_Ani("방패들고 지르기 ", 24, 0.9f);
	pSequence->Add_Child(pAni24);

	//방패로 치고 어퍼치기
	CBT_Play_Ani* pAni25 = Node_Ani("방패로 치고 어퍼치기 ", 25, 0.9f);
	pSequence->Add_Child(pAni25);

	//주머니에서 표창
	CBT_Play_Ani* pAni26 = Node_Ani("주머니에서 표창 ", 26, 0.9f);
	pSequence->Add_Child(pAni26);

	//등에서 표창내던지기
	CBT_Play_Ani* pAni27 = Node_Ani("등에서 표창 내던지기 ", 27, 0.9f);
	pSequence->Add_Child(pAni27);

	//사선후려베기
	CBT_Play_Ani* pAni28 = Node_Ani("사선 후려베기 ", 28, 0.9f);
	pSequence->Add_Child(pAni28);

	//크게 옆구리베기
	CBT_Play_Ani* pAni29 = Node_Ani("크게 옆구리 베기", 29, 0.9f);
	pSequence->Add_Child(pAni29);

	//테니스 토스 두번베기
	CBT_Play_Ani* pAni30 = Node_Ani("테니스 토스 두번베기 ", 30, 0.9f);
	pSequence->Add_Child(pAni30);

	//힘껏 내려배는척하면서 찌르기
	CBT_Play_Ani* pAni31 = Node_Ani("힘껏 내려배는척 하면서 찌르기 ", 31, 0.9f);
	pSequence->Add_Child(pAni31);

	//앉아서 왼쪽옆구리 베기
	CBT_Play_Ani* pAni32 = Node_Ani("앉아서 왼쪽옆구리베기 ", 32, 0.9f);
	pSequence->Add_Child(pAni32);

	//백스텝하며 표창
	CBT_Play_Ani* pAni33 = Node_Ani("백스텝하며 표창 ", 33, 0.9f);
	pSequence->Add_Child(pAni33);

	//달려가 지르기 시작
	CBT_Play_Ani* pAni34 = Node_Ani("달려가 지르기 시작 ", 34, 0.9f);
	pSequence->Add_Child(pAni34);

	//달려가 지르기 루프
	CBT_Play_Ani* pAni35 = Node_Ani("달려가 지르기 루프 ", 35, 0.9f);
	pSequence->Add_Child(pAni35);

	//달려가 지르기 끝
	CBT_Play_Ani* pAni36 = Node_Ani("달려가 지르기 끝 ", 36, 0.9f);
	pSequence->Add_Child(pAni36);

	//테니스 토스 세번베기
	CBT_Play_Ani* pAni37 = Node_Ani("테니스 토스 세번베기 ", 37, 0.9f);
	pSequence->Add_Child(pAni37);

	//휘청거리게 크게 베기
	CBT_Play_Ani* pAni38 = Node_Ani("휘청거리게 크게 베기 ", 38, 0.9f);
	pSequence->Add_Child(pAni38);

	//손날치듯이
	CBT_Play_Ani* pAni39 = Node_Ani("손날치듯이 ", 39, 0.9f);
	pSequence->Add_Child(pAni39);

	//손등으로 뺨치듯이
	CBT_Play_Ani* pAni40 = Node_Ani("손등으로 뺨치듯이 ", 40, 0.9f);
	pSequence->Add_Child(pAni40);

	//미운놈 꿀밤패듯이
	CBT_Play_Ani* pAni41 = Node_Ani("미운놈 꿀밤패듯이", 41, 0.9f);
	pSequence->Add_Child(pAni41);

	//총 기본
	CBT_Play_Ani* pAni42 = Node_Ani("총 기본", 42, 0.9f);
	pSequence->Add_Child(pAni42);

	//개머리판 치기
	CBT_Play_Ani* pAni43 = Node_Ani("개머리판 치기 ", 43, 0.9f);
	pSequence->Add_Child(pAni43);

	//정조준
	CBT_Play_Ani* pAni44 = Node_Ani("정조준 ", 44, 0.9f);
	pSequence->Add_Child(pAni44);

	//팔꿈치 치기
	CBT_Play_Ani* pAni45 = Node_Ani("팔꿈치 치기 ", 45, 0.9f);
	pSequence->Add_Child(pAni45);

	//한손 총 찌르기
	CBT_Play_Ani* pAni46 = Node_Ani("한손 총 찌르기 ", 46, 0.9f);
	pSequence->Add_Child(pAni46);

	//총 왼쪽부터 베기
	CBT_Play_Ani* pAni47 = Node_Ani("총 왼쪽부터 베기 ", 47, 0.9f);
	pSequence->Add_Child(pAni47);

	//백덤블링중 2회 쏘기
	CBT_Play_Ani* pAni48 = Node_Ani("백덤블링중 2회 쏘기 ", 48, 0.9f);
	pSequence->Add_Child(pAni48);

	//총 쏘기
	CBT_Play_Ani* pAni49 = Node_Ani("총 쏘기 ", 49, 0.9f);
	pSequence->Add_Child(pAni49);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	return NOERROR;
}

_int CNormalGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CNormalGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return _int();
}

HRESULT CNormalGenji::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

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

HRESULT CNormalGenji::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_NormalGenji", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;


	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_NormalGenji", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CNormalGenji::SetUp_ConstantTable()
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

	Safe_Release(pManagement);

	return NOERROR;
}

CNormalGenji * CNormalGenji::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNormalGenji* pInstance = new CNormalGenji(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CNormalGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNormalGenji::Clone_GameObject(void * pArg)
{
	// 검, 총, 방패
	// 

	CNormalGenji* pInstance = new CNormalGenji(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CNormalGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNormalGenji::Free()
{
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
