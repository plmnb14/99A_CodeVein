#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CShader;
class CMesh_Static;
class CCollider;
class CRenderer;

class ENGINE_DLL CRenderObject : public CGameObject
{
protected:
	explicit CRenderObject(_Device _pGraphicDev);
	explicit CRenderObject(const CRenderObject& rhs);
	virtual ~CRenderObject();

public:
	virtual _int Update_GameObject(_double _TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void Update_Collider();

public:
	HRESULT Add_Essentional();
	HRESULT Add_Essentional_Copy();
	HRESULT Default_Setting();

public:
	void Change_Mesh(const _tchar* _MeshName);

public:
	virtual void Set_RenderGroup(RENDERID _eGroup);
	virtual void Set_Index(_int _iIndex) { m_iIndex = _iIndex; }
	virtual void Set_Selected(_bool _bSelected) { m_bIsSelected = _bSelected; }
	virtual void Set_OnTool(_bool _bOnTool) { m_bOnTool = _bOnTool; }

public:
	virtual _tchar*	Get_Name()		{ return m_szName; }
	virtual _int	Get_Index()		{ return m_iIndex; }
	virtual _bool   Get_Selected()	{ return m_bIsSelected; }

protected:
	CTransform*		m_pTransform = nullptr;
	CCollider*		m_pCollider = nullptr;
	CMesh_Static*	m_pMesh_Static = nullptr;
	CRenderer*		m_pRenderer = nullptr;
	CShader*		m_pShader = nullptr;

	// 텍스쳐, 메쉬 관련 변수
protected:
	_tchar			m_szName[MAX_STR] = L"";
	_int			m_iIndex = 0;
	_ulong			m_dwPassNum = 0;
	_mat			m_matLastVP;
	// 툴에서 사용되는 변수
protected:
	_bool			m_bIsSelected = false;
	_bool			m_bOnTool = false;

	// 오브젝트 렌더 그룹 변수
private:
	RENDERID		m_eGroup = RENDER_NONALPHA;

private:
	virtual HRESULT Initialize();
	virtual HRESULT LateInit_GameObject();
	virtual void	Init_Shader();

public:
	static CRenderObject* Create(_Device _pGraphicDev);
	static CRenderObject* CreateClone(CRenderObject* _pCopy);

public:
	virtual void Free();

public:
	virtual CGameObject* Clone_GameObject(void* pArg);
};

END

