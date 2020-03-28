// ParticleTab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "ParticleTab.h"

#include "Management.h"
#include "RenderObject.h"
#include "GameObject.h"

// CParticleTab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CParticleTab, CDialog)

CParticleTab::CParticleTab(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PARTICLETAB, pParent)
	, m_EditAnimSpeed(_T("1.0"))
	, m_EditLifeTime(_T("1.0"))
	, m_EditMaxAlpha(_T("1.0"))
	, m_EditMaxCount(_T("0.0"))
	, m_EditSizeX(_T("1.0"))
	, m_EditSizeY(_T("1.0"))
	, m_EditSizeZ(_T("1.0"))
	, m_EditDirX(_T("0.0"))
	, m_EditDirY(_T("0.0"))
	, m_EditDirZ(_T("0.0"))
	, m_EditPosY(_T("0.0"))
	, m_EditPosZ(_T("0.0"))
	, m_EditPosX(_T("0.0"))
	, m_EditRotX(_T("0.0"))
	, m_EditRotY(_T("0.0"))
	, m_EditRotZ(_T("0.0"))
	, m_EditHueStart(_T("0.0"))
	, m_EditContrastStart(_T("0.0"))
	, m_EditBrightnessStart(_T("0.0"))
	, m_EditHueEnd(_T("0.0"))
	, m_EditContrastEnd(_T("0.0"))
	, m_EditBrightnessEnd(_T("0.0"))
	, m_EditParticleCount(_T("1"))
	, m_EditMoveScaleX(_T("0.0"))
	, m_EditMoveScaleY(_T("0.0"))
	, m_EditMoveScaleZ(_T("0.0"))
	, m_EditRandDirX(_T("0.0"))
	, m_EditRandDirY(_T("0.0"))
	, m_EditRandDirZ(_T("0.0"))
	, m_EditMoveSpeed(_T("1.0"))
	, m_EditMoveSpeed_Min(_T("0.0"))
	, m_EditMoveSpeed_Max(_T("0.0"))
	, m_EditRotSpeed(_T("1.0"))
	, m_EditRotSpeed_Min(_T("0.0"))
	, m_EditRotSpeed_Max(_T("0.0"))
	, m_EditAlphaSpeed(_T("1.0"))
	, m_EditAlphaSpeed_Min(_T("0.0"))
	, m_EditAlphaSpeed_Max(_T("0.0"))
	, m_EditCreateDelay(_T("0.1"))
	, m_EditCreateDelay_Min(_T("0.0"))
	, m_EditCreateDelay_Max(_T("0.0"))
	, m_EditSaturationStart(_T("1.0"))
	, m_EditSaturationEnd(_T("1.0"))
	, m_EditFileName(_T(""))
	, m_EditColorIndex(_T("0.0"))
	, m_EditMaskIndex(_T(""))
{

}

CParticleTab::~CParticleTab()
{
	Release();
}

