#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Camera.h"
#include "Dynamic_Camera.h"


BEGIN(Engine)

class ENGINE_DLL CCameraMgr : public CBase
{
	DECLARE_SINGLETON(CCameraMgr)
private:
	explicit CCameraMgr(void);
	virtual ~CCameraMgr(void);

public:
	void Update();

public:
	HRESULT	Reserve_ContainerSize(const _ushort& wSize);

	HRESULT	Ready_Camera(_Device pGraphicDev,
		const _ushort& _eCameraClass,
		const _tchar* pCameraTag,
		CameraView _CameraView,
		CameraMode _CameraMode);

	HRESULT Set_Target(CGameObject* _Target);

public:
	void Change_MainCam(const _ushort& _eCameraClass, const _tchar* pCameraTag, _bool _bDelay = false);
	void Set_MainCamera(const _ushort& _eCameraClass, const _tchar* pCameraTag);
	void Set_CamView(CameraView _eCamViewType);
	void Set_CamMode(CameraMode _eCamMode);

	void Set_MaxDistance(_float _fMaxdist);
	void Set_MidDistance(_float _fMidDist);
	void Set_MinDistance(_float _fMinDist);
	void Set_Distance(_float _fDist);

	void Set_OnAimingTarget(_bool _bOnAim);
	void Set_AimingTarget(CGameObject* pAimingTarget);

	void Set_MouseCtrl(_bool _bMouseCtrl);

public:
	void Add_MainPos(_float _fSpeed, _v3 _vDir);
	void Add_MainAt(_float _fSpeed, _v3 _vDir);
	void Set_MainAt(_v3 _vDir);
	void Set_LockAngleX(_float _fLockAngleX);

	void MainCamera_Oscillatation_SetUp(_float _fDuration, _float _fFrequency, _float _fPower, _float _fMutiply, CCamera::CAM_OSC_TYPE _eOSCType);

	_v3* Calc_MainCamPosAt();

public:
	void Set_MainPos(_v3 _vPos);

public:
	const CameraView Get_CamView();

public:
	const _mat& Get_ViewMat();
	const _mat& Get_ProjMat();

	const _v3& Get_Right();
	const _v3& Get_Up();
	const _v3& Get_Look();
	const _v3& Get_Pos();

public:
	_v3	 Get_At();

public:
	const _float& Get_XAngle();
	const _float& Get_YAngle();

private:
	CCamera* Find_Camera(const _ushort& _eCameraClass, const _tchar* pCameraTag);
	_bool Calc_Distance(CCamera* _pOld, CCamera* _pNew, _v3* _Distance);


private:
	CCamera*							m_pMainCamera = nullptr;
	typedef map<const _tchar*, CCamera*>*	CAM_MAP;
	map<const _tchar*, CCamera*>*		m_pmapCamera = nullptr;
	_ushort								m_wContainerSize = 0;

private:
	_mat	m_matView;
	_mat	m_matProj;
	_v3		m_vRight;
	_v3		m_vUp;
	_v3		m_vLook;
	_v3		m_vPos;
	_v3		m_vDistance;

private:
	_float  m_fY_Angle;
	_float  m_fX_Angle;
	_float	m_fLength;

	_bool	m_bDelay;
	_bool	m_bChange;

	_ushort m_sCamClass;
	_tchar* m_pCamTag;

public:
	virtual void Free(void);

};

END
