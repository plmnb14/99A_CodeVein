#include "stdafx.h"
#include "afxdialogex.h"
#include "Tool.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "SetingView.h"
#include "MonsterTool.h"
#include "AniCtrl.h"
#include "TestMonster.h"
#include "TestWeapon.h"

IMPLEMENT_DYNAMIC(CMonsterTool, CDialog)

USING(Engine)

CMonsterTool::CMonsterTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MONSTERTOOL, pParent), m_strObjPosX(_T("")), m_strObjPosY(_T("")), m_strObjPosZ(_T(""))
	, m_strMeshName(_T("")), m_strBoneName(_T("")), m_strAniName(_T("")), m_strAniIndex(_T("")), m_strAniTime(_T("")), m_strAniPlayMul(_T("")), m_strAniFullTime(_T(""))
	, m_strEffectStartTime(_T("")), m_strEffectEndTime(_T("")), m_strEffectLoopTime(_T("")), m_strEffectAniRatio(_T(""))
	, m_strMoveStartTime(_T("")), m_strMoveEndTime(_T("")), m_strMoveLoopTime(_T("")), m_strMoveSpeed(_T("")), m_strMoveAccel(_T("")), m_strMoveAniRatio(_T(""))
	, m_strSoundStartTime(_T("")), m_strSoundEndTime(_T("")), m_strSoundLoopTime(_T("")), m_strSoundAniRatio(_T(""))
	, m_strCollisionStartTime(_T("")), m_strCollisionEndTime(_T("")), m_strCollisionRange(_T("")), m_strCollisionAniRatio(_T(""))
	, m_strPlayAniRatio(_T("")), m_strAniComboRatio(_T("")), m_strAniComboNumber(_T(""))
	, m_strAttachName(_T("")), m_strRadius(_T("")), m_strWpHeight(_T(""))
	, m_strWeaponName(_T(""))
{
}

CMonsterTool::~CMonsterTool()
{
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

	for (auto& list_iter : m_listMeshName)
		Safe_Delete(list_iter);

	m_listMeshName.clear();

	for (auto& list_iter : m_listWeaponName)
		Safe_Delete(list_iter);

	m_listWeaponName.clear();

}

void CMonsterTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_ObjPosX, m_strObjPosX);
	DDX_Text(pDX, IDC_ObjPosY, m_strObjPosY);
	DDX_Text(pDX, IDC_ObjPosZ, m_strObjPosZ);

	DDX_Control(pDX, IDC_MESHLIST, m_ListBoxMesh);
	DDX_Control(pDX, IDC_ANILIST, m_ListBoxAni);
	DDX_Control(pDX, IDC_BONELIST, m_ListBoxBone);
	DDX_Control(pDX, IDC_ANICOMBOTREE, m_TreeCtrlAniCombo);
	DDX_Control(pDX, IDC_ANIEVENTTREE, m_TreeCtrlAniEvent);
	DDX_Control(pDX, IDC_WPCOMBOBOX, m_ComboBoxWeapon);

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

	DDX_Text(pDX, IDC_ATTACHNAME, m_strAttachName);
	DDX_Text(pDX, IDC_WPRADIUS, m_strRadius);
	DDX_Text(pDX, IDC_WPHEIGHT, m_strWpHeight);
	DDX_Text(pDX, IDC_WPNAME, m_strWeaponName);
}

BOOL CMonsterTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	Seting_ListBoxMesh();
	Seting_ComboBoxWeapon();

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
	ON_BN_CLICKED(IDC_ANIEVENTLOAD, &CMonsterTool::Load_AniEvent)

	ON_BN_CLICKED(IDC_ADDANICOMBO, &CMonsterTool::Add_AniCombo)
	ON_BN_CLICKED(IDC_ANiCOMBOSAVE, &CMonsterTool::Save_AniCombo)
	ON_BN_CLICKED(IDC_ANiCOMBOLOAD, &CMonsterTool::Load_AniCombo)
	ON_BN_CLICKED(IDC_PLAYANICOMBO, &CMonsterTool::Play_AniCombo)
	ON_BN_CLICKED(IDC_WPCREATE, &CMonsterTool::Create_Weapon)
	ON_BN_CLICKED(IDC_WPATTACH, &CMonsterTool::Attach_Weapon)
	ON_EN_CHANGE(IDC_WPRADIUS, &CMonsterTool::Change_Radius)
	ON_EN_CHANGE(IDC_WPHEIGHT, &CMonsterTool::Change_Height)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RADIUSSPIN, &CMonsterTool::Spin_Radius)
	ON_NOTIFY(UDN_DELTAPOS, IDC_HEIGHTSPIN, &CMonsterTool::Spin_Height)
END_MESSAGE_MAP()

void CMonsterTool::Update(const _float & fTimeDelta)
{
	if(nullptr != m_pMonster)
	{
		UpdateData(TRUE);

		m_pMonster->Update_GameObject(fTimeDelta);
		m_strAniFullTime.Format(L"%.3f", TARGET_TO_D_MESH(m_pMonster)->Get_AnimationFullTime());
		m_strAniTime.Format(L"%.3f", TARGET_TO_D_MESH(m_pMonster)->Get_TrackInfo().Position);

		_double NowAniTime = _wtof(m_strAniTime);
		_double MaxAniTime = _wtof(m_strAniFullTime);
		
		_int x = _int(NowAniTime / MaxAniTime);
		
		m_strAniTime.Format( L"%.2f", NowAniTime - (MaxAniTime * x) );
		m_strPlayAniRatio.Format(L"%.3f", (NowAniTime / MaxAniTime) - x);

		m_strObjPosX.Format(L"%.3f", TARGET_TO_TRANS(m_pMonster)->Get_Pos().x);
		m_strObjPosY.Format(L"%.3f", TARGET_TO_TRANS(m_pMonster)->Get_Pos().y);
		m_strObjPosZ.Format(L"%.3f", TARGET_TO_TRANS(m_pMonster)->Get_Pos().z);
		
		UpdateData(FALSE);
	}

	IF_NOT_NULL(m_pWeapon) m_pWeapon->Update_GameObject(fTimeDelta);

	return;
}

