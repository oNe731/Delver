
#include "stdafx.h"

#include "..\Header\TempItem.h"
#include "Export_Function.h"
#include "Player.h"
#include "EffectSquare.h"
#include "EffectDamage.h"

static _int g_iCount = 0;

CTempItem::CTempItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CItem(pGraphicDev)
{
}

CTempItem::CTempItem(const CTempItem& rhs)
	: Engine::CItem(rhs)
{
}

CTempItem::~CTempItem()
{
	Free();
}

HRESULT CTempItem::Ready_Object(_bool _Item)
{
	m_eObjectTag = OBJECTTAG::ITEM;
	m_bWorldItem = _Item;
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	if (!Get_WorldItem())
	{
		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));
		m_pCollider->
			InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

		m_pBasicStat->Get_Stat()->fAttack = 1.f;
		m_pBasicStat->Get_Stat()->fHealth = 20.f;

		m_iAttackTick = 7;

		// 타입 및 아이디 지정
		m_ItemID.eItemType = ITEMTYPE_WEAPONITEM;
		m_ItemID.eItemID = WEAPON_SWORD;
		m_ItemID.iCount = 1;

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pTransform->m_pParent->Get_Host());
		//m_pTransform->Translate(pPlayer->m_pTransform->m_vInfo[INFO_POS] + *dynamic_cast<CPlayer*>(pPlayer)->Get_Offset());

#pragma region ksg

		CTransform* pPlayerTransform = pPlayer->m_pTransform;

		_vec3 vOffSet = 0.7f * pPlayerTransform->m_vInfo[INFO_RIGHT] + 1.5f * pPlayerTransform->m_vInfo[INFO_LOOK] - 0.4f * pPlayerTransform->m_vInfo[INFO_UP];
		m_pTransform->m_vInfo[INFO_POS] = (pPlayerTransform->m_vInfo[INFO_POS] + vOffSet);

#pragma endregion ksg
	}
	else
	{
		m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));
		m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

		m_pTransform->Translate(_vec3(0.0f, 2.f, 0.0f));

		m_pBasicStat->Get_Stat()->fAttack = 1.f;
		m_pBasicStat->Get_Stat()->fHealth = 20.f;

		m_iAttackTick = 7;

		// 타입 및 아이디 지정
		m_ItemID.eItemType = ITEMTYPE_WEAPONITEM;
		m_ItemID.eItemID = WEAPON_SWORD;
		m_ItemID.iCount = 1;
	}

	return S_OK;
}

_int CTempItem::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this);

	if (SceneManager()->Get_GameStop()) { return 0; }

	_int iExit = __super::Update_Object(fTimeDelta);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(SceneManager()->GetInstance()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());
	CItem* ItemType = dynamic_cast<CItem*>(pPlayer->Get_CurrentEquipRight());
	ITEMTYPEID ItemID = {};

	if (ItemType != nullptr)
		ItemID = ItemType->Get_ItemTag();

	if (ItemID.eItemID != ITEMID::WEAPON_SWORD || !pPlayer->Get_ItemEquipRight())
		return iExit;

	if (!Get_WorldItem())
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pTransform->m_pParent->Get_Host());

#pragma region ksg
		if (pPlayer->Get_Attack() && pPlayer != nullptr)
		{
			if (m_iAttackTick > 0)
				m_pTransform->Translate(m_pTransform->m_vInfo[INFO_LOOK] * 0.1f);
			else
				m_pTransform->Translate(m_pTransform->m_vInfo[INFO_LOOK] * -0.1f);

			--m_iAttackTick;

			if (-6 == m_iAttackTick)
			{
				m_iAttackTick = 7;
				pPlayer->Set_Attack(false);

				CTransform* pPlayerTransform = pPlayer->m_pTransform;

				_vec3 vOffSet = 0.7f * pPlayerTransform->m_vInfo[INFO_RIGHT] + 1.5f * pPlayerTransform->m_vInfo[INFO_LOOK] - 0.4f * pPlayerTransform->m_vInfo[INFO_UP];
				m_pTransform->m_vInfo[INFO_POS] = (pPlayerTransform->m_vInfo[INFO_POS] + vOffSet);
			}
		}
#pragma endregion ksg

	}

	return iExit;
}

void CTempItem::LateUpdate_Object(void)
{
	if (SceneManager()->Get_GameStop()) { return; }


	__super::LateUpdate_Object();
	m_pTransform->Scale(_vec3(0.3f, 0.3f, 0.3f));

}


void CTempItem::Render_Object(void)
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(SceneManager()->GetInstance()->
		Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());

	if (!Get_WorldItem() && pPlayer != nullptr)
	{
		CItem* ItemType = dynamic_cast<CItem*>(pPlayer->Get_CurrentEquipRight());

		ITEMTYPEID ItemID = {};

		if (ItemType != nullptr)
			ItemID = ItemType->Get_ItemTag();

		if (ItemID.eItemID == ITEMID::WEAPON_SWORD && pPlayer->Get_ItemEquipRight())
		{
			m_pTexture->Render_Texture();
			m_pBuffer->Render_Buffer();
		}
	}
	else
	{
		m_pTexture->Render_Texture();
		m_pBuffer->Render_Buffer();
	}

