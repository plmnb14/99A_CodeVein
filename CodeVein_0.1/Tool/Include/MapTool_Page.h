#pragma once
#include "afxcmn.h"
#include "RenderObject.h"
#include "afxwin.h"


// CMapTool_Page 대화 상자입니다.

class CMapTool_Page : public CPropertyPage
{
	DECLARE_DYNAMIC(CMapTool_Page)

public:
	enum OBJECT_LAYER
	{
		OBJ_BACKGROUND, OBJ_PROP, OBJ_ACTOR, OBJ_EFFECT, OBJ_UI, OBJ_END
	};

	enum EDIT_MODE
	{
		_SELECT, _CREATE, _REMOVE, _EDIT_END
	};

public:
	CMapTool_Page();
	virtual ~CMapTool_Page();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL_PAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	void Set_MousePos(_v3 _vMousePos) {m_vMouse = _vMousePos;}
	void Delete_MouseObj();

public:
	void Add_Tree();
	void Add_Branch();

public:
	void Update();
	void Render();
	void MyUpdate_Data();

public:
	void KeyInput();
	void Key_Pos();
	void Key_Rot();
	void Key_Scale();

public:
	void Save_Object();
	void Load_Object();

public:
	void CreateObject();
	void Delete_SelectObject();

public:
	void Object_Movement();

public:
	void Check_SelectObj();

public:
	_bool	CheckObject();
	_bool	Get_ObjCheck() { return m_bCheckObj; }
	_int	Get_EditMode() { return m_iEditMode;}

public:
	void	LoadFilePath(const wstring& wstrImgPath);

public:
	void	Change_Mesh(const _tchar* _MeshName);
	void	Add_Object_OnTree(Engine::CRenderObject* _pObj);

public:
	virtual void Free();

private:
	Engine::CRenderObject*		m_pRenderObj;
	Engine::CRenderObject*		m_pSelectedObj;
	Engine::CRenderObject*		m_pPickObj;

private:
	typedef list<Engine::CGameObject*> OBJECT_LIST;
	OBJECT_LIST						m_listObject;
	OBJECT_LIST						m_listTempObj;

	list<Engine::PATH_INFO*> m_listMeshPathInfo; 

	list<TCHAR*>	 m_listStaticFolder;
	list<TCHAR*>	 m_listDynamicFolder;

	list<HTREEITEM*> m_listStaticBranch;
	list<HTREEITEM*> m_listDynamicBranch;

private:
	_float	m_fCross;
	_v3		m_vMouse;
	_bool	m_bCheckObj;

public:
	CTreeCtrl m_Tree;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree3(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio3();

	afx_msg void OnSelectMode(_uint _Mode);
	afx_msg void OnEditCtrl(_uint _Mode);
	afx_msg void OnEditType(_uint _Mode);

	afx_msg void OnEnChangeTransX();
	afx_msg void OnEnChangeTransY();
	afx_msg void OnEnChangeTransZ();
	afx_msg void OnEnChangeRotateX();
	afx_msg void OnEnChangeRotateY();
	afx_msg void OnEnChangeRotateZ();
	afx_msg void OnEnChangeScaleX();
	afx_msg void OnEnChangeScaleY();
	afx_msg void OnEnChangeScaleZ();

	virtual BOOL OnInitDialog();

	HTREEITEM hObjectRoot;



public:
	_bool m_bDelete = false;
	int m_iEditMode;
	CTreeCtrl m_pObjectTree;

	CString m_strPos_X;
	CString m_strPos_Y;
	CString m_strPos_Z;
	CString m_strRot_X;
	CString m_strRot_Y;
	CString m_strRot_Z;
	CString m_strScale_X;
	CString m_strScale_Y;
	CString m_strScale_Z;

	_v3		m_vPos;
	_v3		m_vRot;
	_v3		m_vScale;

	CString m_strPosRate;
	CString m_strRotRate;
	CString m_strScaleRate;

	_float	m_fPosRate;
	_float	m_fRotRate;
	_float	m_fScaleRate;

	afx_msg void OnEnChangeTransRate();
	afx_msg void OnEnChangeRotateRate();
	afx_msg void OnEnChangeScaleRate();

	int m_iEditType;
	BOOL m_bSelectAdvence;

	afx_msg void OnBnClickedTransform00();
	afx_msg void OnBnClickedTransform01();
	afx_msg void OnBnClickedTransform02();
	afx_msg void OnBnClickedCtrl01();
	afx_msg void OnBnClickedCtrl00();
	afx_msg void OnBnClickedButton5();

	CComboBox m_CTagBox;
	afx_msg void OnBnClickedSaveObj();
	afx_msg void OnBnClickedLoadObj();
	afx_msg void OnBnClickedDeleteAll();
};