void CMonsterTool::LateUpdate(const _float & fTimeDelta)
{
	UpdateData(TRUE);

	IF_NOT_NULL(m_pMonster) m_pMonster->Late_Update_GameObject(fTimeDelta);

	IF_NOT_NULL(m_pWeapon) m_pWeapon->Late_Update_GameObject(fTimeDelta);

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
	IF_NOT_NULL(m_pMonster)
		m_pMonster->Render_GameObject();
	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Render_GameObject();

	return;
}

void CMonsterTool::Release()
{
	IF_NOT_NULL(m_pMonster)
		Safe_Release(m_pMonster);

	IF_NOT_NULL(m_pWeapon)
		Safe_Release(m_pWeapon);
}

void CMonsterTool::Seting_ComboBoxWeapon()
{
	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView* pToolView = static_cast<CToolView*>(pFrameWnd->m_MainSplitter.GetPane(0, 0));

	m_ComboBoxWeapon.ResetContent();

	list<Engine::MESH_INFO*> listTemp = pToolView->Get_Weaponlist();
	
	for (auto& list_iter : listTemp)
	{
		CString temp(list_iter->wstrStateKey.c_str());

		_tchar* szTemp = new _tchar[STR_128];
		lstrcpy(szTemp, temp);
		m_listWeaponName.push_back(szTemp);

		m_ComboBoxWeapon.AddString(temp);
	}

	return;
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

		_tchar* szTemp = new _tchar[STR_128];
		lstrcpy(szTemp, temp);
		m_listMeshName.push_back(szTemp);

		m_ListBoxMesh.AddString(temp);
	}

	return;
}