void CParticleTab::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, m_RadioResType[0]);
	DDX_Control(pDX, IDC_RADIO2, m_RadioResType[1]);
	DDX_Control(pDX, IDC_RADIO9, m_RadioBillType[0]);
	DDX_Control(pDX, IDC_RADIO7, m_RadioBillType[1]);
	DDX_Control(pDX, IDC_RADIO8, m_RadioBillType[2]);
	DDX_Control(pDX, IDC_RADIO3, m_RadioMoveType[0]);
	DDX_Control(pDX, IDC_RADIO4, m_RadioMoveType[1]);
	DDX_Control(pDX, IDC_RADIO5, m_RadioMoveType[2]);
	DDX_Control(pDX, IDC_RADIO6, m_RadioMoveType[3]);
	DDX_Control(pDX, IDC_EDIT1, m_EditMeshRes);
	DDX_Control(pDX, IDC_EDIT2, m_EditTextureRes);
	DDX_Control(pDX, IDC_BUTTON1, m_ButtonMeshRes);
	DDX_Control(pDX, IDC_BUTTON2, m_ButtonTextureRes);
	DDX_Text(pDX, IDC_EDIT41, m_EditAnimSpeed);
	DDX_Text(pDX, IDC_EDIT37, m_EditLifeTime);
	DDX_Text(pDX, IDC_EDIT39, m_EditMaxAlpha);
	DDX_Text(pDX, IDC_EDIT38, m_EditMaxCount);
	DDX_Text(pDX, IDC_EDIT4, m_EditSizeX);
	DDX_Text(pDX, IDC_EDIT5, m_EditSizeY);
	DDX_Text(pDX, IDC_EDIT6, m_EditSizeZ);
	DDX_Text(pDX, IDC_EDIT7, m_EditDirX);
	DDX_Text(pDX, IDC_EDIT8, m_EditDirY);
	DDX_Text(pDX, IDC_EDIT9, m_EditDirZ);
	DDX_Text(pDX, IDC_EDIT10, m_EditPosX);
	DDX_Text(pDX, IDC_EDIT11, m_EditPosY);
	DDX_Text(pDX, IDC_EDIT12, m_EditPosZ);
	DDX_Control(pDX, IDC_CHECK2, m_CheckRandPos);
	DDX_Text(pDX, IDC_EDIT19, m_EditRotX);
	DDX_Text(pDX, IDC_EDIT20, m_EditRotY);
	DDX_Text(pDX, IDC_EDIT21, m_EditRotZ);
	DDX_Text(pDX, IDC_EDIT13, m_EditHueStart);
	DDX_Text(pDX, IDC_EDIT14, m_EditContrastStart);
	DDX_Text(pDX, IDC_EDIT15, m_EditBrightnessStart);
	DDX_Text(pDX, IDC_EDIT16, m_EditHueEnd);
	DDX_Text(pDX, IDC_EDIT17, m_EditContrastEnd);
	DDX_Text(pDX, IDC_EDIT18, m_EditBrightnessEnd);
	DDX_Text(pDX, IDC_EDIT43, m_EditParticleCount);
	DDX_Control(pDX, IDC_CHECK15, m_CheckRandRot);
	DDX_Text(pDX, IDC_EDIT22, m_EditMoveScaleX);
	DDX_Text(pDX, IDC_EDIT23, m_EditMoveScaleY);
	DDX_Text(pDX, IDC_EDIT24, m_EditMoveScaleZ);
	DDX_Control(pDX, IDC_CHECK16, m_CheckMoveScale);
	DDX_Text(pDX, IDC_EDIT25, m_EditRandDirX);
	DDX_Text(pDX, IDC_EDIT26, m_EditRandDirY);
	DDX_Text(pDX, IDC_EDIT27, m_EditRandDirZ);
	DDX_Control(pDX, IDC_CHECK3, m_CheckRandDir);
	DDX_Control(pDX, IDC_CHECK17, m_CheckColorTex);
	DDX_Text(pDX, IDC_EDIT28, m_EditMoveSpeed);
	DDX_Text(pDX, IDC_EDIT29, m_EditMoveSpeed_Min);
	DDX_Text(pDX, IDC_EDIT30, m_EditMoveSpeed_Max);
	DDX_Control(pDX, IDC_EDIT29, m_EditCtlMoveSpeed_Min);
	DDX_Control(pDX, IDC_EDIT30, m_EditCtlMoveSpeed_Max);
	DDX_Control(pDX, IDC_CHECK4, m_CheckRandMoveSpeed);
	DDX_Text(pDX, IDC_EDIT45, m_EditRotSpeed);
	DDX_Text(pDX, IDC_EDIT46, m_EditRotSpeed_Min);
	DDX_Text(pDX, IDC_EDIT47, m_EditRotSpeed_Max);
	DDX_Control(pDX, IDC_EDIT46, m_EditCtlRotSpeed_Min);
	DDX_Control(pDX, IDC_EDIT47, m_EditCtlRotSpeed_Max);
	DDX_Control(pDX, IDC_CHECK18, m_CheckRandRotSpeed);
	DDX_Text(pDX, IDC_EDIT31, m_EditAlphaSpeed);
	DDX_Text(pDX, IDC_EDIT32, m_EditAlphaSpeed_Min);
	DDX_Text(pDX, IDC_EDIT33, m_EditAlphaSpeed_Max);
	DDX_Control(pDX, IDC_EDIT32, m_EditCtrAlphaSpeed_Min);
	DDX_Control(pDX, IDC_EDIT33, m_EditCtrAlphaSpeed_Max);
	DDX_Control(pDX, IDC_CHECK5, m_CheckRandAlphaSpeed);
	DDX_Text(pDX, IDC_EDIT34, m_EditCreateDelay);
	DDX_Text(pDX, IDC_EDIT35, m_EditCreateDelay_Min);
	DDX_Text(pDX, IDC_EDIT36, m_EditCreateDelay_Max);
	DDX_Control(pDX, IDC_EDIT35, m_EditCtrCreateDelay_Min);
	DDX_Control(pDX, IDC_EDIT36, m_EditCtrCreateDelay_Max);
	DDX_Control(pDX, IDC_CHECK6, m_CheckRandCreateDelay);
	DDX_Control(pDX, IDC_CHECK11, m_CheckFadeIn);
	DDX_Control(pDX, IDC_CHECK12, m_CheckFadeOut);
	DDX_Control(pDX, IDC_CHECK10, m_CheckDistortion);
	DDX_Control(pDX, IDC_CHECK19, m_CheckTestMesh);
	DDX_Control(pDX, IDC_CHECK14, m_CheckRevColor);
	DDX_Text(pDX, IDC_EDIT48, m_EditSaturationStart);
	DDX_Text(pDX, IDC_EDIT49, m_EditSaturationEnd);
	DDX_Control(pDX, IDC_CHECK20, m_CheckIsAnim);
	DDX_Control(pDX, IDC_EDIT38, m_EditCtrMaxFrame);
	DDX_Text(pDX, IDC_EDIT40, m_EditFileName);
	DDX_Text(pDX, IDC_EDIT42, m_EditColorIndex);
	DDX_Control(pDX, IDC_CHECK1, m_CheckUseRGBA);
	DDX_Control(pDX, IDC_CHECK21, m_bCheckUseMask);
	DDX_Text(pDX, IDC_EDIT50, m_EditMaskIndex);
}

void CParticleTab::Set_GraphicDev(LPDIRECT3DDEVICE9 pDev)
{
	m_pGraphicDev = pDev;
}

void CParticleTab::Set_Index(wstring wstrIdx)
{
	m_fCreatePause = _float(_tstof(m_EditCreateDelay)) + 1.f;

	_bool bIsAnim = m_CheckIsAnim.GetCheck() ? true : false;
	if(!bIsAnim)
		GetDlgItem(IDC_EDIT38)->SetWindowTextW(wstrIdx.c_str());
	else
		GetDlgItem(IDC_EDIT38)->SetWindowTextW(L"0");
}

void CParticleTab::Set_ColorIndex(wstring wstrIdx)
{
	GetDlgItem(IDC_EDIT42)->SetWindowTextW(wstrIdx.c_str());
}

