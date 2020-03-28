#pragma once
#include "afxwin.h"


// CResListPopup ��ȭ �����Դϴ�.

class CResListPopup : public CDialog
{
	DECLARE_DYNAMIC(CResListPopup)

public:
	CResListPopup(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CResListPopup();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESLISTPOPUP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	afx_msg void OnLbnSelchangeList();

	DECLARE_MESSAGE_MAP()
public:
	enum RES_TYPE { TYPE_MESH, TYPE_TEX, TYPE_COLOR, TYPE_GRADIENT, TYPE_END };
public:
	typedef struct tag_PathInfo
	{
		PATH_INFO tPathInfo;
		_int iMaxCount;
	}PATH_INFO_RE;
public:
	void Set_ResType(RES_TYPE eType);

private:
	void Load_TextureFromPath();
	void Load_MeshFromPath();

public:
	CListBox m_ListResourses;
	CStatic m_PictureControl;
	CImage	m_Img;

private:
	list<PATH_INFO_RE*>		m_listFileInfo;
	RES_TYPE m_eResType;

};
