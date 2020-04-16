namespace Engine
{
	class CGraphic_Dev;
	class CGameObject;
	class CRenderer;
}

class CMainFrame;
class CToolDoc;
class CSetingView;
class CTerrain_Guide;

class CToolView : public CView
{
protected:
	CToolView();
	DECLARE_DYNCREATE(CToolView)

public:
	CToolDoc* GetDocument() const;
	virtual void OnDraw(CDC* pDC);
	virtual ~CToolView();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnInitialUpdate();
	DECLARE_MESSAGE_MAP()

public:
	CString Convert_RelativePath(const _tchar* pFullPath);
	list<Engine::MESH_INFO*> Get_Meshlist() { return m_listMeshPathInfo; }
	list<Engine::MESH_INFO*> Get_Weaponlist() { return m_listWeaponPathInfo; }
	LPDIRECT3DDEVICE9 Get_Device() { return m_pDevice; }

	void Create_Mesh_PathInfo();
	void Extract_Mesh_PathInfo(const _tchar* pPath, list<MESH_INFO*>& rPathInfoLst, _bool _bIsDynamic = false);
	void Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst, _tchar* szPath);

	void Start();
	void Update(const _float& fTimeDelta);
	void LateUpdate(const _float& fTimeDelta);
	void Render();
	void KeyUp();
	void KeyDown();

	void Release();

public:
	list<Engine::MESH_INFO*>		m_listMeshPathInfo; //���� �̸���
	list<Engine::MESH_INFO*>		m_listWeaponPathInfo; //���� �̸���
	LPDIRECT3DDEVICE9				m_pDevice = nullptr; //������ �׷��� ����̽��� ���� ����
	Engine::CGameObject*			m_pGreed = nullptr; 
	CRenderer*						m_pRenderer = nullptr;

	CMainFrame*						m_pMainfrm;
	CSetingView*					m_pSetingView;

	_ulong							m_dwRenderCnt;
	_tchar							m_szFPS[MAX_STR];
	_float							m_fTime = 0.f;

};

