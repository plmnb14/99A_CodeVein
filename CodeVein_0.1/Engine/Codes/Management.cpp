#include "..\Headers\Management.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pScene_Manager(CScene_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pGizmo(CGizmo::Get_Instance())
	, m_pBT_Node_Manager(CBT_Node_Manager::Get_Instance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pScene_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pGizmo);
	Safe_AddRef(m_pBT_Node_Manager);
}

HRESULT CManagement::Ready_Engine(_uint iNumScenes)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Container_Size(iNumScenes)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container_Size(iNumScenes)))
		return E_FAIL;

	return S_OK;
}

_int CManagement::Update_Management(_double TimeDelta)
{
	if (nullptr == m_pScene_Manager ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pInput_Device)
		return -1;

	if (FAILED(m_pInput_Device->Set_InputDev()))
		return -1;

	_int iProgress = 0;

	iProgress = m_pObject_Manager->Update_Object_Manager(TimeDelta);
	if (0 > iProgress)
		return iProgress;

	iProgress = m_pPipeLine->Update_PipeLine();
	if (0 > iProgress)
		return iProgress;

	iProgress = m_pObject_Manager->Late_Update_Object_Manager(TimeDelta);

	if (0 > iProgress)
		return iProgress;

	return m_pScene_Manager->Update_Scene(TimeDelta);
}

HRESULT CManagement::Render_Management()
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	if (m_pNavMesh != nullptr)
	{
		m_pNavMesh->Render_NaviMesh();
	}

	return m_pScene_Manager->Render_Scene();
}

HRESULT CManagement::Clear_Instance(_uint iSceneIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Clear_Instance(iSceneIndex)))
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Clear_Instance(iSceneIndex)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CManagement::Release_Engine()
{
	// 최종적으로 엔진에서 사용하고 있는 다양한 매니져클래스들의 정리작업을 수행한다.
	if (0 != CManagement::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CManagement");

	if (0 != CBT_Node_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CBT_Node_Manager");

	if (0 != CObject_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CObject_Manager");

	if (0 != CComponent_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CComponent_Manager");

	if (0 != CScene_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CScene_Manager");

	if (0 != CPipeLine::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CPipeLine");

	if (0 != CLight_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CLight_Manager");

	if (0 != CInput_Device::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CInput_Device");

	if (0 != CTarget_Manager::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CTarget_Manager");

	if (0 != CGizmo::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CGizmo");

	if (0 != CGraphic_Device::Get_Instance()->Destroy_Instance())
		MSG_BOX("Failed To Release CGraphic_Device");



	return S_OK;
}

HRESULT CManagement::Ready_GraphicDev(HWND hWnd, CGraphic_Device::WINMODE eMode, const _uint & iSizeX, const _uint & iSizeY, LPDIRECT3DDEVICE9 * ppGraphicDev)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Ready_GraphicDev(hWnd, eMode, iSizeX, iSizeY, ppGraphicDev);
}

_byte CManagement::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CManagement::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouse)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CManagement::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseState)
{
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

HRESULT CManagement::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	return m_pInput_Device->Ready_Input_Dev(hInst, hWnd);
}

HRESULT CManagement::Set_InputDev(void)
{
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	return m_pInput_Device->Set_InputDev();
}


HRESULT CManagement::SetUp_CurrentScene(CScene * pScene)
{
	if (nullptr == m_pScene_Manager)
		return E_FAIL;

	return m_pScene_Manager->SetUp_CurrentScene(pScene);
}

HRESULT CManagement::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CManagement::Add_Layer(_uint iSceneID, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Layer(iSceneID, pLayerTag);
}

HRESULT CManagement::Add_GameObject_ToLayer(const _tchar * pPrototypeTag, _uint iSceneID, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject_ToLayer(pPrototypeTag, iSceneID, pLayerTag, pArg);
}

CGameObject* CManagement::Get_GameObjectBack(const _tchar* pLayerTag, _uint iSceneID)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObjectBack(pLayerTag, iSceneID);
}

list<CGameObject*> CManagement::Get_GameObjectList(const _tchar * pLayerTag, _uint iSceneID)
{
	return m_pObject_Manager->Get_GameObjectList(pLayerTag, iSceneID);
}

CGameObject * CManagement::Clone_GameObject_Return(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject_Return(pPrototypeTag, pArg);
}

HRESULT CManagement::Add_GameOject_ToLayer_NoClone(CGameObject * _pGameObject, _uint iSceneID, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameOject_ToLayer_NoClone(_pGameObject, iSceneID, pLayerTag, pArg);
}

HRESULT CManagement::LoadCreateObject_FromPath(_Device _pGraphicDev, const _tchar * _FilePath)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->LoadObjectPrototypes_FromPath(_pGraphicDev, _FilePath);
}

HRESULT CManagement::Ready_Component_Manager(_Device _pGraphicDev)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Ready_Component_Manager(_pGraphicDev);
}

HRESULT CManagement::Add_Prototype(_uint iSceneID, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iSceneID, pPrototypeTag, pPrototype);
}

CComponent * CManagement::Clone_Component(_uint iSceneID, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iSceneID, pPrototypeTag, pArg);
}

HRESULT CManagement::LoadMesh_FromPath(_Device pGraphicDev, const _tchar * szImgPath)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->LoadMesh_FilesFromPath(pGraphicDev, szImgPath);
}

HRESULT CManagement::LoadTex_FromPath(_Device pGraphicDev, const _tchar * szImgPath)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;


	return m_pComponent_Manager->LoadTex_FilesFromPath(pGraphicDev, szImgPath);
}