void CMonsterTool::Seting_ListBoxAni()
{
	UpdateData(TRUE);

	m_ListBoxAni.ResetContent(); //모두 지우기

	if(nullptr != m_pMonster)
	{
		_tchar szPath[STR_128];
		_tchar szBuff[STR_128];
		_tchar FullData[STR_128];
		_tchar Num[STR_128];

		_tchar* szRootName;

		CAniCtrl* pAniCtrl = TARGET_TO_D_MESH(m_pMonster)->Get_AniCtrl();

		LPD3DXANIMATIONSET	pAS = nullptr;
		_uint iCount = pAniCtrl->Get_AniCtrl()->GetMaxNumAnimationSets();

		wofstream szAnimationListWrite;
		lstrcat(lstrcpy(szPath, L"../../Data/MonsterData/"), lstrcat(lstrcpy(szBuff, m_strMeshName),L"_AniList.txt"));
		szAnimationListWrite.open(szPath);

		for (_uint i = 0; i < iCount; ++i)
		{
			pAniCtrl->Get_AniCtrl()->GetAnimationSet(i, &pAS);

			const char* charName = pAS->GetName();
			_uint strSize = MultiByteToWideChar(CP_ACP, 0, charName, -1, NULL, NULL);
			szRootName = new _tchar[strSize];
			MultiByteToWideChar(CP_ACP, 0, charName, _int(strlen(charName) + 1), szRootName, strSize);
			
			_itow_s(i, Num, 10);
			swprintf_s<STR_128>(FullData, L"%s == %s", Num, szRootName);
			szAnimationListWrite << FullData << endl;

			m_ListBoxAni.AddString(szRootName);

			Safe_Delete_Array(szRootName);
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Seting_ListBoxBone()
{
	UpdateData(TRUE);

	m_strAttachName = L"";
	m_ListBoxBone.ResetContent();

	if (nullptr != m_pMonster)
	{
		_tchar* szBoneName;

		for (auto vector_iter : TARGET_TO_D_MESH(m_pMonster)->Get_MeshContainer())
		{
			for (DWORD i = 0; i < vector_iter->dwNumBones; ++i)
			{
				const char*	pBoneName = vector_iter->pSkinInfo->GetBoneName(i);
				DWORD strSize = MultiByteToWideChar(CP_ACP, 0, pBoneName, -1, NULL, NULL);
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

	if (nullptr != m_pMonster)
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

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::MOVE)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"EFFECT", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::EFFECT)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"SOUND", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::SOUND)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
		if (Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION) != Mesh_iter->second.find(num)->second.end())
		{
			Root = m_TreeCtrlAniEvent.InsertItem(L"COLLISION", 0, 0, TVI_ROOT, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dStartTime);
			m_TreeCtrlAniEvent.InsertItem(L"Start " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dEndTime);
			m_TreeCtrlAniEvent.InsertItem(L"End " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dLoopTime);
			m_TreeCtrlAniEvent.InsertItem(L"Loop " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dRange);
			m_TreeCtrlAniEvent.InsertItem(L"Range " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dMoveSpeed);
			m_TreeCtrlAniEvent.InsertItem(L"MoveSpeed " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dMoveAccel);
			m_TreeCtrlAniEvent.InsertItem(L"MoveAccel " + temp, 0, 0, Root, TVI_LAST);

			temp.Format(L"%.3f", Mesh_iter->second.find(num)->second.find(ANI_EVENT_TYPE::COLLISION)->second->m_dAniRatio);
			m_TreeCtrlAniEvent.InsertItem(L"AniRatio " + temp, 0, 0, Root, TVI_LAST);
		}
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Seting_TreeCtrlAniCombo()
{
	UpdateData(TRUE);

	if(nullptr != m_pMonster)
	{
		m_TreeCtrlAniCombo.DeleteAllItems();

		CString strTempComboNumber, strTempAniIndex, strTempAniRatio, strTempAniName;
		HTREEITEM Root;

		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				auto& Mesh_Finder = find_if(m_STLAniCombo.begin(), m_STLAniCombo.end(), CTag_Finder(list_iter));

				if (Mesh_Finder == m_STLAniCombo.end())
					return;

				for (auto& ComboNumber_iter : Mesh_Finder->second) //map
				{
					strTempComboNumber.Format(L"%d", ComboNumber_iter.first);
					Root = m_TreeCtrlAniCombo.InsertItem(strTempComboNumber, 0, 0, TVI_ROOT, TVI_LAST);

					for (auto& AniIdx_iter : ComboNumber_iter.second) //vector
					{
						strTempAniIndex.Format(L"%d", AniIdx_iter.first);
						strTempAniRatio.Format(L"%.3f", AniIdx_iter.second);
						m_ListBoxAni.GetText(AniIdx_iter.first, strTempAniName);
						m_TreeCtrlAniCombo.InsertItem(strTempAniIndex + L"/" + strTempAniRatio + L"/ " + strTempAniName, 0, 0, Root, TVI_LAST);
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

	if (nullptr == m_pMonster)
	{
		if (nullptr != m_pWeapon)
			Safe_Release(m_pWeapon);

		UpdateData(TRUE);
		m_ListBoxMesh.GetText(m_ListBoxMesh.GetCurSel(), m_strMeshName);
		UpdateData(FALSE);

		_tchar szTemp[STR_128];
		lstrcpy(szTemp, m_strMeshName);
		m_pMonster = CTestMonster::Create(pToolView->Get_Device(), szTemp);

		Seting_ListBoxAni();
		Seting_ListBoxBone();
		Seting_TreeCtrlAniCombo();

		m_TreeCtrlAniEvent.DeleteAllItems();
	}
	else
	{
		Safe_Release(m_pMonster);

		if (nullptr != m_pWeapon)
			Safe_Release(m_pWeapon);

		UpdateData(TRUE);
		m_ListBoxMesh.GetText(m_ListBoxMesh.GetCurSel(), m_strMeshName);
		UpdateData(FALSE);

		_tchar szTemp[STR_128];
		lstrcpy(szTemp, m_strMeshName);
		m_pMonster = CTestMonster::Create(pToolView->Get_Device(), szTemp);

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
		if (nullptr != m_pMonster)
		{
			m_ListBoxBone.GetText(m_ListBoxBone.GetCurSel(), m_strBoneName);
			m_ListBoxBone.GetText(m_ListBoxBone.GetCurSel(), m_strAttachName);
		}
	}
	UpdateData(FALSE);

	return;
}

void CMonsterTool::LDBClick_AniList()
{
	UpdateData(TRUE);

	if (LB_ERR != m_ListBoxAni.GetCurSel())
	{
		if(nullptr != m_pMonster)
		{
			UpdateData(TRUE);
			m_ListBoxAni.GetText(m_ListBoxAni.GetCurSel(), m_strAniName);
			m_strAniIndex.Format(L"%d", m_ListBoxAni.GetCurSel());
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
	CString strSelectedItemName = m_TreeCtrlAniCombo.GetItemText(m_TreeCtrlAniCombo.GetSelectedItem()); //선택된 노드 이름

	_tchar szSelectedItemName[STR_128];
	lstrcpy(szSelectedItemName, strSelectedItemName);

	if (0 > strSelectedItemName.Find(L"/")) //해당 문자가 없는 경우, 부모인 경우
	{
		_uint TempComboNumber= _ttoi(strSelectedItemName);

		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				auto& Mesh_Finder = find_if(m_STLAniCombo.begin(), m_STLAniCombo.end(), CTag_Finder(list_iter));

				if (Mesh_Finder == m_STLAniCombo.end())
					break;

				auto& ComboNumber_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempComboNumber));

				if (ComboNumber_Finder == Mesh_Finder->second.end())
					break;
				else
					ComboNumber_Finder->second.clear();

				if (ComboNumber_Finder->second.empty())
				{
					Mesh_Finder->second.erase(TempComboNumber);
					break;
				}
			}
		}

	}
	else //자식인 경우
	{
		CString strAniIndex = strSelectedItemName.Left(strSelectedItemName.Find(L'/')); //인덱스 번호
		CString strComboNumber = m_TreeCtrlAniCombo.GetItemText(
								m_TreeCtrlAniCombo.GetNextItem(
								m_TreeCtrlAniCombo.GetNextItem(NULL, TVGN_CARET), TVGN_PARENT));
		
		_uint TempAniIndex = _ttoi(strAniIndex);
		_uint TempComboNumber = _ttoi(strComboNumber);

		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				auto& Mesh_Finder = find_if(m_STLAniCombo.begin(), m_STLAniCombo.end(), CTag_Finder(list_iter));

				if (Mesh_Finder == m_STLAniCombo.end())
					break;

				auto& ComboNumber_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempComboNumber));

				if (ComboNumber_Finder == Mesh_Finder->second.end())
					break;

				//벡터에서 인덱스 번호를 검색하기
				auto& AniIndex_Finder = find_if(ComboNumber_Finder->second.begin(), ComboNumber_Finder->second.end(), Cuint_Finder(TempAniIndex));

				if (AniIndex_Finder == ComboNumber_Finder->second.end())
					break;

				ComboNumber_Finder->second.erase(AniIndex_Finder);

				if (ComboNumber_Finder->second.empty())
				{
					Mesh_Finder->second.erase(TempComboNumber);
					break;
				}

			}
		}
	}

	Seting_TreeCtrlAniCombo();

	*pResult = 0;
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

	if (LB_ERR != m_ListBoxAni.GetCurSel())
	{
		if(nullptr != m_pMonster)
		{
			m_pMonster->Check_ComboPlayBtn(false);

			m_strAniFullTime.Format(L"%.3f", TARGET_TO_D_MESH(m_pMonster)->Get_AnimationFullTime());
			m_strAniTime.Format(L"%.3f", TARGET_TO_D_MESH(m_pMonster)->Get_TrackInfo().Position);
			TARGET_TO_D_MESH(m_pMonster)->SetUp_Animation(_ttoi(m_strAniIndex));
			m_pMonster->Set_AniSpeed((_float)_wtof(m_strAniPlayMul));
		}
	}

	UpdateData(FALSE);
}

