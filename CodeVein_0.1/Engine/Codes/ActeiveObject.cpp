#include "..\Headers\ActeiveObject.h"
#include "Management.h"

static _bool bTestObject = false;

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

	Add_Essentional_Copy();
	Default_Setting();

	m_pTransform->Set_Info(rhs.m_pTransform->Get_Info());

}

CActiveObject::~CActiveObject()
{
}

_int CActiveObject::Update_GameObject(_double _TimeDelta)
{
	return 0;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(_TimeDelta);
	m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);
	// 모든 패턴은 최초 1회만 실행하며 그 이후로 애니메이션이 idle 상태로 고정되어 있어야 함
	CManagement* pManagement = CManagement::Get_Instance();

	switch (m_iAtvNumber)
	{
	case ATV_MISTLETOE:
	{
		// 겨우살이 패턴
		// 플레이어와 겨우살이의 거리를 측정해서 가까이 가서 E키를 누르면
		// 상호작용을 한다.
		// 겨우살이 활성화 시 몬스터 리젠되어야 함
		if (nullptr != pManagement)
		{
			_v3 Test = V3_NULL;
			Test = TARGET_TO_TRANS(pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();
			_v3 TestVec3 = m_pTransform->Get_Pos() - Test;
			if (V3_LENGTH(&TestVec3) <= 2.f && false == m_bCheck_Mistletoe)
			{
				//	cout << "상호작용 가능" << endl;
			}
			if (pManagement->Get_DIKeyState(DIK_E))
			{
				if (m_bCheck_Mistletoe == false)
					cout << "겻우살이 활성화" << endl;

				m_bCheck_Mistletoe = true;
			}

			// Effect
			_v3 vEffPos = m_pTransform->Get_Pos();
			vEffPos.y += 0.3f; // 뿌리
			pManagement->Create_Effect_Offset(L"MistletoeParticle_Sub", 0.1f, vEffPos);
			vEffPos.y += 0.3f; // 꽃
			pManagement->Create_Effect_Offset(L"MistletoeParticle", 0.1f, vEffPos);
		}
		break;
	}
	case ATV_DOOR:
	{
		// 플레이어와 문이 충돌했을 때 E키를 누르면 문이 열리는 애니메이션이 나오고,
		// 문이 열렸을 경우에만 지나갈 수 있게 해야 함
		// 문이 열린 후 계속 지나갈 수 있게
		break;
	}
	case ATV_BOX:
	{
		// 플레이어가 상자와 충돌 후 특정 키를 누르면 박스가 열리면서 아이템이 나온다.
		// 아이템, 상자와 상호작용을 한다.
		// 박스 뚜껑이 일정 각도가 되었을 때 박스 안의 아이템을 E키를 누르면 획득할 수 있게 한다
		// 박스 뚜껑을 불값줘서 true 일 때 아이템을 드랍하고 드랍 후 획득 시 아이템 드랍 금지
		if (true == bTestObject)
		{
			if (false == m_bCheck_Boxopen_end)
				//cout << "아이템 획득 가능" << endl;
			if (pManagement->Get_DIKeyState(DIK_E) && false == m_bCheck_Boxopen_end)
			{
				//cout << "아이템 겟" << endl;
				m_bCheck_Boxopen_end = true;
			}

		}
		else
			//cout << "아이템 획득 불가능" << endl;

		break;
	}
	case ATV_BOX_LID:
	{
		// 플레이어가 박스 앞에서 E키를 눌렀을 때 박스의 뚜껑이 일정 속도로 45도? 정도 돌린다.
		float TestAngle = m_pTransform->Get_Angle(AXIS_X);
		cout << TestAngle << endl;
		_v3 Test = V3_NULL;
		Test = TARGET_TO_TRANS(pManagement->Get_Instance()->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();
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

	auto& iter = m_pmapComponents.find(L"Com_StaticMesh");

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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_szAciveName, L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DefaultBox", L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	lstrcpy(m_szAciveName, L"Mesh_DefaultBox");

	return S_OK;
}

HRESULT CActiveObject::LateInit_GameObject()
{

	m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(45.f));
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
