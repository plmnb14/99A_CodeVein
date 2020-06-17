#include "..\Headers\ActiveObject.h"
#include "Management.h"

static _bool bTestObject = false;

CActiveObject::CActiveObject(_Device _pGraphicDev)
	: CGameObject(_pGraphicDev)
{
}

CActiveObject::CActiveObject(const CActiveObject & rhs)
	: CGameObject(rhs)
{
}

CActiveObject::~CActiveObject()
{
}

_int CActiveObject::Update_GameObject(_double _TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(_TimeDelta);

	CManagement* pManagement = CManagement::Get_Instance();

	switch (m_iAtvNumber)
	{
	case ATV_MISTLETOE:
	{
		// �ܿ���� ����
		// �÷��̾�� �ܿ������ �Ÿ��� �����ؼ� ������ ���� EŰ�� ������
		// ��ȣ�ۿ��� �Ѵ�.
		// �ܿ���� Ȱ��ȭ �� ���� �����Ǿ�� ��
		//if (nullptr != pManagement)
		//{
		//	_v3 Test = V3_NULL;
		//	Test = TARGET_TO_TRANS(pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();
		//	_v3 TestVec3 = m_pTransform->Get_Pos() - Test;
		//	if (V3_LENGTH(&TestVec3) <= 2.f && false == m_bCheck_Mistletoe)
		//	{
		//		//	cout << "��ȣ�ۿ� ����" << endl;
		//	}
		//	if (pManagement->Get_DIKeyState(DIK_E))
		//	{
		//		if (m_bCheck_Mistletoe == false)
		//			cout << "������ Ȱ��ȭ" << endl;

		//		m_bCheck_Mistletoe = true;
		//	}

		// Effect
		_v3 vEffPos = m_pTransform->Get_Pos();
		vEffPos.y += 0.3f; // �Ѹ�
		pManagement->Create_Effect_Offset(L"MistletoeParticle_Sub", 0.1f, vEffPos);
		vEffPos.y += 0.3f; // ��
		pManagement->Create_Effect_Offset(L"MistletoeParticle", 0.1f, vEffPos);

		//}
		break;
	}
	case ATV_DOOR:
	{
		// �÷��̾�� ���� �浹���� �� EŰ�� ������ ���� ������ �ִϸ��̼��� ������,
		// ���� ������ ��쿡�� ������ �� �ְ� �ؾ� ��
		// ���� ���� �� ��� ������ �� �ְ�
		break;
	}
	case ATV_BOX:
	{
		// �÷��̾ ���ڿ� �浹 �� Ư�� Ű�� ������ �ڽ��� �����鼭 �������� ���´�.
		// ������, ���ڿ� ��ȣ�ۿ��� �Ѵ�.
		// �ڽ� �Ѳ��� ���� ������ �Ǿ��� �� �ڽ� ���� �������� EŰ�� ������ ȹ���� �� �ְ� �Ѵ�
		// �ڽ� �Ѳ��� �Ұ��༭ true �� �� �������� ����ϰ� ��� �� ȹ�� �� ������ ��� ����
		if (true == bTestObject)
		{
			if (false == m_bCheck_Boxopen_end)
				//cout << "������ ȹ�� ����" << endl;
			if (pManagement->Get_DIKeyState(DIK_E) && false == m_bCheck_Boxopen_end)
			{
				//cout << "������ ��" << endl;
				m_bCheck_Boxopen_end = true;
			}

		}
		else
			//cout << "������ ȹ�� �Ұ���" << endl;

		break;
	}
	case ATV_BOX_LID:
	{
		// �÷��̾ �ڽ� �տ��� EŰ�� ������ �� �ڽ��� �Ѳ��� ���� �ӵ��� 45��? ���� ������.
		float TestAngle = m_pTransform->Get_Angle(AXIS_X);

		_v3 Test = V3_NULL;
		Test = TARGET_TO_TRANS(pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();
		_v3 TestVec3 = m_pTransform->Get_Pos() - Test;

		if (V3_LENGTH(&TestVec3) <= 5.f && true == m_bCheck_BoxLid)
			m_bCheck_Boxopen = true;
		if (pManagement->Get_DIKeyState(DIK_E))
			m_bCheck_BoxLid = true;
		if (true == m_bCheck_Boxopen && 1.0f >= m_pTransform->Get_Angle(AXIS_X))
			m_pTransform->Add_Angle(AXIS_X, 1.f);
		else if (D3DXToRadian(1.f) <= m_pTransform->Get_Angle(AXIS_X))
			bTestObject = true;

		break;
	}
	}

	return S_OK;
}

_int CActiveObject::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (false == m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), 2.f))
		return NO_EVENT;

	m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);
	m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this);
	m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this);

	return NO_EVENT;
}

