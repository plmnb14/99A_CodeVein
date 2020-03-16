
// ToolView.h : CToolView 클래스의 인터페이스
//

#pragma once

namespace Engine
{
	class CGraphic_Dev;
	class CGameObject;
	class CManagement;
	class CCameraMgr;
	class CLayer;
	class CVIBuffer;
	class CRenderObject;
	class CInput_Dev;
};

class CInspector_Page;
class CMapTool_Page;
class CNavMesh_Page;
class CPivot_Page;
class CToolDoc;
class CToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	CToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

public:
	void Setup_Default();
	void SetUp_Layer();
	void SetUp_Cam();

public:
	void Function();
	void Update();
	void LateInit();
	void Render();

public:
	void Selected_Object();
	void Selected_Transform();
	void Trans_Pivot();
	void Trans_Axis();

public:
	void Check_ActivePage();
	void Get_Pages();

public:
	void Change_Data();

public:
	void Set_RendPivot();

public:
	void Global_KeyState();
	void Global_KeyUp();
	void Global_KeyDown();

public:
	void Create_Cell();

public:
	void Free();

private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	Engine::CGraphic_Dev*	m_pDeviceMgr;

private:
	Engine::CGameObject* m_pGreedTerrain = nullptr;

public:
	Engine::RAY				m_tRay;

public:
	_bool	m_bLateInit;

public:
	_bool	m_bActive_NevMesh;
	_bool	m_bActive_Object;

public:
	_bool	m_bObjMouse;
	_bool	m_bSelectObj;
	_bool	m_bSelect;

	_float	m_fCrossLength;
	_float	m_fConvertLength;

public:
	_v3		m_vSelectMove;

public:
	_bool	m_bActive_Pos_x;
	_bool	m_bActive_Pos_y;
	_bool	m_bActive_Pos_z;

public:
	_bool	m_bNavMesh_Mode;
	_bool	m_bAutoPick;


public:
	CNavMesh_Page*		m_pPage_Navmesh;
	CMapTool_Page*		m_pPage_MapTool;
	CInspector_Page*	m_pPage_Inspecter;
	CPivot_Page*		m_pPage_Pivot;

public:
	_int	m_iIndex;

public:

	//typedef map<ENGINE::OBJ_LAYER, ENGINE::CLayer*>	MAP_LAYER;
	//MAP_LAYER m_mapLayer;
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

