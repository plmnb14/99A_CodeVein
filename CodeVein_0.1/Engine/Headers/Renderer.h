#pragma once

// ȭ�鿡 �׷������ϴ� ��ü���� �׸��� ������� ��Ƽ� ������ ��ü.


#include "Component.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderer() = default;
public:
	virtual HRESULT Ready_Component_Prototype();
	virtual HRESULT Ready_Component(void* pArg);
public:
	HRESULT Add_RenderList(RENDERID eGroup, CGameObject* pGameObject);
	HRESULT Draw_RenderList();
private:
	list<CGameObject*>			m_RenderList[RENDER_END];
	typedef list<CGameObject*>	RENDERLIST;
private:
	CTarget_Manager*			m_pTarget_Manager = nullptr;
	CLight_Manager*				m_pLight_Manager = nullptr;
private:
	CShader*					m_pShader_LightAcc = nullptr;
	CShader*					m_pShader_Blend = nullptr;
	CBuffer_ViewPort*			m_pViewPortBuffer = nullptr;
private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_Distortion();
	HRESULT Render_Alpha();
	HRESULT Render_UI();
private:
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
	HRESULT Render_After();
public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END


