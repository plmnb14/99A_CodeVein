#include "stdafx.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "SetingView.h"
#include "MonsterTool.h"
#include "AniCtrl.h"
#include "TestObject.h"

IMPLEMENT_DYNAMIC(CMonsterTool, CDialog)

CMonsterTool::CMonsterTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MONSTERTOOL, pParent), m_strObjPosX(_T("")), m_strObjPosY(_T("")), m_strObjPosZ(_T(""))
	, m_strMeshName(_T("")), m_strBoneName(_T("")), m_strAniName(_T("")), m_strAniIndex(_T("")), m_strAniTime(_T("")), m_strAniPlayMul(_T("")), m_strAniFullTime(_T(""))
	, m_strEffectStartTime(_T("")), m_strEffectEndTime(_T("")), m_strEffectLoopTime(_T("")), m_strEffectAniRatio(_T(""))
	, m_strMoveStartTime(_T("")), m_strMoveEndTime(_T("")), m_strMoveLoopTime(_T("")), m_strMoveSpeed(_T("")), m_strMoveAccel(_T("")), m_strMoveAniRatio(_T(""))
	, m_strSoundStartTime(_T("")), m_strSoundEndTime(_T("")), m_strSoundLoopTime(_T("")), m_strSoundAniRatio(_T(""))
	, m_strCollisionStartTime(_T("")), m_strCollisionEndTime(_T("")), m_strCollisionRange(_T("")), m_strCollisionAniRatio(_T(""))
	, m_strPlayAniRatio(_T("")), m_strAniComboRatio(_T("")), m_strAniComboNumber(_T(""))
{
}

CMonsterTool::~CMonsterTool()
{
	for (auto& list_iter : m_listMeshName)
		Safe_Delete(list_iter);

	m_listMeshName.clear();

	for (auto& Meshmap_iter : m_mapmapmapAniEvent)
	{
		for (auto& AniIndexmap_iter : Meshmap_iter.second)
		{
			for (auto& AniTypemap_iter : AniIndexmap_iter.second)
			{
				Safe_Delete(AniTypemap_iter.second); //구조체 지우기
			}
			AniIndexmap_iter.second.clear(); //구조체그릇 지우기
		}

		Meshmap_iter.second.clear(); //구조체그릇 담았던 그릇 지우기
	}

	m_mapmapmapAniEvent.clear(); //다 지우기
}

void CMonsterTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ObjPosX, m_strObjPosX);
	DDX_Text(pDX, IDC_ObjPosY, m_strObjPosY);
	DDX_Text(pDX, IDC_ObjPosZ, m_strObjPosZ);

	DDX_Control(pDX, IDC_MESHLIST, m_ListBoxMesh);
	DDX_Control(pDX, IDC_ANILIST, m_ListhBoxAni);
	DDX_Control(pDX, IDC_BONELIST, m_ListBoxBone);
	DDX_Control(pDX, IDC_ANICOMBOTREE, m_TreeCtrlAniCombo);
	DDX_Control(pDX, IDC_ANIEVENTTREE, m_TreeCtrlAniEvent);

	DDX_Text(pDX, IDC_MESHNAME, m_strMeshName);
	DDX_Text(pDX, IDC_BONENAME, m_strBoneName);
	DDX_Text(pDX, IDC_ANINAME, m_strAniName);
	DDX_Text(pDX, IDC_ANIINDEX, m_strAniIndex);
	DDX_Text(pDX, IDC_ANIFULLTIME, m_strAniFullTime);
	DDX_Text(pDX, IDC_ANITIME, m_strAniTime);
	DDX_Text(pDX, IDC_ANIPLAYMUL, m_strAniPlayMul);

	DDX_Control(pDX, IDC_MOVEEVENT, m_ButtonMove);
	DDX_Control(pDX, IDC_EFFECTEVENT, m_ButtonEffect);
	DDX_Control(pDX, IDC_SOUNDEVENT, m_ButtonSound);
	DDX_Control(pDX, IDC_COLLISIONEVENT, m_ButtonCollision);

	DDX_Text(pDX, IDC_MOVESTARTTIME, m_strMoveStartTime);
	DDX_Text(pDX, IDC_MOVEENDTIME, m_strMoveEndTime);
	DDX_Text(pDX, IDC_MOVELOOPTIME, m_strMoveLoopTime);
	DDX_Text(pDX, IDC_MOVESPEED, m_strMoveSpeed);
	DDX_Text(pDX, IDC_MOVEACCEL, m_strMoveAccel);
	DDX_Text(pDX, IDC_MOVEANIRATIO, m_strMoveAniRatio);

	DDX_Text(pDX, IDC_EFFECTSTARTTIME, m_strEffectStartTime);
	DDX_Text(pDX, IDC_EFFECTENDTIME, m_strEffectEndTime);
	DDX_Text(pDX, IDC_EFFECTLOOPTIME, m_strEffectLoopTime);
	DDX_Text(pDX, IDC_EFFECTANIRATIO, m_strEffectAniRatio);

	DDX_Text(pDX, IDC_SOUNDSTARTTIME, m_strSoundStartTime);
	DDX_Text(pDX, IDC_SOUNDENDTIME, m_strSoundEndTime);
	DDX_Text(pDX, IDC_SOUNDLOOPTIME, m_strSoundLoopTime);
	DDX_Text(pDX, IDC_SOUNDANIRATIO, m_strSoundAniRatio);

	DDX_Text(pDX, IDC_COLLISIONANIRATIO, m_strCollisionAniRatio);
	DDX_Text(pDX, IDC_COLLISIONSTARTTIME, m_strCollisionStartTime);
	DDX_Text(pDX, IDC_COLLISIONENDTIME, m_strCollisionEndTime);
	DDX_Text(pDX, IDC_COLLISIONRANGE, m_strCollisionRange);

	DDX_Text(pDX, IDC_PLAYANIRATIO, m_strPlayAniRatio);
	DDX_Text(pDX, IDC_ANICOMBORATIO, m_strAniComboRatio);
	DDX_Text(pDX, IDC_ANICOMBONUMBER, m_strAniComboNumber);
}

