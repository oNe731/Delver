#include "stdafx.h"
#include "..\Header\Tree.h"

#include "Export_Function.h"

CTree::CTree(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
}

CTree::CTree(const CTree& rhs)
	: CGameObject(rhs)
{
}

CTree::~CTree()
{
}

HRESULT CTree::Ready_Object(void)
{
	m_eObjectTag = OBJECTTAG::IMMORTAL;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Scale(_vec3(10.f, 10.f, 10.f));
	m_pTransformCom->Translate(_vec3(0.f, 10.f, -20.f));

	return S_OK;
}

_int CTree::Update_Object(const _float & fTimeDelta)
{
	_int iExit = __super::Update_Object(fTimeDelta);

	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CTree::LateUpdate_Object(void)
{
	__super::LateUpdate_Object();

	//m_pBillBoardCom->LateUpdate_Component();
}

void CTree::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransformCom->WorldMatrix());

	m_pTextureCom->Render_Texture(0);
	m_pBufferCom->Render_Buffer();

	//////////////////////////////////////////////////////////////////////
	m_pTransformCom->Rotate(ROT_Y, D3DXToRadian(90.f));
	_matrix matWorld = m_pTransformCom->WorldMatrix();
	m_pTransformCom->Rotate(ROT_Y, D3DXToRadian(-90.f));
	
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Render_Texture(0);
	m_pBufferCom->Render_Buffer();

	//////////////////////////////////////////////////////////////////////
	m_pTransformCom->Rotate(ROT_Y, D3DXToRadian(45.f));
	matWorld = m_pTransformCom->WorldMatrix();
	m_pTransformCom->Rotate(ROT_Y, D3DXToRadian(-45.f));

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pTextureCom->Render_Texture(3);
	m_pBufferCom->Render_Buffer();

}

HRESULT CTree::Add_Component(void)
{
	CComponent*			pComponent = nullptr;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Tree"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	for (int i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CTree* CTree::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTree*		pInstance = new CTree(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CTree Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CTree::Free()
{
	__super::Free();
}