void CParticleTab::LateInit()
{
	m_RadioResType[RES_TEXTURE].SetCheck(true);
	m_RadioBillType[0].SetCheck(true);
	m_RadioMoveType[0].SetCheck(true);
	GetDlgItem(IDC_RADIO5)->EnableWindow(false);
	GetDlgItem(IDC_RADIO6)->EnableWindow(false);

	m_vecObjectList.reserve(1);

	if (nullptr == m_ResPopup_Mesh.GetSafeHwnd())
	{
		m_ResPopup_Mesh.Create(IDD_RESLISTPOPUP);
		m_ResPopup_Mesh.Set_ResType(CResListPopup::TYPE_MESH);
	}
	if (nullptr == m_ResPopup_Tex.GetSafeHwnd())
	{
		m_ResPopup_Tex.Create(IDD_RESLISTPOPUP);
		m_ResPopup_Tex.Set_ResType(CResListPopup::TYPE_TEX);
	}


	CManagement*		pManagement = CManagement::Get_Instance();
	
	m_pTestBox = static_cast<CMeshEffect*>(pManagement->Clone_GameObject_Return(L"GameObject_MeshEffect", nullptr));
	lstrcpy(m_pTestBox->Get_Info()->szName, L"Mesh_DefaultBox");
	lstrcpy(m_pTestBox->Get_Info()->szColorName, L"Tex_Colors");
	m_pTestBox->Get_Info()->vStartScale = _v3(5, 5, 5);


}

void CParticleTab::Update(const _float DeltaTime)
{
	UpdateData(TRUE);

	Check_ResType();
	Check_FormControlEnable();

	


	Create_Particle(DeltaTime);

	UpdateData(FALSE);
}

void CParticleTab::Render()
{
	if (m_CheckTestMesh.GetCheck() ? true : false)
	{
		//m_pTestBox->Render_GameObject();
		static_cast<CRenderer*>(m_pTestBox->Get_Component(L"Com_Renderer"))->Add_RenderList(RENDER_NONALPHA, m_pTestBox);
	}
}

void CParticleTab::Check_ResType()
{
	_bool bTexture = (m_RadioResType[RES_TEXTURE].GetCheck()) ? true : false;

	m_EditMeshRes.EnableWindow(!bTexture);
	m_ButtonMeshRes.EnableWindow(!bTexture);

	m_EditTextureRes.EnableWindow(bTexture);
	m_ButtonTextureRes.EnableWindow(bTexture);

	_bool bUseColorTex = (m_CheckColorTex.GetCheck()) ? true : false;
	GetDlgItem(IDC_EDIT44)->EnableWindow(bUseColorTex);

	GetDlgItem(IDC_EDIT2)->SetWindowTextW(m_wstrTexName.c_str());
	GetDlgItem(IDC_EDIT44)->SetWindowTextW(m_wstrColorTexName.c_str());
	GetDlgItem(IDC_EDIT1)->SetWindowTextW(m_wstrMeshName.c_str());
}

void CParticleTab::Check_FormControlEnable()
{
	_bool bRandMoveSpeed = (m_CheckRandMoveSpeed.GetCheck()) ? true : false;
	m_EditCtlMoveSpeed_Min.EnableWindow(bRandMoveSpeed);
	m_EditCtlMoveSpeed_Max.EnableWindow(bRandMoveSpeed);
	if (!bRandMoveSpeed)
	{
		wstring wstrZero = L"0.0";
		GetDlgItem(IDC_EDIT29)->SetWindowTextW(wstrZero.c_str());
		GetDlgItem(IDC_EDIT30)->SetWindowTextW(wstrZero.c_str());
	}

	_bool bRandRotSpeed = (m_CheckRandRotSpeed.GetCheck()) ? true : false;
	m_EditCtlRotSpeed_Min.EnableWindow(bRandRotSpeed);
	m_EditCtlRotSpeed_Max.EnableWindow(bRandRotSpeed);
	if (!bRandRotSpeed)
	{
		wstring wstrZero = L"0.0";
		GetDlgItem(IDC_EDIT46)->SetWindowTextW(wstrZero.c_str());
		GetDlgItem(IDC_EDIT47)->SetWindowTextW(wstrZero.c_str());
	}

	_bool bRandAlphaSpeed = (m_CheckRandAlphaSpeed.GetCheck()) ? true : false;
	m_EditCtrAlphaSpeed_Min.EnableWindow(bRandAlphaSpeed);
	m_EditCtrAlphaSpeed_Max.EnableWindow(bRandAlphaSpeed);
	if (!bRandAlphaSpeed)
	{
		wstring wstrZero = L"0.0";
		GetDlgItem(IDC_EDIT32)->SetWindowTextW(wstrZero.c_str());
		GetDlgItem(IDC_EDIT33)->SetWindowTextW(wstrZero.c_str());
	}

	_bool bRandCreateDelay = (m_CheckRandCreateDelay.GetCheck()) ? true : false;
	m_EditCtrCreateDelay_Min.EnableWindow(bRandCreateDelay);
	m_EditCtrCreateDelay_Max.EnableWindow(bRandCreateDelay);
	if (!bRandCreateDelay)
	{
		wstring wstrZero = L"0.0";
		GetDlgItem(IDC_EDIT35)->SetWindowTextW(wstrZero.c_str());
		GetDlgItem(IDC_EDIT36)->SetWindowTextW(wstrZero.c_str());
	}

	_bool bIsAnim = m_CheckIsAnim.GetCheck() ? true : false;
	m_EditCtrMaxFrame.EnableWindow(!bIsAnim);

	GetDlgItemText(IDC_EDIT34, m_EditCreateDelay);
	GetDlgItemText(IDC_EDIT35, m_EditCreateDelay_Min);
	GetDlgItemText(IDC_EDIT36, m_EditCreateDelay_Max);
}


void CParticleTab::Create_Particle(const _float DeltaTime)
{
	if (!m_bCreateParticle)
		return;

	m_fCreatePause -= DeltaTime;
	if (m_fCreatePause > 0)
		return;

	m_fParticleTime_Check += DeltaTime;
	if (m_fParticleTime_Check >= _float(_tstof(m_EditCreateDelay)))
	{
		m_fParticleTime_Check = 0.f;

		GetDlgItemText(IDC_EDIT43, m_EditParticleCount);
		for (_int i = 0; i < _tstoi(m_EditParticleCount); ++i)
		{
			if (m_RadioResType[RES_TEXTURE].GetCheck() ? true : false)
				Create_Effect();
			else
				Create_MeshEffect();
		}
	}
}