BOOL CMonsterTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	Seting_ListBoxMesh();

	return TRUE;
}

void CMonsterTool::OnOK()
{
	return;
}

void CMonsterTool::OnCancel()
{
	return;
}

BEGIN_MESSAGE_MAP(CMonsterTool, CDialog)
	ON_LBN_DBLCLK(IDC_MESHLIST, &CMonsterTool::LDBClick_MeshList)
	ON_LBN_DBLCLK(IDC_BONELIST, &CMonsterTool::LDBClick_BoneList)
	ON_LBN_DBLCLK(IDC_ANILIST, &CMonsterTool::LDBClick_AniList)
	ON_NOTIFY(NM_DBLCLK, IDC_ANICOMBOTREE, &CMonsterTool::LDBClick_TreeCtrlAniCombo)
	ON_NOTIFY(NM_DBLCLK, IDC_ANIEVENTTREE, &CMonsterTool::LDBClick_TreeCtrlAniEvent)

	ON_EN_CHANGE(IDC_ANIPLAYMUL, &CMonsterTool::Change_AniSpeed)
	ON_BN_CLICKED(IDC_PLAY, &CMonsterTool::Play_Ani)
	ON_BN_CLICKED(IDC_STOP, &CMonsterTool::Stop_Ani)

	ON_BN_CLICKED(IDC_ADDEVENT, &CMonsterTool::Add_AniEvent)
	ON_BN_CLICKED(IDC_ANIEVEVNTSAVE, &CMonsterTool::Save_AniEvent)

	ON_BN_CLICKED(IDC_ADDANICOMBO, &CMonsterTool::Add_AniCombo)
	ON_BN_CLICKED(IDC_PLAYANICOMBO, &CMonsterTool::Play_AniCombo)
END_MESSAGE_MAP()

void CMonsterTool::Update(const _float & fTimeDelta)
{
	IF_NOT_NULL(m_pTestObject)
	{
		UpdateData(TRUE);

		m_pTestObject->Update_GameObject(fTimeDelta);
		m_strAniFullTime.Format(L"%.2f", TARGET_TO_D_MESH(m_pTestObject)->Get_AnimationFullTime());
		m_strAniTime.Format(L"%.2f", TARGET_TO_D_MESH(m_pTestObject)->Get_TrackInfo().Position);

		_double NowAniTime = TARGET_TO_D_MESH(m_pTestObject)->Get_TrackInfo().Position;
		_double MaxAniTime = TARGET_TO_D_MESH(m_pTestObject)->Get_AnimationFullTime();
		
		_int x = _int(NowAniTime / MaxAniTime);
		
		m_strAniTime.Format( L"%.2f", NowAniTime - (MaxAniTime * x) );
		m_strPlayAniRatio.Format(L"%.2f", (NowAniTime / MaxAniTime) - x);

		m_strObjPosX.Format(L"%.2f", TARGET_TO_TRANS(m_pTestObject)->Get_Pos().x);
		m_strObjPosY.Format(L"%.2f", TARGET_TO_TRANS(m_pTestObject)->Get_Pos().y);
		m_strObjPosZ.Format(L"%.2f", TARGET_TO_TRANS(m_pTestObject)->Get_Pos().z);
		
		UpdateData(FALSE);
	}

	return;
}

void CMonsterTool::LateUpdate(const _float & fTimeDelta)
{
	UpdateData(TRUE);

	IF_NOT_NULL(m_pTestObject)
	{
		m_pTestObject->Late_Update_GameObject(fTimeDelta);
	}

	if (FALSE == m_ButtonMove.GetCheck())
	{
		((CEdit*)GetDlgItem(IDC_MOVESTARTTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_MOVESTARTTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_MOVEENDTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_MOVEENDTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_MOVELOOPTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_MOVELOOPTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_MOVESPEED))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_MOVESPEED))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_MOVEACCEL))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_MOVEACCEL))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_MOVEANIRATIO))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_MOVEANIRATIO))->SetWindowTextW(L"");
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_MOVESTARTTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_MOVEENDTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_MOVELOOPTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_MOVESPEED))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_MOVEACCEL))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_MOVEANIRATIO))->SetReadOnly(FALSE);
	}

	if (FALSE == m_ButtonEffect.GetCheck())
	{
		((CEdit*)GetDlgItem(IDC_EFFECTSTARTTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EFFECTSTARTTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_EFFECTENDTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EFFECTENDTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_EFFECTLOOPTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EFFECTLOOPTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_EFFECTANIRATIO))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_EFFECTANIRATIO))->SetWindowTextW(L"");
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_EFFECTSTARTTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EFFECTENDTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EFFECTLOOPTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_EFFECTANIRATIO))->SetReadOnly(FALSE);
	}

	if (FALSE == m_ButtonSound.GetCheck())
	{
		((CEdit*)GetDlgItem(IDC_SOUNDSTARTTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_SOUNDSTARTTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_SOUNDENDTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_SOUNDENDTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_SOUNDLOOPTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_SOUNDLOOPTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_SOUNDANIRATIO))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_SOUNDANIRATIO))->SetWindowTextW(L"");
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_SOUNDSTARTTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_SOUNDENDTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_SOUNDLOOPTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_SOUNDANIRATIO))->SetReadOnly(FALSE);
	}

	if (FALSE == m_ButtonCollision.GetCheck())
	{
		((CEdit*)GetDlgItem(IDC_COLLISIONANIRATIO))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_COLLISIONANIRATIO))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_COLLISIONSTARTTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_COLLISIONSTARTTIME))->SetWindowTextW(L"");
		
		((CEdit*)GetDlgItem(IDC_COLLISIONENDTIME))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_COLLISIONENDTIME))->SetWindowTextW(L"");

		((CEdit*)GetDlgItem(IDC_COLLISIONRANGE))->SetReadOnly(TRUE);
		((CEdit*)GetDlgItem(IDC_COLLISIONRANGE))->SetWindowTextW(L"");
	}
	else
	{
		((CEdit*)GetDlgItem(IDC_COLLISIONSTARTTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_COLLISIONENDTIME))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_COLLISIONRANGE))->SetReadOnly(FALSE);
		((CEdit*)GetDlgItem(IDC_COLLISIONANIRATIO))->SetReadOnly(FALSE);
	}

	return;
}

