#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CNaviMesh 대화 상자입니다.

namespace Engine
{
	class CCell;
	class CGameObject;
}

class CNavMesh_Page : public CPropertyPage
{
	DECLARE_DYNAMIC(CNavMesh_Page)

public:
	enum POINTS
	{
		POINT_A, POINT_B, POINT_C, POINT_END
	};

	enum LINES
	{
		LINE_AB, LINE_BC, LINE_CA, LINE_END
	};

	enum GREED_MODE
	{
		_NONE, _GREED, __GREED_END
	};

	enum EDIT_MODE
	{
		_SELECT, _CREATE, _REMOVE, _EDIT_END
	};

	enum CTRL_MODE
	{
		_MOUSE, _KEYBOARD, _CTRL_END
	};

public:
	CNavMesh_Page();
	virtual ~CNavMesh_Page();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NAVIMESH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	void Select_Vtx(RAY _tRay);
	void Select_Tri(RAY _tRay);
	void Select_Poly(RAY _tRay);

public:
	void Move_Position();
	void Key_Down();

public:
	void Create_Vertex(LPDIRECT3DDEVICE9 pGraphicDev,  _v3 _vPos);
	void Create_Polygon(LPDIRECT3DDEVICE9 pGraphicDev);
	void Create_Vertex_Auto(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void Delete_SelectNav();

public:
	void MyUpdateData();

	void Calc_Neighbor();
	void Set_AreaParam();
	void Get_AreaParam();
	void Check_Line(_v3* _DstLine, CCell* _DstCell, CCell* _RscCell , _int _LineNum);

	void Add_Subset();

public:
	void Render();

public:
	void Create_Cell(LPDIRECT3DDEVICE9 pGraphicDev, _v3* _vPos);

public:
	void Save_CellData();
	void Load_CellData();
	void Clear_CellList();

private:
	bool Compare_Vertex(_v3 _vPos, _float _fLength = 0.2f);
	void Greed_Vertex(_v3* _vPos , CGameObject* _target = nullptr, _float _fLength = 0.2f);

public:
	virtual void Free();

public:
	_int m_iMax_VertexCnt;
	_int m_iCur_VertexCnt;
	_int m_iTotal_VertexCnt;
	_int m_iTotal_CellCnt;

public:
	_int	m_iIndex;
	_int	m_iVtxIndex;

public:
	_v3		m_vPoints[POINT_END];
	_v3		m_vLines[LINE_END];
	_v3		m_vCenter;

public:
	EDIT_MODE		m_eEditMode;

	// Pointer
public:
	CCell*	m_pSelectCell;

public:
	_int	m_iSubsetIdx;

	// STL
public:
	typedef list<CCell*>	CELL_LIST;

	vector<CELL_LIST>				m_vecSubset_Cell;
	vector<_v3>						m_vecVertexPos;
	CGameObject*			m_arrVertex[POINT_END];

	// TREE
public:
	CTreeCtrl			NaviNodeTree;
	HTREEITEM*			m_mainRoot;
	vector<HTREEITEM*>	m_vecSubset;
	vector<HTREEITEM*>	m_vecCellset;

	// MSG
public:
	afx_msg void OnEnChangeEdit11();
	afx_msg void OnEnChangeEdit12();
	afx_msg void OnEnChangeEdit13();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnChangeEdit4();

	virtual BOOL OnInitDialog();

private:
	CString m_YRate;
	CString m_YGap;

private:
	_float m_fYRate;
	_float m_fYGap;

private:
	_float m_fGreedRate;
	_int   m_iSelectIdx;

private:
	_int m_iGreedMode;
	_int m_iEditMode;
	_int m_iControlMode;

private:
	afx_msg void OnEnChangeEdit6();
	afx_msg void OnEnChangeEdit5();
	afx_msg void OnBnClickedGreed00();
	afx_msg void OnBnClickedGreed01();
	afx_msg void OnBnClickedEmode00();
	afx_msg void OnBnClickedEmode01();
	afx_msg void OnBnClickedEmode02();
	afx_msg void OnBnClickedControl00();
	afx_msg void OnBnClickedControl01();

	afx_msg void OnGreedMode(_uint _Mode);
	afx_msg void OnEditMode(_uint _Mode);
	afx_msg void OnCtrlMode(_uint _Mode);

	//private:
	//	_v3 vCenter

private:
	CString m_strLineAB;
	CString m_strLineBC;
	CString m_strLineCA;
	CString m_strCenter_X;
	CString m_strCenter_Y;
	CString m_strCenter_Z;
	CString m_strGreedRate;
	CString m_strSelectIdx;

private:
	CButton m_cSave;

private:
	CComboBox m_cAreaList;

private:
	afx_msg void OnBnClickedButton7();
	afx_msg void OnEnChangeIndex();
	afx_msg void OnBnClickedCellSave();
	afx_msg void OnBnClickedCellLoad();
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit1();

	CString m_strSubsetIdx;
};