void CParticleTab::Create_Effect()
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	if (0 > (pManagement->Add_GameObject_ToLayer(L"GameObject_Effect", SCENE_STAGE, L"Layer_Effect")))
		return;

	CEffect* pEff = nullptr;

	pEff = dynamic_cast<CEffect*>(pManagement->Get_GameObjectBack(L"Layer_Effect", SCENE_STAGE));
	m_pInfo = pEff->Get_Info();

	Setup_EffInfo(false);

	Safe_Release(pManagement);
}

void CParticleTab::Create_MeshEffect()
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	if (0 > (pManagement->Add_GameObject_ToLayer(L"GameObject_MeshEffect", SCENE_STAGE, L"Layer_Effect")))
		return;

	CMeshEffect* pEff = nullptr;
	pEff = dynamic_cast<CMeshEffect*>(pManagement->Get_GameObjectBack(L"Layer_Effect", SCENE_STAGE));
	m_pInfo = pEff->Get_Info();
	Setup_EffInfo(true);

	Safe_Release(pManagement);
}

void CParticleTab::Setup_EffInfo(_bool bIsMesh)
{
	m_pInfo->bDistortion = m_CheckDistortion.GetCheck() ? true : false;
	m_pInfo->bRevColor = m_CheckRevColor.GetCheck() ? true : false;

	m_pInfo->bFadeIn = m_CheckFadeIn.GetCheck() ? true : false;
	m_pInfo->bFadeOut = m_CheckFadeOut.GetCheck() ? true : false;

	m_pInfo->bLinearMove = m_RadioMoveType[1].GetCheck() ? true : false;
	m_pInfo->bEaseInMove = m_RadioMoveType[2].GetCheck() ? true : false;
	m_pInfo->bEaseOutMove = m_RadioMoveType[3].GetCheck() ? true : false;

	m_pInfo->bUseRGBA = m_CheckUseRGBA.GetCheck() ? true : false;

	GetDlgItemText(IDC_EDIT42, m_EditColorIndex);
	m_pInfo->fColorIndex = _float(_tstoi(m_EditColorIndex));


	if (m_bCheckUseMask.GetCheck() ? true : false)
	{
		GetDlgItemText(IDC_EDIT50, m_EditMaskIndex);
		m_pInfo->fMaskIndex = _float(_tstoi(m_EditMaskIndex));
	}
	else
		m_pInfo->fMaskIndex = -1.f;


	GetDlgItemText(IDC_EDIT43, m_EditParticleCount);
	m_pInfo->iMaxCount = _int(_tstoi(m_EditParticleCount));

	GetDlgItemText(IDC_EDIT34, m_EditCreateDelay);
	GetDlgItemText(IDC_EDIT35, m_EditCreateDelay_Min);
	GetDlgItemText(IDC_EDIT36, m_EditCreateDelay_Max);
	m_pInfo->fCreateDelay = _float(_tstof(m_EditCreateDelay));
	m_pInfo->fCreateDelay_Min = _float(_tstof(m_EditCreateDelay_Min));
	m_pInfo->fCreateDelay_Max = _float(_tstof(m_EditCreateDelay_Max));

	GetDlgItemText(IDC_EDIT28, m_EditMoveSpeed);
	GetDlgItemText(IDC_EDIT29, m_EditMoveSpeed_Min);
	GetDlgItemText(IDC_EDIT30, m_EditMoveSpeed_Max);
	m_pInfo->fMoveSpeed = _float(_tstof(m_EditMoveSpeed));
	m_pInfo->fMoveSpeed_Min = _float(_tstof(m_EditMoveSpeed_Min));
	m_pInfo->fMoveSpeed_Max = _float(_tstof(m_EditMoveSpeed_Max));

	m_pInfo->bRandomMove = m_CheckRandDir.GetCheck() ? true : false;
	GetDlgItemText(IDC_EDIT25, m_EditRandDirX);
	GetDlgItemText(IDC_EDIT26, m_EditRandDirY);
	GetDlgItemText(IDC_EDIT27, m_EditRandDirZ);
	m_pInfo->vRandDirectionRange = _v3{ _float(_tstof(m_EditRandDirX)), _float(_tstof(m_EditRandDirY)), _float(_tstof(m_EditRandDirZ)) };

	m_pInfo->bScaleMove = m_CheckMoveScale.GetCheck() ? true : false;
	GetDlgItemText(IDC_EDIT22, m_EditMoveScaleX);
	GetDlgItemText(IDC_EDIT23, m_EditMoveScaleY);
	GetDlgItemText(IDC_EDIT24, m_EditMoveScaleZ);
	m_pInfo->vMoveScale = _v3{ _float(_tstof(m_EditMoveScaleX)), _float(_tstof(m_EditMoveScaleY)), _float(_tstof(m_EditMoveScaleZ)) };

	m_pInfo->bUseColorTex = (m_CheckColorTex.GetCheck()) ? true : false;
	m_pInfo->bColorMove = true;
	GetDlgItemText(IDC_EDIT13, m_EditHueStart);
	GetDlgItemText(IDC_EDIT14, m_EditContrastStart);
	GetDlgItemText(IDC_EDIT15, m_EditBrightnessStart);
	GetDlgItemText(IDC_EDIT48, m_EditSaturationStart);

	m_pInfo->vStartColor = _v4{ _float(_tstof(m_EditHueStart)), _float(_tstof(m_EditContrastStart)), _float(_tstof(m_EditBrightnessStart)), _float(_tstof(m_EditSaturationStart)) };

	GetDlgItemText(IDC_EDIT16, m_EditHueEnd);
	GetDlgItemText(IDC_EDIT17, m_EditContrastEnd);
	GetDlgItemText(IDC_EDIT18, m_EditBrightnessEnd);
	GetDlgItemText(IDC_EDIT49, m_EditSaturationEnd);
	m_pInfo->vEndColor = _v4{ _float(_tstof(m_EditHueEnd)), _float(_tstof(m_EditContrastEnd)), _float(_tstof(m_EditBrightnessEnd)),  _float(_tstof(m_EditSaturationEnd)) };

	m_pInfo->bBillBoard = m_RadioBillType[1].GetCheck() ? true : false;
	m_pInfo->bOnlyYRot = m_RadioBillType[2].GetCheck() ? true : false;


	GetDlgItemText(IDC_EDIT19, m_EditRotX);
	GetDlgItemText(IDC_EDIT20, m_EditRotY);
	GetDlgItemText(IDC_EDIT21, m_EditRotZ);
	m_pInfo->vRotDirection = _v3{ _float(_tstof(m_EditRotX)), _float(_tstof(m_EditRotY)), _float(_tstof(m_EditRotZ)) };

	GetDlgItemText(IDC_EDIT45, m_EditRotSpeed);
	GetDlgItemText(IDC_EDIT46, m_EditRotSpeed_Min);
	GetDlgItemText(IDC_EDIT47, m_EditRotSpeed_Max);
	m_pInfo->fRotSpeed = _float(_tstof(m_EditRotSpeed));
	m_pInfo->fRotSpeed_Min = _float(_tstof(m_EditRotSpeed_Min));
	m_pInfo->fRotSpeed_Max = _float(_tstof(m_EditRotSpeed_Max));
	m_pInfo->bRandomRot = m_CheckRandRot.GetCheck() ? true : false;


	m_pInfo->bRotMove = (m_pInfo->fRotSpeed == 0) ? false : true;


	GetDlgItemText(IDC_EDIT10, m_EditPosX);
	GetDlgItemText(IDC_EDIT11, m_EditPosY);
	GetDlgItemText(IDC_EDIT12, m_EditPosZ);
	m_pInfo->vStartPos = _v3{ _float(_tstof(m_EditPosX)), _float(_tstof(m_EditPosY)), _float(_tstof(m_EditPosZ)) };
	m_pInfo->bRandStartPos = m_CheckRandPos.GetCheck() ? true : false;
	memcpy(&m_pInfo->fRandStartPosRange_Min, &_v3{ 0.f, 0.f, 0.f }, sizeof(_v3));
	memcpy(&m_pInfo->fRandStartPosRange_Max, &_v3{ _float(_tstof(m_EditPosX)), _float(_tstof(m_EditPosY)), _float(_tstof(m_EditPosZ)) }, sizeof(_v3));
	//m_pInfo->fRandStartPosRange_Max = _v3{ _float(_tstof(m_EditPosX)), _float(_tstof(m_EditPosY)), _float(_tstof(m_EditPosZ)) };

	m_pInfo->bDirMove = true;
	GetDlgItemText(IDC_EDIT7, m_EditDirX);
	GetDlgItemText(IDC_EDIT8, m_EditDirY);
	GetDlgItemText(IDC_EDIT9, m_EditDirZ);
	m_pInfo->vMoveDirection = _v3{ _float(_tstof(m_EditDirX)), _float(_tstof(m_EditDirY)), _float(_tstof(m_EditDirZ)) };

	GetDlgItemText(IDC_EDIT4, m_EditSizeX);
	GetDlgItemText(IDC_EDIT5, m_EditSizeY);
	GetDlgItemText(IDC_EDIT6, m_EditSizeZ);
	m_pInfo->vStartScale = _v3{ _float(_tstof(m_EditSizeX)), _float(_tstof(m_EditSizeY)), _float(_tstof(m_EditSizeZ)) };

	m_pInfo->bStaticFrame = m_CheckIsAnim.GetCheck() ? false : true;

	GetDlgItemText(IDC_EDIT38, m_EditMaxCount);
	if (m_pInfo->bStaticFrame)
		m_pInfo->fMaxFrame = _float(_tstof(m_EditMaxCount));
	else
		m_pInfo->fMaxFrame = m_fMaxFrame;

	GetDlgItemText(IDC_EDIT37, m_EditLifeTime);
	m_pInfo->fLifeTime = _float(_tstof(m_EditLifeTime));

	GetDlgItemText(IDC_EDIT39, m_EditMaxAlpha);
	m_pInfo->fMaxAlpha = _float(_tstof(m_EditMaxAlpha));

	GetDlgItemText(IDC_EDIT41, m_EditAnimSpeed);
	m_pInfo->fAnimSpeed = _float(_tstof(m_EditAnimSpeed));

	GetDlgItemText(IDC_EDIT31, m_EditAlphaSpeed);
	GetDlgItemText(IDC_EDIT32, m_EditAlphaSpeed_Min);
	GetDlgItemText(IDC_EDIT33, m_EditAlphaSpeed_Max);
	m_pInfo->fAlphaSpeed = _float(_tstof(m_EditAlphaSpeed));
	m_pInfo->fAlphaSpeed_Min = _float(_tstof(m_EditAlphaSpeed_Min));
	m_pInfo->fAlphaSpeed_Max = _float(_tstof(m_EditAlphaSpeed_Max));

	if(bIsMesh)
		lstrcpy(m_pInfo->szName, m_wstrMeshName.c_str());
	else
		lstrcpy(m_pInfo->szName, m_wstrTexName.c_str());

	lstrcpy(m_pInfo->szGradientName, m_wstrGradientTexName.c_str());
	lstrcpy(m_pInfo->szColorName, m_wstrColorTexName.c_str());

}

