#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CItem :
    public CGameObject
{
protected:
    explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CItem(const CItem& rhs);
    virtual ~CItem();

public:
    HRESULT     Ready_Object();
    _int        Update_Object(const _float& fTimeDelta);

protected:
    // ITEM_TAG m_eItemTag;
    // ITEM_ID m_eItemID;    // �ʿ��ϴٸ� �߰�
    // MonsterAI Component �߰��ؼ� ���
    // 
// CGameObject�κ��� ���
    //map<COMPONENTTAG, CComponent*>	m_mapComponent[ID_END];
    //LPDIRECT3DDEVICE9					m_pGraphicDev;
    //_bool								m_IsDead;

    //OBJECTTAG							m_eObjectTag;
    //CTransform*                       m_pTransform;	// Transform Attached to this GameObject
protected:
    virtual void	Free(void);
};

END