#include "..\Headers\ActeiveObject.h"
#include "Management.h"

CActiveObject::CActiveObject(_Device _pGraphicDev)
	: CGameObject(_pGraphicDev)
	, m_pGrouop(RENDER_ALPHA)
	, m_iIndex(0)
{
}

CActiveObject::CActiveObject(const CActiveObject & rhs)
	: CGameObject(rhs.m_pGraphic_Dev)
	, m_iIndex(rhs.m_iIndex)
	, m_pGrouop(rhs.m_pGrouop)
{
	memcpy(m_szAciveName, rhs.m_szAciveName, sizeof(_tchar[MAX_STR]));

	Default_Setting();
	m_pTransform->Set_Info(rhs.m_pTransform->Get_Info());
}

CActiveObject::~CActiveObject()
{
}

_int CActiveObject::Update_GameObject(_double _TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(_TimeDelta);
	m_pRenderer->Add_RenderList(RENDER_ALPHA, this);
	// ��� ������ ���� 1ȸ�� �����ϸ� �� ���ķ� �ִϸ��̼��� idle ���·� �����Ǿ� �־�� ��
	CManagement* pManagement = CManagement::Get_Instance();

	switch (m_iAtvNumber)
	{
	case ATV_MISTLETOE:
	{
		// �ܿ���� ����
		// �÷��̾�� �ܿ������ �Ÿ��� �����ؼ� ������ ���� EŰ�� ������
		// ��ȣ�ۿ��� �Ѵ�.
		// �ܿ���� Ȱ��ȭ �� ���� �����Ǿ�� ��
		if (nullptr != pManagement)
		{
			_v3 Test = V3_NULL;
			Test =TARGET_TO_TRANS(pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();
			_v3 TestVec3 = m_pTransform->Get_Pos() - Test;
			if (V3_LENGTH(&TestVec3) <= 2.f && false == m_bCheck_Mistletoe)
			{
				//cout << "��ȣ�ۿ� ����" << endl;
			}
			if (pManagement->Get_DIKeyState(DIK_E))
				m_bCheck_Mistletoe = true;
		}
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
		/*if (true == m_bCheck_Boxopen_end)
		{
			cout << "������ ���" << endl;
			if (pManagement->Get_DIKeyState(DIK_E))
				cout << "������ ��" << endl;
		}*/
			
		break;
	}
	case ATV_BOX_LID:
	{
		// �÷��̾ �ڽ� �տ��� EŰ�� ������ �� �ڽ��� �Ѳ��� ���� �ӵ��� 45��? ���� ������.
		/*cout << m_pTransform->Get_Angle(AXIS_X) << endl;
		_v3 Test = V3_NULL;
		Test = TARGET_TO_TRANS(pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();
		_v3 TestVec3 = m_pTransform->Get_Pos() - Test;

		if (V3_LENGTH(&TestVec3) <= 2.f && true == m_bCheck_BoxLid)
			m_bCheck_Boxopen = true;
		if (pManagement->Get_DIKeyState(DIK_E))
			m_bCheck_BoxLid = true;
		if (true == m_bCheck_Boxopen && 45.f >= m_pTransform->Get_Angle(AXIS_X))
			m_pTransform->Add_Angle(AXIS_X, 10.f);
		else if (45.f <= m_pTransform->Get_Angle(AXIS_X))
			m_bCheck_Boxopen_end = true;
		*/
		break;
	}
	}

	return S_OK;
}

_int CActiveObject::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CActiveObject::Render_GameObject()
{
	Init_Shader();

	m_pShader->Begin_Shader();

	_ulong dwNum = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNum; ++i)
	{
		m_pShader->Begin_Pass(m_PassNum);

		if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	return S_OK;
}

void CActiveObject::Chaning_AtvMesh(const _tchar* _MeshName)
{
	if (!lstrcmp(m_szAciveName, _MeshName))
		return;

	lstrcpy(m_szAciveName, _MeshName);

	auto& iter = m_pmapComponents.find(L"Mesh");

	Safe_Release(m_pMesh_Static);
	Safe_Release(iter->second);
	
	iter->second = m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_szAciveName));
	Safe_AddRef(iter->second);
	//iter->second = m_pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_szAciveName));

	return;
}

HRESULT CActiveObject::Add_Essentional_Copy()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_szAciveName, L"Mesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActiveObject::Default_Setting()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

HRESULT CActiveObject::Initialize()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DefaultBox", L"Mesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	lstrcpy(m_szAciveName, L"Mesh_DefaultBox");

	return S_OK;
}

HRESULT CActiveObject::LateInit_GameObject()
{
	return S_OK;
}

void CActiveObject::Init_Shader()
{
	_mat		matWorld, matView, matProj;

	matWorld = m_pTransform->Get_WorldMat();
	m_pGraphic_Dev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Dev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pShader->Set_Value("g_matWorld", &matWorld, sizeof(_mat));
	m_pShader->Set_Value("g_matView", &matView, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", &matProj, sizeof(_mat));
}

CActiveObject* CActiveObject::Create(_Device _pGraphicDev, _int iActeivaNumber)
{
	CActiveObject* pInstance = new CActiveObject(_pGraphicDev);
	pInstance->m_iAtvNumber = iActeivaNumber;

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CActiveObject* CActiveObject::CreateClone(CActiveObject* _pCopy)
{
	return new CActiveObject(*_pCopy);
}

void CActiveObject::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	CGameObject::Free();
}

CGameObject* CActiveObject::Clone_GameObject(void * pArg)
{
	return nullptr;
}
