#pragma once

#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL CDynamic_Camera : public CCamera
{

protected:
	explicit CDynamic_Camera(_Device pGraphicDev);
	virtual ~CDynamic_Camera();

public:
	void KeyInput();

public:
	virtual _int Update_GameObject() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

private:
	virtual HRESULT Initialize() override;
	virtual HRESULT LateInit() override;

private:
	HRESULT AddComponent();

public:
	virtual void Free() override;

public:
	static CDynamic_Camera* Create(_Device pGraphicDev, 
								   CameraView _CameraViewType, 
								   CameraMode _CameraMode);

public:
	virtual CGameObject* Clone_GameObject(void* pArg);
};

END

