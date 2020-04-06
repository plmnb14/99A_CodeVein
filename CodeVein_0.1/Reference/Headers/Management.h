#pragma once

// 1. 엔진을 초기화ㅏ한.다.
// 2. 엔진의 사용을 멈추낟(해제한다.)
// 3. 엔진의 기능을 이용할 수 있게 해준다.

#include "Base.h"
#include "Scene_Manager.h"
#include "Object_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "Target_Manager.h"
#include "Gizmo.h"
#include "BT_Node_Mgr.h"
#include "Timer_Manager.h"
#include "FrameMgr.h"
#include "ParticleMgr.h"
#include "Renderer.h"

BEGIN(Engine)

class ENGINE_DLL CManagement final : public CBase
{
	DECLARE_SINGLETON(CManagement)
private:
	explicit CManagement();
	virtual ~CManagement() = default;
public:
	HRESULT Ready_Engine(_uint iNumScenes);
	_int Update_Management(_double TimeDelta);
	HRESULT Render_Management();
	HRESULT Clear_Instance(_uint iSceneIndex);
	static HRESULT Release_Engine();

public: // For.Graphic_Device
	HRESULT	Ready_GraphicDev(HWND hWnd, CGraphic_Device::WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, LPDIRECT3DDEVICE9* ppGraphicDev = nullptr);


public: // for.Input_Device
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseState);
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	HRESULT Set_InputDev(void);

public: // for.Scene_Manager
	HRESULT SetUp_CurrentScene(CScene* pScene);

public: // For.Object_Manager
	HRESULT				Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT				Add_Layer(_uint iSceneID, const _tchar* pLayerTag);
	HRESULT				Add_GameObject_ToLayer(const _tchar* pPrototypeTag, _uint iSceneID, const _tchar* pLayerTag, void* pArg = nullptr);
	CGameObject*		Get_GameObjectBack(const _tchar* pLayerTag, _uint iSceneID);
	list<CGameObject*>	Get_GameObjectList(const _tchar* pLayerTag, _uint iSceneID);
	CGameObject*		Clone_GameObject_Return(const _tchar* pPrototypeTag, void* pArg);
	HRESULT				Add_GameOject_ToLayer_NoClone(CGameObject* _pGameObject, _uint iSceneID, const _tchar* pLayerTag, void* pArg);
	HRESULT				LoadCreateObject_FromPath(_Device _pGraphicDev, const _tchar* _FilePath);

public: // for.Component_Manager
	HRESULT		Ready_Component_Manager(_Device _pGraphicDev);
	HRESULT		Add_Prototype(_uint iSceneID, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iSceneID, const _tchar* pPrototypeTag, void* pArg = nullptr);

	HRESULT LoadMesh_FromPath(_Device pGraphicDev, const _tchar* szImgPath);
	HRESULT LoadTex_FromPath(_Device pGraphicDev, const _tchar* szImgPath);

public:
	void	Clone_NavMesh_OnManagement();
	void	LoadNavMesh_FromFile(_Device _pGraphicDev, const _tchar* szFile);

public: // For.PipeLine
	_v3	Get_CamPosition();
	_mat Get_Transform(D3DTRANSFORMSTATETYPE eState);
	HRESULT Set_Transform(D3DTRANSFORMSTATETYPE eState, _mat StateMatrix);

public: // For.Light_Manager
	const D3DLIGHT9* Get_LightDesc(_uint iIndex = 0);
	const _mat Get_LightViewProj(_uint iIndex = 0);
	void Set_LightPos(_uint iIndex, _v3 vPos);
	HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc);

public: // For.Target_Manager
	LPDIRECT3DTEXTURE9 Get_Target_Texture(const _tchar* pTargetTag);

public: // For.Gizmo
	void Ready_Gizmo(_Device _pGraphicDev);
	void Gizmo_Draw_XYZ(const _v3 _vPos, const _v3 _vLook, const _v3 _vRight, const _v3 _vUp = WORLD_UP);
	void Gizmo_Draw_Vertex(const _v3 _vVertex, const _v3 _vSize = { 0.1f, 0.1f,0.1f });
	void Gizmo_Draw_Sphere(const _v3 _vVertex, const _float _fRadius = 1.f);
	void Gizmo_Draw_Triangle_Line(_v3 * _vVertex, const _bool _bSelect = false, CELL_PARAM _eParam = NORMAL);
	void Gizmo_Draw_Triangle(VTX_COL * _vVertex);
	void Gizmo_Draw_Triangle(VTX_COL * _vVertex , D3DXCOLOR _vColor);
	void Gizmo_Draw_AABB(_v3* _vVertex, const _v3 _vPos, const _v3 _vSize = _v3{ 1.f, 1.f, 1.f });
	void Gizmo_Draw_OBB(_v3* _vVertex, const _v3 vRotate, const _v3 _vPos, const _v3 _vSize = _v3{ 1.f, 1.f, 1.f });
	void Gizmo_Draw_Capsule(_v3 _vVertex, const _v3 _vRadius);

	void Gizmo_ColliderEnable();
	void Gizmo_CellEnable();
	void Gizmo_Toggle();

public: // For.BT_Node_Manager
	HRESULT Ready_BT_Node();
	CBT_Node* Clone_Node(const _tchar* pPrototypeTag, CBT_Node_Manager::NODE_TYPE eType, void* pInit_Struct);

public: // For.ParticleManager
	void Create_ParticleEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect(_tchar* szName, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_DirEffect(_tchar* szName, _v3 vPos, _v3 vDir, CTransform* pFollowTrans = nullptr);
	void Create_AngleEffect(_tchar* szName, _v3 vPos, _v3 vAngle, CTransform* pFollowTrans = nullptr);
	void Create_AutoFindEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans, _v3 vPos = V3_NULL);
	void Create_Hit_Effect(CCollider* pAttackCol, CCollider* pHittedCol, CTransform* pHittedTrans, _float fPower = 1.5f);
	void Create_Spawn_Effect(_v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_FootSmoke_Effect(_v3 vPos, _float fOffset);

private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CScene_Manager*				m_pScene_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CGizmo*						m_pGizmo = nullptr;
	CBT_Node_Manager*			m_pBT_Node_Manager = nullptr;
	CNavMesh*					m_pNavMesh = nullptr;

public:
	virtual void Free();

};

END