#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CSky final : public CGameObject
{
protected:
	explicit CSky(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSky(const CSky& rhs);
	virtual ~CSky() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype(); // 원복객체 생성 시, 호출될 함수.
	virtual HRESULT Ready_GameObject(void* pArg); // 인게임 객체 생성 시, 호출될 함수.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;	
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	//CBuffer_CubeTex*		m_pBufferCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
public:
	static CSky* Create(LPDIRECT3DDEVICE9 pGraphic_Device); // 원형객체 생성.
	virtual CGameObject* Clone_GameObject(void* pArg); // 복사객체 생성.
	virtual void Free();
};

END