void CMonsterTool::Render()
{
	IF_NOT_NULL(m_pTestObject)
	{
		m_pTestObject->Render_GameObject();
	}

	return;
}

void CMonsterTool::Release()
{
	IF_NOT_NULL(m_pTestObject)
	{
		Safe_Release(m_pTestObject);
	}
}

void CMonsterTool::Seting_ListBoxMesh()
{
	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView* pToolView = static_cast<CToolView*>(pFrameWnd->m_MainSplitter.GetPane(0, 0));

	m_ListBoxMesh.ResetContent();

	list<Engine::MESH_INFO*> listTemp = pToolView->Get_Meshlist();

	for (auto& list_iter : listTemp)
	{
		CString temp(list_iter->wstrStateKey.c_str());

		_tchar* szTemp = new _tchar[MAX_STR];
		lstrcpy(szTemp, temp);
		m_listMeshName.push_back(szTemp);

		m_ListBoxMesh.AddString(temp);
	}

	return;
}

void CMonsterTool::Seting_ListBoxAni()
{
	UpdateData(TRUE);

	m_ListhBoxAni.ResetContent(); //모두 지우기

	IF_NOT_NULL(m_pTestObject)
	{
		CAniCtrl* pAniCtrl = TARGET_TO_D_MESH(m_pTestObject)->Get_AniCtrl();
		_uint iCount = pAniCtrl->Get_AniCtrl()->GetMaxNumAnimationSets();

		LPD3DXANIMATIONSET	pAS = nullptr;
		_tchar* szRootName;

		for (_uint i = 0; i < pAniCtrl->Get_AniCtrl()->GetMaxNumAnimationSets(); ++i)
		{
			pAniCtrl->Get_AniCtrl()->GetAnimationSet(i, &pAS);

			const char* charName = pAS->GetName();
			_uint strSize = MultiByteToWideChar(CP_ACP, 0, charName, -1, NULL, NULL);
			szRootName = new _tchar[strSize];
			MultiByteToWideChar(CP_ACP, 0, charName, _int(strlen(charName) + 1), szRootName, strSize);

			m_ListhBoxAni.AddString(szRootName);

			Safe_Delete_Array(szRootName);
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Seting_ListBoxBone()
{
	UpdateData(TRUE);

	m_ListBoxBone.ResetContent();

	IF_NOT_NULL(m_pTestObject)
	{
		_tchar* szBoneName;

		for (auto vector_iter : TARGET_TO_D_MESH(m_pTestObject)->Get_MeshContainer())
		{
			for (_uint i = 0; i < vector_iter->dwNumBones; ++i)
			{
 				const char*	pBoneName = vector_iter->pSkinInfo->GetBoneName(i);
				_uint strSize = MultiByteToWideChar(CP_ACP, 0, pBoneName, -1, NULL, NULL);
				szBoneName = new _tchar[strSize];
				MultiByteToWideChar(CP_ACP, 0, pBoneName, _int(strlen(pBoneName) + 1), szBoneName, strSize);

				m_ListBoxBone.AddString(szBoneName);

				Safe_Delete_Array(szBoneName);
			}
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Seting_TreeCtrlAniEvent()
{
	UpdateData(TRUE);

	IF_NOT_NULL(m_pTestObject)
	{
		HTREEITEM Root;
		CString temp;

		m_TreeCtrlAniEvent.DeleteAllItems();

		_uint num = _ttoi(m_strAniIndex);

		//해당 몬스터 찾기
		auto& Mesh_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

		//해당 몬스터가 없다면
		if (Mesh_iter == m_mapmapmapAniEvent.end())
			return;

		//해당 인덱스를 찾지 못했다면
		if (Mesh_iter->second.find(num) == Mesh_iter->second.end())
			return;

		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"MOVE", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"EFFECT", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"SOUND", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"COLLISION", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.2f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Seting_TreeCtrlAniCombo()
{
	UpdateData(TRUE);

	IF_NOT_NULL(m_pTestObject)
	{
		m_TreeCtrlAniCombo.DeleteAllItems();

		HTREEITEM Root;
		CString strTempComboNumber;
		CString strTempAniIndex;
		CString strTempAniRatio;
		CString strTempAniName;

		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				auto& Mesh_iter = find_if(m_mapmapmapAniCombo.begin(), m_mapmapmapAniCombo.end(), CTag_Finder(list_iter));

				if (Mesh_iter == m_mapmapmapAniCombo.end())
					return;

				for (auto& AniComboNumber_iter : Mesh_iter->second) //_uint, 콤보번호
				{
					strTempComboNumber.Format(L"%d", AniComboNumber_iter.first);
					Root = m_TreeCtrlAniCombo.InsertItem(strTempComboNumber, 0, 0, TVI_ROOT, TVI_LAST);

					for (auto& AniIndex_iter : AniComboNumber_iter.second) //_uint, 애니인덱스
					{
						strTempAniIndex.Format(L"%d", AniIndex_iter.first);
						strTempAniRatio.Format(L"%.2f", AniIndex_iter.second);
						m_ListhBoxAni.GetText(AniIndex_iter.first, strTempAniName);
						m_TreeCtrlAniCombo.InsertItem(strTempAniIndex + L"/" + strTempAniRatio + L"/ " + strTempAniName, 0, 0, Root, TVI_LAST);
						break;
					}
				}

			}
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::LDBClick_MeshList()
{
	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView* pToolView = static_cast<CToolView*>(pFrameWnd->m_MainSplitter.GetPane(0, 0));

	IF_NULL(m_pTestObject)
	{	
		UpdateData(TRUE);
		m_ListBoxMesh.GetText(m_ListBoxMesh.GetCurSel(), m_strMeshName);
		UpdateData(FALSE);

		_tchar szTemp[MAX_STR];
		lstrcpy(szTemp, m_strMeshName);
		m_pTestObject = CTestObject::Create(pToolView->Get_Device(), szTemp);

		Seting_ListBoxAni();
		Seting_ListBoxBone();
		Seting_TreeCtrlAniCombo();

		m_TreeCtrlAniEvent.DeleteAllItems();
	}
	else
	{
		Safe_Release(m_pTestObject);

		UpdateData(TRUE);
		m_ListBoxMesh.GetText(m_ListBoxMesh.GetCurSel(), m_strMeshName);
		UpdateData(FALSE);

		_tchar szTemp[MAX_STR];
		lstrcpy(szTemp, m_strMeshName);
		m_pTestObject = CTestObject::Create(pToolView->Get_Device(), szTemp);

		Seting_ListBoxAni();
		Seting_ListBoxBone();
		Seting_TreeCtrlAniCombo();

		m_TreeCtrlAniEvent.DeleteAllItems();
	}

	return;
}

void CMonsterTool::LDBClick_BoneList()
{
	UpdateData(TRUE);

	if (LB_ERR != m_ListBoxBone.GetCurSel())
	{
		IF_NOT_NULL(m_pTestObject)
		{
			m_ListBoxBone.GetText(m_ListBoxBone.GetCurSel(), m_strBoneName);
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::LDBClick_AniList()
{
	UpdateData(TRUE);

	if (LB_ERR != m_ListhBoxAni.GetCurSel())
	{
		IF_NOT_NULL(m_pTestObject)
		{
			UpdateData(TRUE);
			m_ListhBoxAni.GetText(m_ListhBoxAni.GetCurSel(), m_strAniName);
			m_strAniIndex.Format(L"%d", m_ListhBoxAni.GetCurSel());
			UpdateData(FALSE);

			Seting_TreeCtrlAniEvent();
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::LDBClick_TreeCtrlAniEvent(NMHDR *pNMHDR, LRESULT *pResult)
{
	//부모 노드를 더블클릭하면 지운다
	//자식 항목 선택시에는 아무 일도 없다
	HTREEITEM hSelectedItem = m_TreeCtrlAniEvent.GetSelectedItem();
	CString strTemp = m_TreeCtrlAniEvent.GetItemText(hSelectedItem);

	_uint idx = _ttoi(m_strAniIndex);
	_float Ratio = _float(_ttof(m_strAniComboRatio));

	if (0 == lstrcmp(strTemp, L"MOVE"))
	{
		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, m_strMeshName))
			{
				auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder(list_iter));

				for (auto& AniIndex_iter : Meshmap_iter->second)
				{
					if (idx == AniIndex_iter.first)
					{
						for (auto& EventType_iter : AniIndex_iter.second)
						{
							if (ANI_EVENT_TYPE::MOVE == EventType_iter.first)
							{
								Safe_Delete(EventType_iter.second);
								AniIndex_iter.second.erase(EventType_iter.first);
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		Seting_TreeCtrlAniEvent();
	}

	if (0 == lstrcmp(strTemp, L"EFFECT"))
	{
		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, m_strMeshName))
			{
				auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder(list_iter));

				for (auto& AniIndex_iter : Meshmap_iter->second)
				{
					if (idx == AniIndex_iter.first)
					{
						for (auto& EventType_iter : AniIndex_iter.second)
						{
							if (ANI_EVENT_TYPE::EFFECT == EventType_iter.first)
							{
								Safe_Delete(EventType_iter.second);
								AniIndex_iter.second.erase(EventType_iter.first);
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		Seting_TreeCtrlAniEvent();
	}

	if (0 == lstrcmp(strTemp, L"SOUND"))
	{
		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, m_strMeshName))
			{
				auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder(list_iter));

				for (auto& AniIndex_iter : Meshmap_iter->second)
				{
					if (idx == AniIndex_iter.first)
					{
						for (auto& EventType_iter : AniIndex_iter.second)
						{
							if (ANI_EVENT_TYPE::SOUND == EventType_iter.first)
							{
								Safe_Delete(EventType_iter.second);
								AniIndex_iter.second.erase(EventType_iter.first);
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		Seting_TreeCtrlAniEvent();
	}

	if (0 == lstrcmp(strTemp, L"COLLISION"))
	{
		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, m_strMeshName))
			{
				auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder(list_iter));

				for (auto& AniIndex_iter : Meshmap_iter->second)
				{
					if (idx == AniIndex_iter.first)
					{
						for (auto& EventType_iter : AniIndex_iter.second)
						{
							if (ANI_EVENT_TYPE::COLLISION == EventType_iter.first)
							{
								Safe_Delete(EventType_iter.second);
								AniIndex_iter.second.erase(EventType_iter.first);
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}
		Seting_TreeCtrlAniEvent();
	}

	*pResult = 0;
}

void CMonsterTool::LDBClick_TreeCtrlAniCombo(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelectedItem = m_TreeCtrlAniCombo.GetSelectedItem();
	CString strText = m_TreeCtrlAniCombo.GetItemText(hSelectedItem); //선택된 친구의 이름

	_tchar szTreeItemNameText[MAX_STR];
	lstrcpy(szTreeItemNameText, strText);

	HTREEITEM ParentOrChildNode = Find_Node_By_Name(m_TreeCtrlAniCombo, szTreeItemNameText); //부모인지 자식인지 판별
	CString strParentOrChild = m_TreeCtrlAniCombo.GetItemText(ParentOrChildNode); //부모또는자식 이름

	if (0 > strParentOrChild.Find(L"/")) //자식이라면
	{
		CString strComboNum = strParentOrChild.Left(strParentOrChild.ReverseFind(L'/'));

		_tchar szTemp[MAX_STR];
		lstrcpy(szTemp, m_strMeshName);
		HTREEITEM hTemp, hClickedChildFindParent;
		CString strParentName;

		hTemp = m_TreeCtrlAniCombo.GetNextItem(NULL, TVGN_CARET);
		hClickedChildFindParent = m_TreeCtrlAniCombo.GetNextItem(hTemp, TVGN_PARENT); // 현재 선택되어진 아이템의 상위 아이템을 가져온다.
		strParentName = m_TreeCtrlAniCombo.GetItemText(hClickedChildFindParent); // 그 아이템의 이름을 얻어온다.

		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, szTemp))
			{
				auto& MeshName_iter = find_if(m_mapmapmapAniCombo.begin(), m_mapmapmapAniCombo.end(), CTag_Finder(list_iter));

				for (auto& AniCombo_iter : MeshName_iter->second)
				{
					//콤보 번호 검색
					if (_ttoi(strParentName) == AniCombo_iter.first)
					{
						for (auto& AniIndex_iter : AniCombo_iter.second)
						{
							//애니인덱스 번호 검색
							if (_ttoi(strComboNum) == AniIndex_iter.first)
							{
								AniCombo_iter.second.erase(_ttoi(strComboNum));
								break;
							}
						}
					}
				}
			}
		}
		Seting_TreeCtrlAniCombo();
	}
	else //자식 선택시 특정값만 지우기
	{
		_tchar szTemp[MAX_STR];
		lstrcpy(szTemp, m_strMeshName);

		CString strComboNum = m_TreeCtrlAniCombo.GetItemText(hSelectedItem);

		for (auto& list_iter : m_listMeshName)
		{
			if (!lstrcmp(list_iter, szTemp))
			{
				auto& Meshname_iter = find_if(m_mapmapmapAniCombo.begin(), m_mapmapmapAniCombo.end(), CTag_Finder(list_iter));

				for (auto& ComboNumber_iter : Meshname_iter->second) // 콤보번호 _uint
				{
					if (_ttoi(strComboNum) == ComboNumber_iter.first)
					{
						Meshname_iter->second.clear();
						Meshname_iter->second.erase(_ttoi(strComboNum));
						break;
					}
				}
			}
		}
		Seting_TreeCtrlAniCombo();
	}


	*pResult = 0;
}

HTREEITEM CMonsterTool::Find_Node_By_Name(CTreeCtrl & _TreeCtrl, const _tchar * _szTargetName, HTREEITEM _RootNode)
{
	HTREEITEM NodeFinder;
	HTREEITEM treeNext;

	// 시작 노드가 없다면, 최상위 부모를 받아온다.
	IF_NULL(_RootNode)
	{
		NodeFinder = _TreeCtrl.GetRootItem();
	}
	else // 아니라면 시작 위치를 인자값 노드로 설정한다.
	{
		NodeFinder = _RootNode;
	}

	// 찾는 이름과 같다면,
	if (_szTargetName == _TreeCtrl.GetItemText(NodeFinder))
		return NodeFinder;

	// 만약 형제가 있다면
	IF_NOT_NULL(treeNext = _TreeCtrl.GetNextSiblingItem(NodeFinder))
	{
		// 형제를 노드 인자값으로 넣고 다시 찾기.
		NodeFinder = Find_Node_By_Name(_TreeCtrl, _szTargetName, treeNext);

		// 만약 찾으면 반환
		IF_NOT_NULL(NodeFinder)
			return NodeFinder;
	}

	// 만약 자식이 있다면
	IF_NOT_NULL(treeNext = _TreeCtrl.GetChildItem(NodeFinder))
	{
		// 자식 노드 인자값으로 넣고 다시 찾기.
		NodeFinder = Find_Node_By_Name(_TreeCtrl, _szTargetName, treeNext);

		// 만약 찾으면 반환
		IF_NOT_NULL(NodeFinder)
			return NodeFinder;
	}

	return NULL;
}

void CMonsterTool::Change_AniSpeed()
{
	UpdateData(TRUE);

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Play_Ani()
{
	UpdateData(TRUE);

	if (LB_ERR != m_ListhBoxAni.GetCurSel())
	{
		IF_NOT_NULL(m_pTestObject)
		{
			m_strAniFullTime.Format(L"%.2f", TARGET_TO_D_MESH(m_pTestObject)->Get_AnimationFullTime());
			m_strAniTime.Format(L"%.2f", TARGET_TO_D_MESH(m_pTestObject)->Get_TrackInfo().Position);
			TARGET_TO_D_MESH(m_pTestObject)->SetUp_Animation(_ttoi(m_strAniIndex));
			m_pTestObject->Set_AniSpeed((_float)_wtof(m_strAniPlayMul));

		}
	}

	UpdateData(FALSE);
}

void CMonsterTool::Stop_Ani()
{
	UpdateData(TRUE);

	IF_NOT_NULL(m_pTestObject)
	{
		m_pTestObject->Set_AniSpeed(0.f);
		m_strAniFullTime.Format(L"%.2f", TARGET_TO_D_MESH(m_pTestObject)->Get_AnimationFullTime());
		m_strAniTime.Format(L"%.2f", TARGET_TO_D_MESH(m_pTestObject)->Get_TrackInfo().Position);
	}

	UpdateData(FALSE);
}

void CMonsterTool::Add_AniEvent()
{
	if (TRUE == m_ButtonMove.GetCheck())
	{
		auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

		//해당 몬스터가 map에 없다면
		if (Meshmap_iter == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strMoveStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strMoveEndTime);
			TempEventValue->m_dLoopTime = _wtof(m_strMoveLoopTime);
			TempEventValue->m_dMoveSpeed = _wtof(m_strMoveSpeed);
			TempEventValue->m_dMoveAccel = _wtof(m_strMoveAccel);
			TempEventValue->m_dAniRatio = _wtof(m_strMoveAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::MOVE, TempEventValue);

			_uint num = _ttoi(m_strAniIndex);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(num, FirstValue);
		
			for (auto& list_iter : m_listMeshName)
			{
				if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
				{
					m_mapmapmapAniEvent.emplace(list_iter, SecondValue);
					break;
				}
			}
		}
		else //해당 몬스터가 있다면
		{
			_uint num = _ttoi(m_strAniIndex);

			for (auto& AniIdx_iter : Meshmap_iter->second)
			{
				//해당 애니인덱스가 없다면
				if (Meshmap_iter->second.find(num) == Meshmap_iter->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strMoveStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strMoveEndTime);
					TempEventValue->m_dLoopTime = _wtof(m_strMoveLoopTime);
					TempEventValue->m_dMoveSpeed = _wtof(m_strMoveSpeed);
					TempEventValue->m_dMoveAccel = _wtof(m_strMoveAccel);
					TempEventValue->m_dAniRatio = _wtof(m_strMoveAniRatio);

					map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
					FirstValue.emplace(ANI_EVENT_TYPE::MOVE, TempEventValue);
					Meshmap_iter->second.emplace(num, FirstValue);
					break;
				}
				else //해당 애니인덱스가 있다면
				{
					for (auto& EventType_iter : AniIdx_iter.second)
					{
						//해당 이벤트가 없다면
						if (Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE) == Meshmap_iter->second.find(num)->second.end())
						{
							ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
							TempEventValue->m_dStartTime = _wtof(m_strMoveStartTime);
							TempEventValue->m_dEndTime = _wtof(m_strMoveEndTime);
							TempEventValue->m_dLoopTime = _wtof(m_strMoveLoopTime);
							TempEventValue->m_dMoveSpeed = _wtof(m_strMoveSpeed);
							TempEventValue->m_dMoveAccel = _wtof(m_strMoveAccel);
							TempEventValue->m_dAniRatio = _wtof(m_strMoveAniRatio);

							Meshmap_iter->second.find(num)->second.emplace(ANI_EVENT_TYPE::MOVE, TempEventValue);
							break;
						}
						else //해당 이벤트가 있다면
						{
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dStartTime = _wtof(m_strMoveStartTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dEndTime = _wtof(m_strMoveEndTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dLoopTime = _wtof(m_strMoveLoopTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dMoveSpeed = _wtof(m_strMoveSpeed);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dMoveAccel = _wtof(m_strMoveAccel);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dAniRatio = _wtof(m_strMoveAniRatio);
							break;
						}
					}
					break;
				}
			}
		}
	}

	if (TRUE == m_ButtonEffect.GetCheck())
	{
		auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

		if (Meshmap_iter == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strEffectStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strEffectEndTime);
			TempEventValue->m_dLoopTime = _wtof(m_strEffectLoopTime);
			TempEventValue->m_dAniRatio = _wtof(m_strEffectAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::EFFECT, TempEventValue);

			_uint num = _ttoi(m_strAniIndex);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(num, FirstValue);

			for (auto& list_iter : m_listMeshName)
			{
				if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
				{
					m_mapmapmapAniEvent.emplace(list_iter, SecondValue);
					break;
				}
			}
		}
		else
		{
			_uint num = _ttoi(m_strAniIndex);

			for (auto& AniIdx_iter : Meshmap_iter->second)
			{
				if (Meshmap_iter->second.find(num) == Meshmap_iter->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strEffectStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strEffectEndTime);
					TempEventValue->m_dLoopTime = _wtof(m_strEffectLoopTime);
					TempEventValue->m_dAniRatio = _wtof(m_strEffectAniRatio);

					map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
					FirstValue.emplace(ANI_EVENT_TYPE::EFFECT, TempEventValue);
					Meshmap_iter->second.emplace(num, FirstValue);
					break;
				}
				else
				{
					for (auto& EventType_iter : AniIdx_iter.second)
					{
						if (Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT) == Meshmap_iter->second.find(num)->second.end())
						{
							ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
							TempEventValue->m_dStartTime = _wtof(m_strEffectStartTime);
							TempEventValue->m_dEndTime = _wtof(m_strEffectEndTime);
							TempEventValue->m_dLoopTime = _wtof(m_strEffectLoopTime);
							TempEventValue->m_dAniRatio = _wtof(m_strEffectAniRatio);

							Meshmap_iter->second.find(num)->second.emplace(ANI_EVENT_TYPE::EFFECT, TempEventValue);
							break;
						}
						else
						{
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dStartTime = _wtof(m_strEffectStartTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dEndTime = _wtof(m_strEffectEndTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dLoopTime = _wtof(m_strEffectLoopTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dAniRatio = _wtof(m_strEffectAniRatio);
							break;
						}
					}
					break;
				}
			}
		}
	}

	if (TRUE == m_ButtonSound.GetCheck())
	{
		auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

		if (Meshmap_iter == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strSoundStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strSoundEndTime);
			TempEventValue->m_dLoopTime = _wtof(m_strSoundLoopTime);
			TempEventValue->m_dAniRatio = _wtof(m_strSoundAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::SOUND, TempEventValue);

			_uint num = _ttoi(m_strAniIndex);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(num, FirstValue);

			for (auto& list_iter : m_listMeshName)
			{
				if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
				{
					m_mapmapmapAniEvent.emplace(list_iter, SecondValue);
					break;
				}
			}
		}
		else
		{
			_uint num = _ttoi(m_strAniIndex);

			for (auto& AniIdx_iter : Meshmap_iter->second)
			{
				if (Meshmap_iter->second.find(num) == Meshmap_iter->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strSoundStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strSoundEndTime);
					TempEventValue->m_dLoopTime = _wtof(m_strSoundLoopTime);
					TempEventValue->m_dAniRatio = _wtof(m_strSoundAniRatio);

					map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
					FirstValue.emplace(ANI_EVENT_TYPE::SOUND, TempEventValue);
					Meshmap_iter->second.emplace(num, FirstValue);
					break;
				}
				else //해당 애니인덱스가 있다면
				{
					for (auto& EventType_iter : AniIdx_iter.second)
					{
						if (Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND) == Meshmap_iter->second.find(num)->second.end())
						{
							ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
							TempEventValue->m_dStartTime = _wtof(m_strSoundStartTime);
							TempEventValue->m_dEndTime = _wtof(m_strSoundEndTime);
							TempEventValue->m_dLoopTime = _wtof(m_strSoundLoopTime);
							TempEventValue->m_dAniRatio = _wtof(m_strSoundAniRatio);

							Meshmap_iter->second.find(num)->second.emplace(ANI_EVENT_TYPE::SOUND, TempEventValue);
							break;
						}
						else
						{
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dStartTime = _wtof(m_strSoundStartTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dEndTime = _wtof(m_strSoundEndTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dLoopTime = _wtof(m_strSoundLoopTime);
							Meshmap_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dAniRatio = _wtof(m_strSoundAniRatio);
							break;
						}
					}
					break;
				}
			}
		}
	}

	if (TRUE == m_ButtonCollision.GetCheck())
	{
		auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

		if (Meshmap_iter == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strCollisionStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strCollisionEndTime);
			TempEventValue->m_dRange = _wtof(m_strCollisionRange);
			TempEventValue->m_dAniRatio = _wtof(m_strCollisionAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::COLLISION, TempEventValue);

			_uint num = _ttoi(m_strAniIndex);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(num, FirstValue);

			for (auto& list_iter : m_listMeshName)
			{
				if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
				{
					m_mapmapmapAniEvent.emplace(list_iter, SecondValue);
					break;
				}
			}
		}
		else
		{
			_uint num = _ttoi(m_strAniIndex);

			for (auto& IndexTypeValue_iter : Meshmap_iter->second)
			{
				if (Meshmap_iter->second.find(num) == Meshmap_iter->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strCollisionStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strCollisionEndTime);
					TempEventValue->m_dRange = _wtof(m_strCollisionRange);
					TempEventValue->m_dAniRatio = _wtof(m_strCollisionAniRatio);

					map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
					FirstValue.emplace(ANI_EVENT_TYPE::COLLISION, TempEventValue);
					Meshmap_iter->second.emplace(num, FirstValue);
					break;
				}
				else
				{
					for (auto& TypeValue_iter : IndexTypeValue_iter.second)
					{
						if (IndexTypeValue_iter.second.find(ANI_EVENT_TYPE::COLLISION) == IndexTypeValue_iter.second.end())
						{
							ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
							TempEventValue->m_dStartTime = _wtof(m_strCollisionStartTime);
							TempEventValue->m_dEndTime = _wtof(m_strCollisionEndTime);
							TempEventValue->m_dRange = _wtof(m_strCollisionRange);
							TempEventValue->m_dAniRatio = _wtof(m_strCollisionAniRatio);

							IndexTypeValue_iter.second.emplace(ANI_EVENT_TYPE::COLLISION, TempEventValue);
							break;
						}
						else
						{
							IndexTypeValue_iter.second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dStartTime = _wtof(m_strCollisionStartTime);
							IndexTypeValue_iter.second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dEndTime = _wtof(m_strCollisionEndTime);
							IndexTypeValue_iter.second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dRange = _wtof(m_strCollisionRange);
							IndexTypeValue_iter.second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dAniRatio = _wtof(m_strCollisionAniRatio);
							break;
						}
					}
				}
			}
		}
	}

	Seting_TreeCtrlAniEvent();

	return;
}

void CMonsterTool::Save_AniEvent()
{
	CFileDialog Dlg(FALSE, L".txt", L"제목없음.txt",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Text Files(*.txt)|*.txt||", this);

	_tchar szCurrentPath[MAX_STR] = L"";
	::GetCurrentDirectory(MAX_STR, szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\MonsterData");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal())
	{
		wofstream wcharFileWrite;
		wcharFileWrite.open(Dlg.GetPathName()); //해당 파일 열기, 모드 바이너리로 하고 싶다면 wifstream::binary

		_tchar FullData[MAX_STR];
		_tchar szEventType[MAX_STR];

		if (wcharFileWrite.is_open()) //텍스트 파일을 연다면
		{
			for (auto& MeshName_iter : m_mapmapmapAniEvent)
			{
				for (auto& AniIndex_iter : MeshName_iter.second)
				{
					for (auto& AniEventType_iter : AniIndex_iter.second)
					{
						// 메쉬이름 애니인덱스 이벤트종류 이벤트값
						if (AniIndex_iter.first == ANI_EVENT_TYPE::MOVE) lstrcpy(szEventType, L"MOVE");
						if (AniIndex_iter.first == ANI_EVENT_TYPE::EFFECT) lstrcpy(szEventType, L"EFFECT");
						if (AniIndex_iter.first == ANI_EVENT_TYPE::SOUND) lstrcpy(szEventType, L"SOUND");
						if (AniIndex_iter.first == ANI_EVENT_TYPE::COLLISION) lstrcpy(szEventType, L"COLLISION");
						swprintf_s<MAX_STR>(FullData, L"%s|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f",
							MeshName_iter.first, AniIndex_iter.first, szEventType,
							AniEventType_iter.second->m_dStartTime, AniEventType_iter.second->m_dEndTime,
							AniEventType_iter.second->m_dLoopTime, AniEventType_iter.second->m_dRange,
							AniEventType_iter.second->m_dMoveSpeed, AniEventType_iter.second->m_dMoveAccel,
							AniEventType_iter.second->m_dAniRatio );
							wcharFileWrite << FullData << endl;
					}
				}
			}
		}
		else //열리지 않았다면
			::AfxMessageBox(L"ObjectTool Object Save Failed");

		wcharFileWrite.close();
	}

	return;
}

/////////////////////////////////////////콤보 구현 관련 함수
void CMonsterTool::Add_AniCombo()
{
	UpdateData(TRUE);

	auto& Meshmap_iter = find_if(m_mapmapmapAniCombo.begin(), m_mapmapmapAniCombo.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

	_uint num = _ttoi(m_strAniComboNumber);
	_uint idx = _ttoi(m_strAniIndex);
	_float Ratio = _float(_ttof(m_strAniComboRatio));

	//해당 map에 몬스터가 없을 경우
	if (Meshmap_iter == m_mapmapmapAniCombo.end())
	{
		//애니 인덱스, 비율
		map<_uint, _float> FirstValue;
		FirstValue.emplace(idx, Ratio);

		//콤보 번호, 애니인덱스, 비율
		map<_uint, map<_uint, _float>> SecondValue;
		SecondValue.emplace(num, FirstValue);

		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				//해당 자료를 컨테이너에 저장
				m_mapmapmapAniCombo.emplace(list_iter, SecondValue);
				break;
			}
		}

		Seting_TreeCtrlAniCombo();
	}
	else //해당 map에 몬스터가 있을 경우
	{
		//해당 콤보번호 검색
		for (auto& ComboNumber_iter : Meshmap_iter->second)
		{
			auto& ComboNumFind_iter = Meshmap_iter->second.find(num);

			//해당 콤보번호가 없을 경우 (신규추가)
			if (ComboNumFind_iter == Meshmap_iter->second.end())
			{
				map<_uint, _float> FirstValue;
				FirstValue.emplace(idx, Ratio);

				map<_uint, map<_uint, _float>> SecondValue;
				SecondValue.emplace(num, FirstValue);

				for (auto& list_iter : m_listMeshName)
				{
					if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
					{
						m_mapmapmapAniCombo.emplace(list_iter, SecondValue);
						break;
					}
				}
			}
			else //해당 콤보번호가 있을 경우(수정)
			{
				for (auto& AniIndex_iter : ComboNumber_iter.second)
				{
					auto& AniIndexFind_iter = ComboNumber_iter.second.find(idx);

					//해당 인덱스가 없을 경우(신규 추가)
					if (AniIndexFind_iter == ComboNumber_iter.second.end())
					{
						ComboNumber_iter.second.emplace(idx, Ratio);
						break;
					}
					else //해당 인덱스가 있을경우(수정)
					{
						AniIndex_iter.second = Ratio;
						break;
					}
				}
				break;
			}
		}
		Seting_TreeCtrlAniCombo();
	}

	UpdateData(FALSE);
}

void CMonsterTool::Play_AniCombo()
{
}

void CMonsterTool::Save_AniCombo()
{
}

void CMonsterTool::Load_AniCombo()
{
}