void CParticleTab::Release()
{
	Safe_Release(m_pTestBox);
}



BEGIN_MESSAGE_MAP(CParticleTab, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CParticleTab::OnBnClickedButton_MeshRes)
	ON_BN_CLICKED(IDC_BUTTON6, &CParticleTab::OnBnClickedButton_StartParticle)
	ON_BN_CLICKED(IDC_BUTTON7, &CParticleTab::OnBnClickedButton_StopParticle)
	ON_BN_CLICKED(IDC_BUTTON8, &CParticleTab::OnBnClickedButton_CreateOnce)
	ON_BN_CLICKED(IDC_BUTTON2, &CParticleTab::OnBnClickedButton_ResTexture)
	ON_BN_CLICKED(IDC_BUTTON9, &CParticleTab::OnBnClickedButton_ResColor)
	ON_BN_CLICKED(IDC_BUTTON4, &CParticleTab::OnBnClickedButton_Save)
	ON_BN_CLICKED(IDC_BUTTON5, &CParticleTab::OnBnClickedButton_Load)
	ON_BN_CLICKED(IDC_BUTTON3, &CParticleTab::OnBnClickedButton_ResMask)

END_MESSAGE_MAP()


// CParticleTab 메시지 처리기입니다.


void CParticleTab::OnBnClickedButton_MeshRes()
{
	m_ResPopup_Mesh.ShowWindow(SW_SHOW);
}