void CManagement::Clone_NavMesh_OnManagement()
{
	if (nullptr == m_pComponent_Manager)
		return;

	m_pNavMesh = static_cast<CNavMesh*>(m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"NavMesh", nullptr));
}

void CManagement::LoadNavMesh_FromFile(_Device _pGraphicDev, const _tchar * szFile)
{
	IF_NOT_NULL(m_pNavMesh)
		return;

	m_pNavMesh->Ready_NaviMesh(_pGraphicDev, szFile);
}

_v3 CManagement::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _v3();

	return _v3(m_pPipeLine->Get_CamPosition());
}

_mat CManagement::Get_Transform(D3DTRANSFORMSTATETYPE eState)
{
	if (nullptr == m_pPipeLine)
		return _mat();

	return _mat(m_pPipeLine->Get_Transform(eState));
}

HRESULT CManagement::Set_Transform(D3DTRANSFORMSTATETYPE eState, _mat StateMatrix)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return m_pPipeLine->Set_Transform(eState, StateMatrix);
}

const D3DLIGHT9 * CManagement::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

const _mat CManagement::Get_LightViewProj(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return _mat();

	return m_pLight_Manager->Get_LightViewProj(iIndex);
}

void CManagement::Set_LightPos(_uint iIndex, _v3 vPos)
{
	if (nullptr == m_pLight_Manager)
		return;

	return m_pLight_Manager->Set_Pos(iIndex, vPos);
}

HRESULT CManagement::Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pGraphic_Device, LightDesc);
}

LPDIRECT3DTEXTURE9 CManagement::Get_Target_Texture(const _tchar * pTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_Texture(pTargetTag);
}

void CManagement::Ready_Gizmo(_Device _pGraphicDev)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Ready_Gizmo(_pGraphicDev);
}

void CManagement::Gizmo_Draw_XYZ(const _v3 _vPos, const _v3 _vLook, const _v3 _vRight, const _v3 _vUp)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_XYZ(_vPos, _vLook, _vRight, _vUp);
}

void CManagement::Gizmo_Draw_Vertex(const _v3 _vVertex, const _v3 _vSize)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_Vertex(_vVertex, _vSize);
}

void CManagement::Gizmo_Draw_Sphere(const _v3 _vVertex, const _float _fRadius)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_Sphere(_vVertex, _fRadius);
}