#if _DEBUG
	m_pCollider->Render_Collider();
#endif
}

HRESULT CTempItem::Add_Component(void)
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);

	pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pTexture = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_Sword"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);

	pComponent = m_pCollider = dynamic_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pBasicStat = dynamic_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	if (!Get_WorldItem())
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());

		m_pTransform->Set_Parent(SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front()->m_pTransform);
		m_pTransform->Copy_RUL(SceneManager()->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front()->m_pTransform->m_vInfo);

		for (_int i = 0; i < ID_END; ++i)
			for (auto& iter : m_mapComponent[i])
				iter.second->Init_Property(this);
	}
	else
	{
		pComponent = m_pBillBoard = dynamic_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
		NULL_CHECK_RETURN(pComponent, E_FAIL);
		m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

		for (int i = 0; i < ID_END; ++i)
			for (auto& iter : m_mapComponent[i])
				iter.second->Init_Property(this);
	}

	return S_OK;
}

void CTempItem::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	if (!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::MONSTER) &&
		!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::PLAYER))
		__super::OnCollisionEnter(_pOther);
	// 몬스터거나 플레이어면 밀어내지않는다.

	CPlayer& pPlayer = *dynamic_cast<CPlayer*>(SceneManager()->GetInstance()
		->Get_ObjectList(LAYERTAG::GAMELOGIC, OBJECTTAG::PLAYER).front());
	// 플레이어의 정보를 레퍼런스로 얻어옴.

	if (_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::MONSTER)
		// 무기 콜리전에 들어온 타입이 몬스터이면서, 플레이어의 스테이트가 공격이라면
	{
		if (!pPlayer.Get_AttackTick() &&
			dynamic_cast<CMonster*>(_pOther->Get_Host())->Get_StateMachine()->Get_State() != STATE::DEAD)
			// 공격 하지 않은 상태라면.
		{
			dynamic_cast<CMonster*>(_pOther->Get_Host())->Get_BasicStat()->Take_Damage(1.f);
			pPlayer.Set_AttackTick(true);

			++g_iCount;

			if (dynamic_cast<CMonster*>(_pOther->Get_Host())->Get_StateMachine()->Get_PrevState() != STATE::HIT
				&& g_iCount > 4)
			{
				g_iCount = 0;
				dynamic_cast<CMonster*>(_pOther->Get_Host())->Get_StateMachine()->Set_State(STATE::HIT);
			}

			//////////////////////////////////////// 이펙트 추가
			_matrix      matMonsterWorld  = _pOther->GetHost()->m_pTransform->WorldMatrix();
			_vec3        vecMonsterPos    = _vec3(matMonsterWorld._41, matMonsterWorld._42 + .5f, matMonsterWorld._43);
			CGameObject* pGameObject      = nullptr;

			// 이펙트 생성
			switch (dynamic_cast<CMonster*>(_pOther->Get_Host())->Get_MonsterTag())
			{
			case MONSTERTAG::SPIDER:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_BROWN);
				break;
			case MONSTERTAG::WARRIOR:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_RED);
				break;
			case MONSTERTAG::BAT:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_BROWN);
				break;
			case MONSTERTAG::WIZARD:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_APRICOT);
				break;
			case MONSTERTAG::ALIEN:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_PINK);
				break;
			case MONSTERTAG::SLIME:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_GREEN);
				break;
			case MONSTERTAG::SKELETON:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_APRICOT);
				break;
			case MONSTERTAG::BONEGHOST:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_WHITE);
				break;
			case MONSTERTAG::WORM:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_RED);
				break;
			default:
				pGameObject = CEffectSquare::Create(m_pGraphicDev, vecMonsterPos, 50, EFFECTCOLOR::ECOLOR_RED);
				break;
			}
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);

			pGameObject = CEffectDamage::Create(m_pGraphicDev);
			pGameObject->m_pTransform->Translate(_vec3(vecMonsterPos.x, vecMonsterPos.y - 0.5f, vecMonsterPos.z + .5f));
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);
			//////////////////////////////////////// 이펙트 추가

			cout << "데미지" << endl;
		}
	}
}

void CTempItem::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }


	if (!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::MONSTER) &&
		!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::PLAYER))
		__super::OnCollisionStay(_pOther);
}

void CTempItem::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }


	if (!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::MONSTER) &&
		!(_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::PLAYER))
		__super::OnCollisionExit(_pOther);
}

CTempItem* CTempItem::Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool _Item)
{
	CTempItem* pInstance = new CTempItem(pGraphicDev);

	if (FAILED(pInstance->Ready_Object(_Item)))
	{
		Safe_Release(pInstance);

		MSG_BOX("TempItem Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CTempItem::Free()
{
	__super::Free();
}