void CMonsterTool::Stop_Ani()
{
	UpdateData(TRUE);

	if(nullptr != m_pMonster)
	{
		m_pMonster->Set_AniSpeed(0.f);
		m_strAniFullTime.Format(L"%.3f", TARGET_TO_D_MESH(m_pMonster)->Get_AnimationFullTime());
		m_strAniTime.Format(L"%.3f", TARGET_TO_D_MESH(m_pMonster)->Get_TrackInfo().Position);
	}

	UpdateData(FALSE);
}

void CMonsterTool::Add_AniEvent()
{
	if (TRUE == m_ButtonMove.GetCheck())
	{
		auto& Mesh_Finder = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));
		_uint TempAniIndex = _ttoi(m_strAniIndex);

		//해당 몬스터가 없다면
		if (Mesh_Finder == m_mapmapmapAniEvent.end())
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

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(TempAniIndex, FirstValue);

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
			auto& AniIndex_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempAniIndex));
			
			//해당 애니 인덱스가 없다면
			if (AniIndex_Finder == Mesh_Finder->second.end())
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
				Mesh_Finder->second.emplace(TempAniIndex, FirstValue);
			}
			else //해당 애니 인덱스가 있다면
			{
				auto& EventType_Finder = find_if(AniIndex_Finder->second.begin(), AniIndex_Finder->second.end(), Cuint_Finder(ANI_EVENT_TYPE::MOVE));

				//해당 이벤트가 없다면
				if (EventType_Finder == AniIndex_Finder->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strMoveStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strMoveEndTime);
					TempEventValue->m_dLoopTime = _wtof(m_strMoveLoopTime);
					TempEventValue->m_dMoveSpeed = _wtof(m_strMoveSpeed);
					TempEventValue->m_dMoveAccel = _wtof(m_strMoveAccel);
					TempEventValue->m_dAniRatio = _wtof(m_strMoveAniRatio);

					AniIndex_Finder->second.emplace(ANI_EVENT_TYPE::MOVE, TempEventValue);
				}
				else
				{
					EventType_Finder->second->m_dStartTime = _wtof(m_strMoveStartTime);
					EventType_Finder->second->m_dEndTime = _wtof(m_strMoveEndTime);
					EventType_Finder->second->m_dLoopTime = _wtof(m_strMoveLoopTime);
					EventType_Finder->second->m_dMoveSpeed = _wtof(m_strMoveSpeed);
					EventType_Finder->second->m_dMoveAccel = _wtof(m_strMoveAccel);
					EventType_Finder->second->m_dAniRatio = _wtof(m_strMoveAniRatio);
				}
			}
		}
	}

	if (TRUE == m_ButtonEffect.GetCheck())
	{
		auto& Mesh_Finder = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));
		_uint TempAniIndex = _ttoi(m_strAniIndex);

		if (Mesh_Finder == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strEffectStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strEffectEndTime);
			TempEventValue->m_dLoopTime = _wtof(m_strEffectLoopTime);
			TempEventValue->m_dAniRatio = _wtof(m_strEffectAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::EFFECT, TempEventValue);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(TempAniIndex, FirstValue);

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
			auto& AniIndex_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempAniIndex));

			if (AniIndex_Finder == Mesh_Finder->second.end())
			{
				ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
				TempEventValue->m_dStartTime = _wtof(m_strEffectStartTime);
				TempEventValue->m_dEndTime = _wtof(m_strEffectEndTime);
				TempEventValue->m_dLoopTime = _wtof(m_strEffectLoopTime);
				TempEventValue->m_dAniRatio = _wtof(m_strEffectAniRatio);

				map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
				FirstValue.emplace(ANI_EVENT_TYPE::EFFECT, TempEventValue);

				Mesh_Finder->second.emplace(TempAniIndex, FirstValue);
			}
			else
			{
				auto& EventType_Finder = find_if(AniIndex_Finder->second.begin(), AniIndex_Finder->second.end(), Cuint_Finder(ANI_EVENT_TYPE::EFFECT));

				if (EventType_Finder == AniIndex_Finder->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strEffectStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strEffectEndTime);
					TempEventValue->m_dLoopTime = _wtof(m_strEffectLoopTime);
					TempEventValue->m_dAniRatio = _wtof(m_strEffectAniRatio);

					AniIndex_Finder->second.emplace(ANI_EVENT_TYPE::EFFECT, TempEventValue);
				}
				else
				{
					EventType_Finder->second->m_dStartTime = _wtof(m_strEffectStartTime);
					EventType_Finder->second->m_dEndTime = _wtof(m_strEffectEndTime);
					EventType_Finder->second->m_dLoopTime = _wtof(m_strEffectLoopTime);
					EventType_Finder->second->m_dAniRatio = _wtof(m_strEffectAniRatio);
				}
			}
		}
	}

	if (TRUE == m_ButtonSound.GetCheck())
	{
		auto& Mesh_Finder = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));
		_uint TempAniIndex = _ttoi(m_strAniIndex);

		if (Mesh_Finder == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strSoundStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strSoundEndTime);
			TempEventValue->m_dLoopTime = _wtof(m_strSoundLoopTime);
			TempEventValue->m_dAniRatio = _wtof(m_strSoundAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::SOUND, TempEventValue);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(TempAniIndex, FirstValue);

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
			auto& AniIndex_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempAniIndex));

			if (AniIndex_Finder == Mesh_Finder->second.end())
			{
				ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
				TempEventValue->m_dStartTime = _wtof(m_strSoundStartTime);
				TempEventValue->m_dEndTime = _wtof(m_strSoundEndTime);
				TempEventValue->m_dLoopTime = _wtof(m_strSoundLoopTime);
				TempEventValue->m_dAniRatio = _wtof(m_strSoundAniRatio);

				map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
				FirstValue.emplace(ANI_EVENT_TYPE::SOUND, TempEventValue);

				Mesh_Finder->second.emplace(TempAniIndex, FirstValue);
			}
			else
			{
				auto& EventType_Finder = find_if(AniIndex_Finder->second.begin(), AniIndex_Finder->second.end(), Cuint_Finder(ANI_EVENT_TYPE::SOUND));

				if (EventType_Finder == AniIndex_Finder->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strSoundStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strSoundEndTime);
					TempEventValue->m_dLoopTime = _wtof(m_strSoundLoopTime);
					TempEventValue->m_dAniRatio = _wtof(m_strSoundAniRatio);

					AniIndex_Finder->second.emplace(ANI_EVENT_TYPE::SOUND, TempEventValue);
				}
				else
				{
					EventType_Finder->second->m_dStartTime = _wtof(m_strSoundStartTime);
					EventType_Finder->second->m_dEndTime = _wtof(m_strSoundEndTime);
					EventType_Finder->second->m_dLoopTime = _wtof(m_strSoundLoopTime);
					EventType_Finder->second->m_dAniRatio = _wtof(m_strSoundAniRatio);
				}
			}
		}
	}

	if (TRUE == m_ButtonCollision.GetCheck())
	{
		auto& Mesh_Finder = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));
		_uint TempAniIndex = _ttoi(m_strAniIndex);

		if (Mesh_Finder == m_mapmapmapAniEvent.end())
		{
			ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
			TempEventValue->m_dStartTime = _wtof(m_strCollisionStartTime);
			TempEventValue->m_dEndTime = _wtof(m_strCollisionEndTime);
			TempEventValue->m_dRange = _wtof(m_strCollisionRange);
			TempEventValue->m_dAniRatio = _wtof(m_strCollisionAniRatio);

			map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
			FirstValue.emplace(ANI_EVENT_TYPE::COLLISION, TempEventValue);

			map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
			SecondValue.emplace(TempAniIndex, FirstValue);

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
			auto& AniIndex_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempAniIndex));

			if (AniIndex_Finder == Mesh_Finder->second.end())
			{
				ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
				TempEventValue->m_dStartTime = _wtof(m_strCollisionStartTime);
				TempEventValue->m_dEndTime = _wtof(m_strCollisionEndTime);
				TempEventValue->m_dRange = _wtof(m_strCollisionRange);
				TempEventValue->m_dAniRatio = _wtof(m_strCollisionAniRatio);

				map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
				FirstValue.emplace(ANI_EVENT_TYPE::COLLISION, TempEventValue);

				Mesh_Finder->second.emplace(TempAniIndex, FirstValue);
			}
			else
			{
				auto& EventType_Finder = find_if(AniIndex_Finder->second.begin(), AniIndex_Finder->second.end(), Cuint_Finder(ANI_EVENT_TYPE::COLLISION));

				if (EventType_Finder == AniIndex_Finder->second.end())
				{
					ANI_EVENT_VALUE* TempEventValue = new ANI_EVENT_VALUE;
					TempEventValue->m_dStartTime = _wtof(m_strCollisionStartTime);
					TempEventValue->m_dEndTime = _wtof(m_strCollisionEndTime);
					TempEventValue->m_dRange = _wtof(m_strCollisionRange);
					TempEventValue->m_dAniRatio = _wtof(m_strCollisionAniRatio);

					AniIndex_Finder->second.emplace(ANI_EVENT_TYPE::COLLISION, TempEventValue);
				}
				else
				{
					EventType_Finder->second->m_dStartTime = _wtof(m_strCollisionStartTime);
					EventType_Finder->second->m_dEndTime = _wtof(m_strCollisionEndTime);
					EventType_Finder->second->m_dRange = _wtof(m_strCollisionRange);
					EventType_Finder->second->m_dAniRatio = _wtof(m_strCollisionAniRatio);
				}
			}
		}
	}

	Seting_TreeCtrlAniEvent();

	return;
}

