#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"

BEGIN(Client)

class CScene_Title final : public CScene
{
private:
	explicit CScene_Title(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Title() = default;
public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();
public:
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Layer_LoadingUI(const _tchar* pLayerTag);

public:
	HRESULT Temp_Stage_Loader(const _tchar* _DatPath);

private:
	_short				m_sStageOptionIdx = 0;

private:
	CLoading*			m_pLoading = nullptr;

public:
	static CScene_Title* Create(_Device pGraphic_Device, _short _sStageNum, _bool _bLoadStatic);
	virtual void Free();
};

END