void CParticleTab::OnBnClickedButton_ResTexture()
{
	m_ResPopup_Tex.ShowWindow(SW_SHOW);
	m_ResPopup_Tex.Set_ResType(CResListPopup::TYPE_TEX);
}

void CParticleTab::OnBnClickedButton_ResColor()
{
	m_ResPopup_Tex.ShowWindow(SW_SHOW);
	m_ResPopup_Tex.Set_ResType(CResListPopup::TYPE_COLOR);
}

void CParticleTab::OnBnClickedButton_ResMask()
{
	m_ResPopup_Tex.ShowWindow(SW_SHOW);
	m_ResPopup_Tex.Set_ResType(CResListPopup::TYPE_GRADIENT);
}


void CParticleTab::OnBnClickedButton_StartParticle()
{
	m_bCreateParticle = true;
}


void CParticleTab::OnBnClickedButton_StopParticle()
{
	m_bCreateParticle = false;
}


void CParticleTab::OnBnClickedButton_CreateOnce()
{
	GetDlgItemText(IDC_EDIT43, m_EditParticleCount);
	for (_int i = 0; i < _tstoi(m_EditParticleCount); ++i)
	{
		if (m_RadioResType[RES_TEXTURE].GetCheck() ? true : false)
			Create_Effect();
		else
			Create_MeshEffect();
	}
}



