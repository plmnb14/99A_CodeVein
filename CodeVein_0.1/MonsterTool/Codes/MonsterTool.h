#include "afxcmn.h"
#include "afxwin.h"

class CTestMonster;
class CTestWeapon;

class CMonsterTool : public CDialog
{
	DECLARE_DYNAMIC(CMonsterTool)

	enum ANI_EVENT_TYPE
	{ 
		MOVE,
		EFFECT,
		SOUND,
		COLLISION,
		ANI_EVENT_TYPE_END
	};

	struct ANI_EVENT_VALUE
	{
		_double m_dStartTime = 0;
		_double m_dEndTime = 0;
		_double m_dLoopTime = 0;
		_double m_dRange = 0;
		_double m_dMoveSpeed = 0;
		_double m_dMoveAccel = 0;
		_double m_dAniRatio = 0;
	};

	class Cuint_Finder
	{
	public:
		explicit Cuint_Finder(_uint _num) { m_uint = _num; }
		~Cuint_Finder() {		}

	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (m_uint == pair.first) return true;

			return false;
		}	
	private:
		_uint m_uint;
	};

public:
	CMonsterTool(CWnd* pParent = NULL);
	virtual ~CMonsterTool();

#ifdef AFX_DESIGN_TIME
	enum {IDD = IDD_MONSTERTOOL};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

public:
	void Update(const _float& fTimeDelta);
	void LateUpdate(const _float& fTimeDelta);
	void Render();
	void Release();
	void Seting_ComboBoxWeapon();
	void Seting_ListBoxMesh();
	void Seting_ListBoxAni();
	void Seting_ListBoxBone();
	void Seting_TreeCtrlAniEvent();
	void Seting_TreeCtrlAniCombo();

public:
	afx_msg void LDBClick_MeshList();
	afx_msg void LDBClick_BoneList();
	afx_msg void LDBClick_AniList();
	afx_msg void LDBClick_TreeCtrlAniEvent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void LDBClick_TreeCtrlAniCombo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void Change_AniSpeed();

	afx_msg void Add_AniEvent();
	afx_msg void Save_AniEvent();
	afx_msg void Load_AniEvent();

	afx_msg void Play_Ani();
	afx_msg void Stop_Ani();

	afx_msg void Add_AniCombo();
	afx_msg void Play_AniCombo();
	afx_msg void Save_AniCombo();
	afx_msg void Load_AniCombo();

	afx_msg void Create_Weapon();
	afx_msg void Attach_Weapon();
	afx_msg void Change_Radius();
	afx_msg void Change_Height();

	afx_msg void Spin_Radius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void Spin_Height(NMHDR *pNMHDR, LRESULT *pResult);

public:
	//몬스터 종류, 애니인덱스, 이벤트종류, 이벤트 값
	map<const _tchar*, map<_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>>> m_mapmapmapAniEvent;

	//몬스터 종류, 콤보번호, 애니인덱스, 애니재생비율
	typedef pair<_uint, _float> IdxRatio;
	map<const _tchar*, map<_uint, vector<IdxRatio>>> m_STLAniCombo;
	list<const _tchar*>				m_listMeshName; //글자 저장소
	list<const _tchar*>				m_listWeaponName;
	CTestMonster*					m_pMonster = nullptr;
	CTestWeapon*					m_pWeapon = nullptr;

	CListBox	m_ListBoxMesh;
	CListBox	m_ListBoxAni;
	CListBox	m_ListBoxBone;
	CListBox	m_ListBoxAniComboList;
	CTreeCtrl	m_TreeCtrlAniEvent;
	CTreeCtrl	m_TreeCtrlAniCombo;
	CComboBox	m_ComboBoxWeapon;

	//데이터 표기 관련 변수
	CString		m_strMeshName;
	CString		m_strBoneName;
	CString		m_strAniName;
	CString		m_strAniIndex;
	CString		m_strAniFullTime;
	CString		m_strAniTime;
	CString		m_strPlayAniRatio;
	CString		m_strAniPlayMul;
	CString		m_strAniComboRatio;
	CString		m_strAniComboNumber;

	//무기 변수
	CString		m_strWeaponName;
	CString		m_strAttachName;
	CString		m_strRadius;
	CString		m_strWpHeight;

	//버튼 체크 관련 변수
	CButton		m_ButtonMove;
	CButton		m_ButtonEffect;
	CButton		m_ButtonSound;
	CButton		m_ButtonCollision;

	//이동 이벤트 변수
	CString		m_strMoveStartTime;
	CString		m_strMoveEndTime;
	CString		m_strMoveLoopTime;
	CString		m_strMoveSpeed;
	CString		m_strMoveAccel;
	CString		m_strMoveAniRatio;

	//효과 이벤트 변수
	CString		m_strEffectStartTime;
	CString		m_strEffectEndTime;
	CString		m_strEffectLoopTime;
	CString		m_strEffectAniRatio;

	//소리 이벤트 변수
	CString		m_strSoundStartTime;
	CString		m_strSoundEndTime;
	CString		m_strSoundLoopTime;
	CString		m_strSoundAniRatio;

	//충돌 이벤트 변수
	CString		m_strCollisionStartTime;
	CString		m_strCollisionEndTime;
	CString		m_strCollisionRange;
	CString		m_strCollisionAniRatio;

	//몬스터	위치값
	CString		m_strObjPosX;
	CString		m_strObjPosY;
	CString		m_strObjPosZ;

};