HRESULT CActiveObject::Render_GameObject()
{
	_mat matveiwView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat matPro = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	m_pShader->Set_Value("g_matView", &matveiwView, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", &matPro, sizeof(_mat));

	Init_Shader(m_pShader);

	m_pShader->Begin_Shader();

	_ulong dwNum = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNum; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	return S_OK;
}

HRESULT CActiveObject::Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur)
{
	//============================================================================================
	// ���� ����
	//============================================================================================
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// ��� �� ���
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
		_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_float fBloomPower = 0.5f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// ���̴� ����
	//============================================================================================

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================

	return NOERROR;
}

HRESULT CActiveObject::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Init_Shader(pShader);

	_ulong dwNum = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNum; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		pShader->Begin_Pass(m_iPass);

		pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	return S_OK;
}


void CActiveObject::Chaning_AtvMesh(const _tchar* _MeshName)
{
	if (!lstrcmp(m_szAciveName, _MeshName))
		return;

	lstrcpy(m_szAciveName, _MeshName);

	auto& iter = m_pmapComponents.find(L"Com_StaticMesh");

	Safe_Release(m_pMesh_Static);
	Safe_Release(iter->second);

	iter->second = m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_szAciveName));
	Safe_AddRef(iter->second);
	//iter->second = m_pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_szAciveName));

	return;
}

HRESULT CActiveObject::Default_Setting()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	//===================================================================
	// ������Ʈ ���� ����	
	//===================================================================
	m_pBattleAgent->Set_RimAlpha(0.5f);
	m_pBattleAgent->Set_RimValue(8.f);
	m_pBattleAgent->Set_OriginRimAlpha(0.5f);
	m_pBattleAgent->Set_OriginRimValue(8.f);
	//===================================================================

	return S_OK;
}

HRESULT CActiveObject::Ready_GameObject(void * pArg)
{
	ACTOBJ_INFO Info = *(ACTOBJ_INFO*)(pArg);

	// Components
	if (FAILED(Add_Components(Info.szMeshName)))
		return E_FAIL;

	// Argument
	if (FAILED(Setup_Arg(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveObject::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CActiveObject::LateInit_GameObject()
{
	m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(45.f));
	return S_OK;
}

void CActiveObject::Init_Shader(CShader* pShader)
{
	_mat		matWorld, matView, matProj;

	matWorld = m_pTransform->Get_WorldMat();

	pShader->Set_Value("g_matWorld", &matWorld, sizeof(_mat));

	_float	fEmissivePower = 1.f;	// �̹̽ú� : ���� ����, ��ü �߱��� ������.
	_float	fSpecularPower = 2.f;	// ��Ż�Ͻ� : ���� ����, ���ݻ簡 ������.
	_float	fRoughnessPower = 0.5f;	// �����Ͻ� : ���� ����, �� ����� ������(���� ������).
	_float	fRimLightPower = 0.f;	// ��		: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).
	_float	fMinSpecular = 1.f;	// �ּ� ��	: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).
	_float	fID_R = 1.0f;	// ID_R : Rä�� ID �� , 1�� �ִ�
	_float	fID_G = 0.5f;	// ID_G : Gä�� ID �� , 1�� �ִ�
	_float	fID_B = 0.1f;	// ID_B	: Bä�� ID �� , 1�� �ִ�

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return;
}

HRESULT CActiveObject::Add_Components(_tchar* _meshName)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, _meshName, L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	// for.Com_Optimization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Opimaization", (CComponent**)& m_pOptimization)))
		return E_FAIL;

	lstrcpy(m_szAciveName, _meshName);

	return S_OK;
}

HRESULT CActiveObject::Setup_Arg(void * pArg)
{
	if (nullptr != pArg)
	{
		ACTOBJ_INFO Info = *(ACTOBJ_INFO*)(pArg);

		m_iAtvNumber = Info.iActiveIndex;
		m_iIndex = Info.iObjIndex;

		m_pTransform->Set_Pos(Info.vPos);
		m_pTransform->Set_Angle(Info.vAngle);
		m_pTransform->Set_Scale(Info.vScale);
	}

	return S_OK;
}

CActiveObject* CActiveObject::Create(_Device _pGraphicDev)
{
	CActiveObject* pInstance = new CActiveObject(_pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActiveObject::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pBattleAgent);
	Safe_Release(m_pOptimization);

	CGameObject::Free();
}

CGameObject* CActiveObject::Clone_GameObject(void * pArg)
{
	CActiveObject* pInstance = new CActiveObject(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}