void CParticleTab::OnBnClickedButton_Save()
{
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE, L".dat", L"제목없음.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurrentPath[MAX_STR] = L"";

	// 현재 작업경로를 얻어오는 함수
	::GetCurrentDirectory(MAX_STR, szCurrentPath);
	// 현재 경로에서 파일명을 제거하는 함수. 제거할 파일명 없다면 말단 폴더명을 제거 (\Debug ,\Tool)
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\EffectData");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal()) // IDOK : 확인, 저장, 열기 등 버튼 눌렀을 때 반환
	{
		HANDLE hFile = ::CreateFile(Dlg.GetPathName(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			::MSG_BOX("Save Failed. [INVALID_HANDLE_VALUE]");

		DWORD dwByte = 0;

		_bool bIsTex = m_RadioResType[0].GetCheck() ? true : false;
		::WriteFile(hFile, &bIsTex, sizeof(_bool), &dwByte, nullptr);

		::WriteFile(hFile, &m_pInfo->bBillBoard, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bOnlyYRot, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bDistortion, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bStaticFrame, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bUseColorTex, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bUseRGBA, sizeof(_bool), &dwByte, nullptr);

		::WriteFile(hFile, &m_pInfo->bColorMove, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bDirMove, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bFadeIn, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bFadeOut, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bLinearMove, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bRandomMove, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bRandomRot, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bRandStartPos, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bRevColor, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bRotMove, sizeof(_bool), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->bScaleMove, sizeof(_bool), &dwByte, nullptr);

		::WriteFile(hFile, &m_pInfo->fAlphaSpeed, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fAlphaSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fAlphaSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fColorSpeed, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fCreateDelay, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fCreateDelay_Max, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fCreateDelay_Min, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fLifeTime, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMaxAlpha, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fAnimSpeed, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMaxFrame, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMoveScaleSpeed, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMoveSpeed, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMoveSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMoveSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fRandStartPosRange_Max, sizeof(_float) * 3, &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fRandStartPosRange_Min, sizeof(_float) * 3, &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fRotSpeed, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fRotSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fRotSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->iMaxCount, sizeof(_int), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->szColorName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->szGradientName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vEndColor, sizeof(_v4), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vMoveDirection, sizeof(_v3), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vMoveScale, sizeof(_v3), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vRandDirectionRange, sizeof(_v3), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vRotDirection, sizeof(_v3), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vStartColor, sizeof(_v4), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vStartPos, sizeof(_v3), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->vStartScale, sizeof(_v3), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fColorIndex, sizeof(_float), &dwByte, nullptr);
		::WriteFile(hFile, &m_pInfo->fMaskIndex, sizeof(_float), &dwByte, nullptr);
		

		_bool bRandRotSpeed = (m_CheckRandRotSpeed.GetCheck()) ? true : false;
		::WriteFile(hFile, &bRandRotSpeed, sizeof(_bool), &dwByte, nullptr);
		_bool bRandAlphaSpeed = (m_CheckRandAlphaSpeed.GetCheck()) ? true : false;
		::WriteFile(hFile, &bRandAlphaSpeed, sizeof(_bool), &dwByte, nullptr);
		_bool bRandMoveSpeed = (m_CheckRandMoveSpeed.GetCheck()) ? true : false;
		::WriteFile(hFile, &bRandMoveSpeed, sizeof(_bool), &dwByte, nullptr);
		_bool bRandCreateDelay = (m_CheckRandCreateDelay.GetCheck()) ? true : false;
		::WriteFile(hFile, &bRandCreateDelay, sizeof(_bool), &dwByte, nullptr);

		CloseHandle(hFile);
		MessageBox(_T("Save Success."), _T("Save"), MB_OK);
	}

	UpdateData(FALSE);
}


void CParticleTab::OnBnClickedButton_Load()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_fCreatePause = _float(_tstof(m_EditCreateDelay)) + 1.f;

	UpdateData(TRUE);

	CFileDialog Dlg(TRUE, L".dat", L"제목없음.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"Data Files(*.dat)|*.dat||", this);

	TCHAR szCurrentPath[MAX_STR] = L"";

	// 현재 작업경로를 얻어오는 함수
	::GetCurrentDirectory(MAX_STR, szCurrentPath);
	// 현재 경로에서 파일명을 제거하는 함수. 제거할 파일명 없다면 말단 폴더명을 제거 (\Debug ,\Tool)
	::PathRemoveFileSpec(szCurrentPath);
	::PathRemoveFileSpec(szCurrentPath);

	lstrcat(szCurrentPath, L"\\Data\\EffectData");

	// 절대 경로만 가능
	Dlg.m_ofn.lpstrInitialDir = szCurrentPath;

	if (IDOK == Dlg.DoModal()) // IDOK : 확인, 저장, 열기 등 버튼 눌렀을 때 반환
	{
		HANDLE hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
			::MSG_BOX("Save Failed. [INVALID_HANDLE_VALUE]");

		DWORD dwByte = 0;
		EFFECT_INFO tInfo;
		_tchar szBuff[256] = L"";

		_bool bIsTex = false;
		::ReadFile(hFile, &bIsTex, sizeof(_bool), &dwByte, nullptr);
		m_RadioResType[0].SetCheck(bIsTex);
		m_RadioResType[1].SetCheck(!bIsTex);

		GetDlgItem(IDC_EDIT40)->SetWindowTextW(::PathFindFileName(Dlg.GetPathName()));

		while (true)
		{
			::ReadFile(hFile, &tInfo.bBillBoard, sizeof(_bool), &dwByte, nullptr);
			m_RadioBillType[1].SetCheck(tInfo.bBillBoard);

			::ReadFile(hFile, &tInfo.bOnlyYRot, sizeof(_bool), &dwByte, nullptr);
			m_RadioBillType[2].SetCheck(tInfo.bOnlyYRot);
			m_RadioBillType[0].SetCheck(!(tInfo.bBillBoard || tInfo.bOnlyYRot));

			::ReadFile(hFile, &tInfo.bDistortion, sizeof(_bool), &dwByte, nullptr);
			m_CheckDistortion.SetCheck(tInfo.bDistortion);

			::ReadFile(hFile, &tInfo.bStaticFrame, sizeof(_bool), &dwByte, nullptr);
			m_CheckIsAnim.SetCheck(!tInfo.bStaticFrame);

			::ReadFile(hFile, &tInfo.bUseColorTex, sizeof(_bool), &dwByte, nullptr);
			m_CheckColorTex.SetCheck(tInfo.bUseColorTex);

			::ReadFile(hFile, &tInfo.bUseRGBA, sizeof(_bool), &dwByte, nullptr);
			m_CheckUseRGBA.SetCheck(tInfo.bUseRGBA);

			::ReadFile(hFile, &tInfo.bColorMove, sizeof(_bool), &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.bDirMove, sizeof(_bool), &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.bFadeIn, sizeof(_bool), &dwByte, nullptr);
			m_CheckFadeIn.SetCheck(tInfo.bFadeIn);

			::ReadFile(hFile, &tInfo.bFadeOut, sizeof(_bool), &dwByte, nullptr);
			m_CheckFadeOut.SetCheck(tInfo.bFadeOut);

			::ReadFile(hFile, &tInfo.bLinearMove, sizeof(_bool), &dwByte, nullptr);
			m_RadioMoveType[1].SetCheck(tInfo.bLinearMove);
			m_RadioMoveType[0].SetCheck(!tInfo.bLinearMove);

			::ReadFile(hFile, &tInfo.bRandomMove, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandDir.SetCheck(tInfo.bRandomMove);

			::ReadFile(hFile, &tInfo.bRandomRot, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandRot.SetCheck(tInfo.bRandomRot);

			::ReadFile(hFile, &tInfo.bRandStartPos, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandPos.SetCheck(tInfo.bRandStartPos);

			::ReadFile(hFile, &tInfo.bRevColor, sizeof(_bool), &dwByte, nullptr);
			m_CheckRevColor.SetCheck(tInfo.bRevColor);

			::ReadFile(hFile, &tInfo.bRotMove, sizeof(_bool), &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.bScaleMove, sizeof(_bool), &dwByte, nullptr);
			m_CheckMoveScale.SetCheck(tInfo.bScaleMove);

			::ReadFile(hFile, &tInfo.fAlphaSpeed, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fAlphaSpeed);
			m_EditAlphaSpeed.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fAlphaSpeed_Max, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fAlphaSpeed_Max);
			m_EditAlphaSpeed_Max.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fAlphaSpeed_Min, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fAlphaSpeed_Min);
			m_EditAlphaSpeed_Min.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fColorSpeed, sizeof(_float), &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.fCreateDelay, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fCreateDelay);
			m_EditCreateDelay.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fCreateDelay_Max, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fCreateDelay_Max);
			m_EditCreateDelay_Max.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fCreateDelay_Min, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fCreateDelay_Min);
			m_EditCreateDelay_Min.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fLifeTime, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fLifeTime);
			m_EditLifeTime.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fMaxAlpha, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fMaxAlpha);
			m_EditMaxAlpha.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fAnimSpeed, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fAnimSpeed);
			m_EditAnimSpeed.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fMaxFrame, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fMaxFrame);
			m_EditMaxCount.SetString(szBuff);
			m_fMaxFrame = tInfo.fMaxFrame;

			::ReadFile(hFile, &tInfo.fMoveScaleSpeed, sizeof(_float), &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.fMoveSpeed, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fMoveSpeed);
			m_EditMoveSpeed.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fMoveSpeed_Max, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fMoveSpeed_Max);
			m_EditMoveSpeed_Max.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fMoveSpeed_Min, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fMoveSpeed_Min);
			m_EditMoveSpeed_Min.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fRandStartPosRange_Max, sizeof(_float) * 3, &dwByte, nullptr);
			if (tInfo.bRandStartPos)
			{
				_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fRandStartPosRange_Max[0]);
				m_EditPosX.SetString(szBuff);

				_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fRandStartPosRange_Max[1]);
				m_EditPosY.SetString(szBuff);

				_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fRandStartPosRange_Max[2]);
				m_EditPosZ.SetString(szBuff);
			}

			::ReadFile(hFile, &tInfo.fRandStartPosRange_Min, sizeof(_float) * 3, &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.fRotSpeed, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fRotSpeed);
			m_EditRotSpeed.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fRotSpeed_Max, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fRotSpeed_Max);
			m_EditRotSpeed_Max.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fRotSpeed_Min, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fRotSpeed_Min);
			m_EditRotSpeed_Min.SetString(szBuff);

			::ReadFile(hFile, &tInfo.iMaxCount, sizeof(_int), &dwByte, nullptr);
			wsprintf(szBuff, L"%i", tInfo.iMaxCount);
			m_EditParticleCount.SetString(szBuff);

			::ReadFile(hFile, &tInfo.szColorName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
			GetDlgItem(IDC_EDIT44)->SetWindowTextW(tInfo.szColorName);
			m_wstrColorTexName = tInfo.szColorName;

			if (bIsTex)
			{
				::ReadFile(hFile, &tInfo.szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
				GetDlgItem(IDC_EDIT2)->SetWindowTextW(tInfo.szName);
				m_wstrTexName = tInfo.szName;
			}
			else
			{
				::ReadFile(hFile, &tInfo.szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
				GetDlgItem(IDC_EDIT2)->SetWindowTextW(tInfo.szName);
				m_wstrMeshName = tInfo.szName;
			}

			::ReadFile(hFile, &tInfo.szGradientName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);

			::ReadFile(hFile, &tInfo.vEndColor, sizeof(_v4), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vEndColor.x);

			m_EditHueEnd.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vEndColor.y);
			m_EditContrastEnd.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vEndColor.z);
			m_EditBrightnessEnd.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vEndColor.w);
			m_EditSaturationEnd.SetString(szBuff);


			::ReadFile(hFile, &tInfo.vMoveDirection, sizeof(_v3), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vMoveDirection.x);
			m_EditDirX.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vMoveDirection.y);
			m_EditDirY.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vMoveDirection.z);
			m_EditDirZ.SetString(szBuff);

			::ReadFile(hFile, &tInfo.vMoveScale, sizeof(_v3), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vMoveScale.x);
			m_EditMoveScaleX.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vMoveScale.y);
			m_EditMoveScaleY.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vMoveScale.z);
			m_EditMoveScaleZ.SetString(szBuff);

			::ReadFile(hFile, &tInfo.vRandDirectionRange, sizeof(_v3), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vRandDirectionRange.x);
			m_EditRandDirX.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vRandDirectionRange.y);
			m_EditRandDirY.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vRandDirectionRange.z);
			m_EditRandDirZ.SetString(szBuff);

			::ReadFile(hFile, &tInfo.vRotDirection, sizeof(_v3), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vRotDirection.x);
			m_EditRotX.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vRotDirection.y);
			m_EditRotY.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vRotDirection.z);
			m_EditRotZ.SetString(szBuff);

			::ReadFile(hFile, &tInfo.vStartColor, sizeof(_v4), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartColor.x);

			m_EditHueStart.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartColor.y);
			m_EditContrastStart.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartColor.z);
			m_EditBrightnessStart.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartColor.w);
			m_EditSaturationStart.SetString(szBuff);


			::ReadFile(hFile, &tInfo.vStartPos, sizeof(_v3), &dwByte, nullptr);
			if (!tInfo.bRandStartPos)
			{
				_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartPos.x);
				m_EditPosX.SetString(szBuff);
				_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartPos.y);
				m_EditPosY.SetString(szBuff);
				_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartPos.z);
				m_EditPosZ.SetString(szBuff);
			}

			::ReadFile(hFile, &tInfo.vStartScale, sizeof(_v3), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartScale.x);
			m_EditSizeX.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartScale.y);
			m_EditSizeY.SetString(szBuff);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.vStartScale.z);
			m_EditSizeZ.SetString(szBuff);

			::ReadFile(hFile, &tInfo.fColorIndex, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fColorIndex);
			m_EditColorIndex.SetString(szBuff);


			::ReadFile(hFile, &tInfo.fMaskIndex, sizeof(_float), &dwByte, nullptr);
			_stprintf_s(szBuff, _countof(szBuff), L"%.2f", tInfo.fMaskIndex);
			m_EditMaskIndex.SetString(szBuff);


			_bool bRandRotSpeed = false;
			::ReadFile(hFile, &bRandRotSpeed, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandRotSpeed.SetCheck(bRandRotSpeed);

			_bool bRandAlphaSpeed = false;
			::ReadFile(hFile, &bRandAlphaSpeed, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandAlphaSpeed.SetCheck(bRandAlphaSpeed);

			_bool bRandMoveSpeed = false;
			::ReadFile(hFile, &bRandMoveSpeed, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandMoveSpeed.SetCheck(bRandMoveSpeed);

			_bool bRandCreateDelay = false;
			::ReadFile(hFile, &bRandCreateDelay, sizeof(_bool), &dwByte, nullptr);
			m_CheckRandCreateDelay.SetCheck(bRandCreateDelay);

			//if (0 == dwByte)
			break;
		}

		CloseHandle(hFile);
		MessageBox(_T("Load Success."), _T("Load"), MB_OK);
	}

	UpdateData(FALSE);
}