void CManagement::Gizmo_Draw_Triangle_Line(_v3 * _vVertex, const _bool _bSelect, CELL_PARAM _eParam)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_Triangle_Line(_vVertex, _bSelect, _eParam);
}

void CManagement::Gizmo_Draw_Triangle(VTX_COL * _vVertex)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_Triangle(_vVertex);
}

void CManagement::Gizmo_Draw_Triangle(VTX_COL * _vVertex , D3DXCOLOR _vColor)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_Triangle(_vVertex  , _vColor);
}

void CManagement::Gizmo_Draw_AABB(_v3 * _vVertex, const _v3 _vPos, const _v3 _vSize)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_AABB(_vVertex, _vPos, _vSize);
}

void CManagement::Gizmo_Draw_OBB(_v3 * _vVertex, const _v3 vRotate, const _v3 _vPos, const _v3 _vSize)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_OBB(_vVertex, vRotate, _vPos, _vSize);
}

void CManagement::Gizmo_Draw_Capsule(_v3 _vVertex, const _v3 _vRadius)
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Draw_Capsule(_vVertex, _vRadius);
}

void CManagement::Gizmo_ColliderEnable()
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Enable_GizmoCollider();
}

void CManagement::Gizmo_CellEnable()
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Enable_GizmoCell();
}

void CManagement::Gizmo_Toggle()
{
	if (m_pGizmo == nullptr)
		return;

	m_pGizmo->Gizmo_Toggle();
}

HRESULT CManagement::Ready_BT_Node()
{
	return m_pBT_Node_Manager->Ready_BT_Node();
}

CBT_Node * CManagement::Clone_Node(const _tchar * pPrototypeTag, CBT_Node_Manager::NODE_TYPE eType, void * pInit_Struct)
{
	return m_pBT_Node_Manager->Clone_Node(pPrototypeTag, eType, pInit_Struct);
}

void CManagement::Create_ParticleEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_ParticleEffect(szName, fLifeTime, vPos, pFollowTrans);
}

void CManagement::Create_Effect(_tchar* szName, _v3 vPos, CTransform * pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_Effect(szName, vPos, pFollowTrans);
}

void CManagement::Create_DirEffect(_tchar * szName, _v3 vPos, _v3 vDir, CTransform * pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_DirEffect(szName, vPos, vDir, pFollowTrans);
}

void CManagement::Create_AngleEffect(_tchar * szName, _v3 vPos, _v3 vAngle, CTransform * pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_AngleEffect(szName, vPos, vAngle, pFollowTrans);
}

void CManagement::Create_AutoFindEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans, _v3 vPos)
{
	CParticleMgr::Get_Instance()->Create_AutoFindEffect(szName, fLifeTime, vPos, pFollowTrans);
}

void CManagement::Create_Hit_Effect(CCollider* pAttackCol, CCollider* pHittedCol, CTransform* pHittedTrans, _float fPower)
{
	CParticleMgr::Get_Instance()->Create_Hit_Effect(pAttackCol, pHittedCol, pHittedTrans, fPower);
}

void CManagement::Create_Spawn_Effect(_v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_Spawn_Effect(vPos, vFinishPos, pFollowTrans);
}

void CManagement::Create_Effect_Offset(_tchar* szName, _float fOffset, _v3 vPos, CTransform * pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_Effect_Offset(szName, fOffset, vPos, pFollowTrans);
}

void CManagement::Create_Effect_Delay(_tchar * szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _v3 vAngle)
{
	CParticleMgr::Get_Instance()->Create_Effect_Delay(szName, fDelay, vPos, pFollowTrans, vAngle);
}

void CManagement::Create_ParticleEffect_Delay(_tchar * szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform * pFollowTrans)
{
	CParticleMgr::Get_Instance()->Create_ParticleEffect_Delay(szName, fLifeTime, fDelay, vPos, pFollowTrans);
}

void CManagement::Free()
{
	Safe_Release(m_pBT_Node_Manager);
	Safe_Release(m_pGizmo);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pScene_Manager);
	Safe_Release(m_pGraphic_Device);
}