void CMonsterTool::Save_AniEvent()
{
	CFileDialog Dlg(FALSE, L".txt", L"Event.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Text Files(*.txt)|*.txt||", this);

	_tchar szCurrentPath[STR_128] = L"";
	::GetCurrentDirectory(STR_128, szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\MonsterData");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal())
	{
		wofstream wcharFileWrite;
		wcharFileWrite.open(Dlg.GetPathName()); //해당 파일 열기, 모드 바이너리로 하고 싶다면 wifstream::binary

		_tchar FullData[STR_128];
		_tchar szEventType[STR_128];

		if (wcharFileWrite.is_open()) //텍스트 파일을 연다면
		{
			for (auto& MeshName_iter : m_mapmapmapAniEvent)
			{
				for (auto& AniIndex_iter : MeshName_iter.second)
				{
					for (auto& AniEventType_iter : AniIndex_iter.second)
					{
						// 메쉬이름 애니인덱스 이벤트종류 이벤트값
						switch (AniEventType_iter.first)
						{
						case ANI_EVENT_TYPE::MOVE:
							lstrcpy(szEventType, L"MOVE");
							swprintf_s<STR_128>(FullData, L"%s|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f",
								MeshName_iter.first, AniIndex_iter.first, szEventType,
								AniEventType_iter.second->m_dStartTime, AniEventType_iter.second->m_dEndTime,
								AniEventType_iter.second->m_dLoopTime, AniEventType_iter.second->m_dRange,
								AniEventType_iter.second->m_dMoveSpeed, AniEventType_iter.second->m_dMoveAccel,
								AniEventType_iter.second->m_dAniRatio);
							wcharFileWrite << FullData << endl;
							break;
						case ANI_EVENT_TYPE::EFFECT:
							lstrcpy(szEventType, L"EFFECT");
							swprintf_s<STR_128>(FullData, L"%s|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f",
								MeshName_iter.first, AniIndex_iter.first, szEventType,
								AniEventType_iter.second->m_dStartTime, AniEventType_iter.second->m_dEndTime,
								AniEventType_iter.second->m_dLoopTime, AniEventType_iter.second->m_dRange,
								AniEventType_iter.second->m_dMoveSpeed, AniEventType_iter.second->m_dMoveAccel,
								AniEventType_iter.second->m_dAniRatio);
							wcharFileWrite << FullData << endl;
							break;
						case ANI_EVENT_TYPE::SOUND:
							lstrcpy(szEventType, L"SOUND");
							swprintf_s<STR_128>(FullData, L"%s|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f",
								MeshName_iter.first, AniIndex_iter.first, szEventType,
								AniEventType_iter.second->m_dStartTime, AniEventType_iter.second->m_dEndTime,
								AniEventType_iter.second->m_dLoopTime, AniEventType_iter.second->m_dRange,
								AniEventType_iter.second->m_dMoveSpeed, AniEventType_iter.second->m_dMoveAccel,
								AniEventType_iter.second->m_dAniRatio);
							wcharFileWrite << FullData << endl;
							break;
						case ANI_EVENT_TYPE::COLLISION:
							lstrcpy(szEventType, L"COLLISION");
							swprintf_s<STR_128>(FullData, L"%s|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f",
								MeshName_iter.first, AniIndex_iter.first, szEventType,
								AniEventType_iter.second->m_dStartTime, AniEventType_iter.second->m_dEndTime,
								AniEventType_iter.second->m_dLoopTime, AniEventType_iter.second->m_dRange,
								AniEventType_iter.second->m_dMoveSpeed, AniEventType_iter.second->m_dMoveAccel,
								AniEventType_iter.second->m_dAniRatio);
							wcharFileWrite << FullData << endl;
							break;
						}
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

void CMonsterTool::Load_AniEvent()
{
	UpdateData(TRUE);

	CFileDialog	Dlg(TRUE, L".txt", L"Event.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Text Files(*.txt)|*.txt||", this);

	_tchar szCurrentPath[STR_128] = L"";
	::GetCurrentDirectory(STR_128, szCurrentPath);

	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\MonsterData");

	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal())
	{
		wifstream wcharFileRead;
		wcharFileRead.open(Dlg.GetPathName()); //해당 파일 열기

		_tchar szMeshName[STR_128];
		_tchar Number[STR_128];
		ANI_EVENT_TYPE EventType;

		if (!m_mapmapmapAniEvent.empty())
		{
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


		if (wcharFileRead.is_open()) //텍스트 파일을 연다면
		{
			while (true)
			{
				wcharFileRead.getline(szMeshName, STR_128, '|');

				if (!lstrcmp(szMeshName, L""))
					break;

				wcharFileRead.getline(Number, STR_128, '|');
				_uint AniIndexNumber = _wtoi(Number); //인덱스 번호

				wcharFileRead.getline(Number, STR_128, '|');
				//이벤트 종류

				if (!lstrcmp(Number, L"MOVE")) EventType = ANI_EVENT_TYPE::MOVE;
				if (!lstrcmp(Number, L"EFFECT"))  EventType = ANI_EVENT_TYPE::EFFECT;
				if (!lstrcmp(Number, L"SOUND"))  EventType = ANI_EVENT_TYPE::SOUND;
				if (!lstrcmp(Number, L"COLLISION"))  EventType = ANI_EVENT_TYPE::COLLISION;

				wcharFileRead.getline(Number, STR_128, '|'); //시작 값
				_double dStartTime = _wtof(Number);
				
				wcharFileRead.getline(Number, STR_128, '|'); //끝 값
				_double dEndTime = _wtof(Number);

				wcharFileRead.getline(Number, STR_128, '|'); //반복 값
				_double dLoopTime = _wtof(Number);

				wcharFileRead.getline(Number, STR_128, '|'); //반경 값
				_double dRange = _wtof(Number);

				wcharFileRead.getline(Number, STR_128, '|'); //이동속도 값
				_double dMoveSpeed = _wtof(Number);

				wcharFileRead.getline(Number, STR_128, '|'); //이동가속도 값
				_double dMoveAccel = _wtof(Number);

				wcharFileRead.getline(Number, STR_128); //비율 값
				_double dAniRatio = _wtof(Number);

				for (auto& list_iter : m_listMeshName)
				{
					//몬스터가 map에 존재하는지
					auto& Meshmap_iter = find_if(m_mapmapmapAniEvent.begin(), m_mapmapmapAniEvent.end(), CTag_Finder(szMeshName));

					//map에 몬스터가 없다면
					if (Meshmap_iter == m_mapmapmapAniEvent.end())
					{
						ANI_EVENT_VALUE* EventValue = new ANI_EVENT_VALUE;

						EventValue->m_dStartTime = dStartTime;
						EventValue->m_dEndTime = dEndTime;
						EventValue->m_dLoopTime = dLoopTime;
						EventValue->m_dRange = dRange;
						EventValue->m_dMoveSpeed = dMoveSpeed;
						EventValue->m_dMoveAccel = dMoveAccel;
						EventValue->m_dAniRatio = dAniRatio;

						map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
						FirstValue.emplace(EventType, EventValue);
						map <_uint, map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*>> SecondValue;
						SecondValue.emplace(AniIndexNumber, FirstValue);

						m_mapmapmapAniEvent.emplace(list_iter, SecondValue);
					}
					else //map에 몬스터가 있다면
					{
						//해당 인덱스가 존재 하지 않는다면
						if (Meshmap_iter->second.find(AniIndexNumber) == Meshmap_iter->second.end())
						{
							ANI_EVENT_VALUE* EventValue = new ANI_EVENT_VALUE;

							EventValue->m_dStartTime = dStartTime;
							EventValue->m_dEndTime = dEndTime;
							EventValue->m_dLoopTime = dLoopTime;
							EventValue->m_dRange = dRange;
							EventValue->m_dMoveSpeed = dMoveSpeed;
							EventValue->m_dMoveAccel = dMoveAccel;
							EventValue->m_dAniRatio = dAniRatio;

							map<ANI_EVENT_TYPE, ANI_EVENT_VALUE*> FirstValue;
							FirstValue.emplace(EventType, EventValue);

							Meshmap_iter->second.emplace(AniIndexNumber, FirstValue);
						}
						else //해당 인덱스가 존재 한다면
						{
							//해당 이벤트가 존재 하지 않는다면
							if (Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType) == Meshmap_iter->second.find(AniIndexNumber)->second.end())
							{
								ANI_EVENT_VALUE* EventValue = new ANI_EVENT_VALUE;

								EventValue->m_dStartTime = dStartTime;
								EventValue->m_dEndTime = dEndTime;
								EventValue->m_dLoopTime = dLoopTime;
								EventValue->m_dRange = dRange;
								EventValue->m_dMoveSpeed = dMoveSpeed;
								EventValue->m_dMoveAccel = dMoveAccel;
								EventValue->m_dAniRatio = dAniRatio;

								Meshmap_iter->second.find(AniIndexNumber)->second.emplace(EventType, EventValue);
							}
							else //해당 이벤트가존재할경우
							{
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dStartTime = dStartTime;
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dEndTime = dEndTime;
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dLoopTime = dLoopTime;
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dRange = dRange;
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dMoveSpeed = dMoveSpeed;
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dMoveAccel = dMoveAccel;
								Meshmap_iter->second.find(AniIndexNumber)->second.find(EventType)->second->m_dAniRatio = dAniRatio;
							}
						}
					}
				}
			}
		}
		else //열리지 않았다면
			::AfxMessageBox(L"MonsterTool Event Load Failed");

		wcharFileRead.close();
	}

	UpdateData(FALSE);
}

void CMonsterTool::Add_AniCombo()
{
	UpdateData(TRUE);

	auto& Mesh_Finder = find_if(m_STLAniCombo.begin(), m_STLAniCombo.end(), CTag_Finder((_tchar*)(LPCTSTR)m_strMeshName));

	_uint TempComboNumber = _ttoi(m_strAniComboNumber);
	_uint TempAniIdx = _ttoi(m_strAniIndex);
	_float TempAniRatio =(_float)_ttof(m_strAniComboRatio);

	if (Mesh_Finder == m_STLAniCombo.end())
	{
		IdxRatio AniValue;
		AniValue.first = TempAniIdx;
		AniValue.second = TempAniRatio;

		vector<IdxRatio> FirstValue;
		FirstValue.push_back(AniValue);

		map<_uint, vector<IdxRatio>> SecondValue;
		SecondValue.emplace(TempComboNumber, FirstValue);
		
		for (auto& list_iter : m_listMeshName)
		{
			if (0 == lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				m_STLAniCombo.emplace(list_iter, SecondValue);
				break;
			}
		}
	}
	else
	{
		auto& ComboNumber_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempComboNumber));

		if (ComboNumber_Finder == Mesh_Finder->second.end())
		{
			IdxRatio AniValue;
			AniValue.first = TempAniIdx;
			AniValue.second = TempAniRatio;

			vector<IdxRatio> FirstValue;
			FirstValue.push_back(AniValue);

			Mesh_Finder->second.emplace(TempComboNumber, FirstValue);
		}
		else
		{
			auto& AniIndexNumber_Finder = find_if(ComboNumber_Finder->second.begin(), ComboNumber_Finder->second.end(), Cuint_Finder(TempAniIdx));

			if (AniIndexNumber_Finder == ComboNumber_Finder->second.end())
			{
				IdxRatio AniValue;
				AniValue.first = TempAniIdx;
				AniValue.second = TempAniRatio;

				ComboNumber_Finder->second.push_back(AniValue);
			}
			else
			{
				*AniIndexNumber_Finder = IdxRatio(TempAniIdx,TempAniRatio);
			}
		}
	}

	Seting_TreeCtrlAniCombo();

	UpdateData(FALSE);
}

void CMonsterTool::Play_AniCombo()
{
	if (nullptr != m_pMonster)
	{
		m_pMonster->Check_ComboPlayBtn(false);

		_uint TempAniComboNumber = _ttoi(m_strAniComboNumber);

		for(auto& list_iter : m_listMeshName)
		{
			if (!lstrcmp(list_iter, (_tchar*)(LPCTSTR)m_strMeshName))
			{
				auto& Mesh_Finder = find_if(m_STLAniCombo.begin(), m_STLAniCombo.end(), CTag_Finder(list_iter));

				if (Mesh_Finder == m_STLAniCombo.end()) return;

				auto& ComboNumber_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(TempAniComboNumber));

				if (ComboNumber_Finder == Mesh_Finder->second.end()) return;

				m_pMonster->Reset_Combo();

				for (auto& vector_iter : ComboNumber_Finder->second)
				{
					m_pMonster->Set_Combo(vector_iter.first, vector_iter.second);
				}
			}
		}
		m_pMonster->Set_AniSpeed((_float)_wtof(m_strAniPlayMul));

		m_pMonster->Check_ComboPlayBtn(true);
	}

	return;
}

void CMonsterTool::Save_AniCombo()
{
	CFileDialog Dlg(FALSE, L".txt", L"Combo.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Text Files(*.txt)|*.txt||", this);

	_tchar szCurrentPath[STR_128] = L"";
	::GetCurrentDirectory(STR_128, szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\MonsterData");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal())
	{
		wofstream wcharFileWrite;
		wcharFileWrite.open(Dlg.GetPathName()); //해당 파일 열기, 모드 바이너리로 하고 싶다면 wifstream::binary

		_tchar FullData[STR_128];

		if (wcharFileWrite.is_open()) //텍스트 파일을 연다면
		{
			for (auto& MeshName_iter : m_STLAniCombo)
			{
				for (auto& ComboNumber_iter : MeshName_iter.second)
				{
					for (auto& AniIndex_iter : ComboNumber_iter.second)
					{
						//몬스터 종류, 콤보번호, 애니인덱스, 애니재생비율
						swprintf_s<STR_128>(FullData, L"%s|%d|%d|%.2f",
							MeshName_iter.first, ComboNumber_iter.first, AniIndex_iter.first, AniIndex_iter.second);
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

void CMonsterTool::Load_AniCombo()
{
	UpdateData(TRUE);

	CFileDialog	Dlg(TRUE, L".txt", L"Combo.txt", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"Text Files(*.txt)|*.txt||", this);

	_tchar szCurrentPath[STR_128] = L"";
	::GetCurrentDirectory(STR_128, szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\MonsterData");

	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal())
	{
		m_TreeCtrlAniCombo.DeleteAllItems();

		wifstream wcharFileRead;

		wcharFileRead.open(Dlg.GetPathName()); //해당 파일 열기
		_tchar szMeshName[STR_128];
		_tchar Number[STR_128];
		_uint AniComboNumber, AniIndexNumber;
		_float AniRatio;

		if (wcharFileRead.is_open()) //텍스트 파일을 연다면
		{
			while (true) //끝자락에 도달하지 않는다면 계속 반복하라
			{
				wcharFileRead.getline(szMeshName, STR_128, '|');

				if (!lstrcmp(szMeshName, L""))
					break;

				wcharFileRead.getline(Number, STR_128, '|');
				AniComboNumber = _wtoi(Number); //콤보 번호

				wcharFileRead.getline(Number, STR_128, '|');
				AniIndexNumber = _wtoi(Number); //인덱스 번호

				wcharFileRead.getline(Number, STR_128);
				AniRatio = (_float)_wtof(Number); //애니 비율

				for (auto& list_iter : m_listMeshName)
				{
					if (!lstrcmp(list_iter, szMeshName))
					{
						auto& Mesh_Finder = find_if(m_STLAniCombo.begin(), m_STLAniCombo.end(), CTag_Finder(list_iter));

						if (Mesh_Finder == m_STLAniCombo.end())
						{
							vector<IdxRatio> FirstValue;
							FirstValue.emplace_back(IdxRatio(AniIndexNumber, AniRatio));

							map<_uint, vector<IdxRatio>> SecondValue;
							SecondValue.emplace(AniComboNumber, FirstValue);

							m_STLAniCombo.emplace(list_iter, SecondValue);
						}
						else
						{
							auto& ComboNumber_Finder = find_if(Mesh_Finder->second.begin(), Mesh_Finder->second.end(), Cuint_Finder(AniComboNumber));

							if (ComboNumber_Finder == Mesh_Finder->second.end())
							{
								vector<IdxRatio> FirstValue;
								FirstValue.emplace_back(IdxRatio(AniIndexNumber, AniRatio));

								Mesh_Finder->second.emplace(AniComboNumber, FirstValue);
							}
							else
							{
								auto& AniIndex_Finder = find_if(ComboNumber_Finder->second.begin(), ComboNumber_Finder->second.end(), Cuint_Finder(AniIndexNumber));
								
								if (AniIndex_Finder == ComboNumber_Finder->second.end())
								{
									ComboNumber_Finder->second.emplace_back(IdxRatio(AniIndexNumber, AniRatio));
								}
								else
									AniIndex_Finder->second = AniRatio;
							}
						}
					}
				}
			}
		}
		else //열리지 않았다면
			::AfxMessageBox(L"MonsterTool Combo Load Failed");

		Seting_TreeCtrlAniCombo();
		wcharFileRead.close();
	}

	UpdateData(FALSE);
}

void CMonsterTool::Create_Weapon()
{
	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	CToolView* pToolView = static_cast<CToolView*>(pFrameWnd->m_MainSplitter.GetPane(0, 0));

	if (nullptr == m_pWeapon)
	{
		UpdateData(TRUE);

		m_ComboBoxWeapon.GetLBText(m_ComboBoxWeapon.GetCurSel(), m_strWeaponName);

		UpdateData(FALSE);

		_tchar szTemp[STR_128];
		lstrcpy(szTemp, m_strWeaponName);
		m_pWeapon = CTestWeapon::Create(pToolView->Get_Device(), szTemp);
	}
	else
	{
		Safe_Release(m_pWeapon);

		UpdateData(TRUE);

		m_ComboBoxWeapon.GetLBText(m_ComboBoxWeapon.GetCurSel(), m_strWeaponName);

		UpdateData(FALSE);

		_tchar szTemp[STR_128];
		lstrcpy(szTemp, m_strWeaponName);
		m_pWeapon = CTestWeapon::Create(pToolView->Get_Device(), szTemp);
	}

	return;
}

void CMonsterTool::Attach_Weapon()
{
	UpdateData(TRUE);

	if (nullptr != m_pMonster && nullptr != m_pWeapon)
	{
		D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)TARGET_TO_D_MESH(m_pMonster)->Get_BonInfo((CStringA)m_strAttachName);
		m_pWeapon->Change_Matrix(&TARGET_TO_TRANS(m_pMonster)->Get_WorldMat(), &pFrame->CombinedTransformationMatrix);
	}

	UpdateData(FALSE);
	return;
}

void CMonsterTool::Change_Radius()
{
	UpdateData(TRUE);

	if (nullptr != m_pWeapon)
	{
		_float Radius = (_float)_wtof(m_strRadius);
		m_pWeapon->Change_Radius(Radius);
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Change_Height()
{
	UpdateData(TRUE);

	if (nullptr != m_pWeapon)
	{
		_float Height = (_float)_wtof(m_strWpHeight);
		m_pWeapon->Change_Height(Height);
	}

	UpdateData(FALSE);

	return;
}

void CMonsterTool::Spin_Radius(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	_float fBuff = (_float)_wtof(m_strRadius);
	fBuff += -(pNMUpDown->iDelta * 0.1f);
	m_strRadius.Format(L"%.3f", fBuff);
	GetDlgItem(IDC_WPRADIUS)->SetWindowText(m_strRadius);

	UpdateData(FALSE);

	*pResult = 0;
}

void CMonsterTool::Spin_Height(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	_float fBuff = (_float)_wtof(m_strWpHeight);
	fBuff += -(pNMUpDown->iDelta * 0.1f);
	m_strWpHeight.Format(L"%.3f", fBuff);
	GetDlgItem(IDC_WPHEIGHT)->SetWindowText(m_strWpHeight);

	UpdateData(FALSE);

	*pResult = 0;
}
