#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CNormalGenji final : public CGameObject
{
public:
	//// ��, ��, ���п� ��
	//enum Weapon { Gun, Sword, Shield };
	//enum Color { Normal, Jungle, White };

	//typedef struct tagInitInfo
	//{
	//	tagInitInfo(Weapon _eWeapon, Color _eColor)
	//		: eWeapon(_eWeapon), eColor(_eColor)
	//	{}

	//	Weapon	eWeapon;
	//	Color	eColor;
	//}INFO;

protected:
	explicit CNormalGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CNormalGenji(const CNormalGenji& rhs);
	virtual ~CNormalGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//�������� Ÿ�ӵ�Ÿ �������ؼ� �����س���
	_double				m_dTimeDelta = 0;

	////���� ����
	//Weapon				m_eWeapon = Gun;
	//Color				m_eColor = Normal;

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CNormalGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END