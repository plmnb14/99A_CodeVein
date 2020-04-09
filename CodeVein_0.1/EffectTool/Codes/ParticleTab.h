#pragma once
#include "afxwin.h"
#include "resource.h"       // 주 기호입니다.
#include "ResListPopup.h"

#include "Effect.h"
#include "MeshEffect.h"

// CParticleTab 대화 상자입니다.

namespace Engine
{
	class CGameObject;
}
class CParticleTab : public CDialog
{
	DECLARE_DYNAMIC(CParticleTab)

public:
	CParticleTab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CParticleTab();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARTICLETAB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnBnClickedButton_MeshRes();
	afx_msg void OnBnClickedButton_ResTexture();
	afx_msg void OnBnClickedButton_StartParticle();
	afx_msg void OnBnClickedButton_StopParticle();
	afx_msg void OnBnClickedButton_CreateOnce();
	afx_msg void OnBnClickedButton_ResColor();
	afx_msg void OnBnClickedButton_Save();
	afx_msg void OnBnClickedButton_Load();
	DECLARE_MESSAGE_MAP()

public:
	enum RES_TYPE { RES_TEXTURE, RES_MESH, RES_END };

public:
	void Set_GraphicDev(LPDIRECT3DDEVICE9 pDev);
	void Set_TexName(wstring wstrName) { m_wstrTexName = wstrName; }
	void Set_GradientTexName(wstring wstrName) { m_wstrGradientTexName = wstrName; }
	void Set_ColorTexName(wstring wstrName) { m_wstrColorTexName = wstrName; }
	void Set_MeshName(wstring wstrName) { m_wstrMeshName = wstrName; }
	void Set_MaxFrame(_float fIdx) { m_fMaxFrame = fIdx; }
	void Set_Index(wstring wstrIdx);
	void Set_ColorIndex(wstring wstrIdx);

public:
	void LateInit();
	void Update(const _float DeltaTime);
	void Render();

private:
	void Check_ResType();
	void Check_FormControlEnable();
	void Create_Particle(const _float DeltaTime);
	void Create_Effect();
	void Create_MeshEffect();
	void Setup_EffInfo(_bool bIsMesh);
	void Release();

private:
	vector<CGameObject*> m_vecObjectList;
	Engine::EFFECT_INFO* m_pInfo;
	_float m_fParticleTime = 0.1f;
	_float m_fParticleTime_Check = 0.f;
	_float m_fCreatePause = 0.f; // 임시
	_float m_fMaxFrame = 0.f;

	CMeshEffect* m_pTestBox = nullptr;
private:
	LPDIRECT3DDEVICE9	 m_pGraphicDev = nullptr;
	CResListPopup		 m_ResPopup_Tex = nullptr;
	CResListPopup		 m_ResPopup_Mesh = nullptr;

private:
	_bool m_bCreateParticle = false;
	wstring m_wstrTexName = L"Tex_LinePoint";
	wstring m_wstrGradientTexName = L"Tex_Gradient";
	wstring m_wstrColorTexName = L"Tex_Gradient";
	wstring m_wstrMeshName = L"Mesh_Player";

private:
	CButton m_RadioResType[2];
	CButton m_RadioBillType[3];
	CButton m_RadioMoveType[4];
	CEdit m_EditMeshRes;
	CEdit m_EditTextureRes;
	CButton m_ButtonMeshRes;
	CButton m_ButtonTextureRes;
	CString m_EditAnimSpeed;
	CString m_EditLifeTime;
	CString m_EditMaxAlpha;
	CString m_EditMaxCount;
	CButton m_CheckRandPos;
	CString m_EditSizeX;
	CString m_EditSizeY;
	CString m_EditSizeZ;
	CString m_EditDirX;
	CString m_EditDirY;
	CString m_EditDirZ;
	CString m_EditPosX;
	CString m_EditPosY;
	CString m_EditPosZ;
	CString m_EditRotX;
	CString m_EditRotY;
	CString m_EditRotZ;
	CString m_EditHueStart;
	CString m_EditContrastStart;
	CString m_EditBrightnessStart;
	CString m_EditHueEnd;
	CString m_EditContrastEnd;
	CString m_EditBrightnessEnd;
	CString m_EditParticleCount;
	CButton m_CheckRandRot;
	CString m_EditMoveScaleX;
	CString m_EditMoveScaleY;
	CString m_EditMoveScaleZ;
	CButton m_CheckMoveScale;
	CString m_EditRandDirX;
	CString m_EditRandDirY;
	CString m_EditRandDirZ;
	CButton m_CheckRandDir;
	CButton m_CheckColorTex;
	CString m_EditMoveSpeed;
	CString m_EditMoveSpeed_Min;
	CString m_EditMoveSpeed_Max;
	CEdit m_EditCtlMoveSpeed_Min;
	CEdit m_EditCtlMoveSpeed_Max;
	CButton m_CheckRandMoveSpeed;
	CString m_EditRotSpeed;
	CString m_EditRotSpeed_Min;
	CString m_EditRotSpeed_Max;
	CEdit m_EditCtlRotSpeed_Min;
	CEdit m_EditCtlRotSpeed_Max;
	CButton m_CheckRandRotSpeed;
	CString m_EditAlphaSpeed;
	CString m_EditAlphaSpeed_Min;
	CString m_EditAlphaSpeed_Max;
	CEdit m_EditCtrAlphaSpeed_Min;
	CEdit m_EditCtrAlphaSpeed_Max;
	CButton m_CheckRandAlphaSpeed;
	CString m_EditCreateDelay;
	CString m_EditCreateDelay_Min;
	CString m_EditCreateDelay_Max;
	CEdit m_EditCtrCreateDelay_Min;
	CEdit m_EditCtrCreateDelay_Max;
	CButton m_CheckRandCreateDelay;
	CButton m_CheckFadeIn;
	CButton m_CheckFadeOut;
	CButton m_CheckDistortion;
	CButton m_CheckTestMesh;
	CButton m_CheckRevColor;
	CString m_EditSaturationStart;
	CString m_EditSaturationEnd;
	CButton m_CheckIsAnim;
	CEdit m_EditCtrMaxFrame;
	CString m_EditFileName;
	CString m_EditColorIndex;
	CButton m_CheckUseRGBA;
public:
	CButton m_bCheckUseMask;
	afx_msg void OnBnClickedButton_ResMask();
	CString m_EditMaskIndex;
	CButton m_CheckGravity;
	CButton m_CheckRandSize;
	CButton m_CheckMoveWithRot;
	CButton m_CheckSlowly;
	CString m_EditDistortion;
	CButton m_CheckDissolve;
	CString m_EditScaleSpeed;
};