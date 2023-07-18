#include "..\Header\SkullGhost.h"
#include "Export_Function.h"
#include "Monster_Move.h"
#include "Ghost_IDLE.h"
#include "Ghost_Attack.h"
#include "Charge.h"
#include "Player.h"
#include "Monster_Fly.h"
#include "Warrior_Attack.h"
#include "EffectBlood.h"

#include "PoolManager.h"

CSkullGhost::CSkullGhost(LPDIRECT3DDEVICE9 pGrapicDev)
	: CMonster(pGrapicDev)
{
}

CSkullGhost::CSkullGhost(const CSkullGhost& rhs)
	: CMonster(rhs)
{
}

CSkullGhost::~CSkullGhost()
{
}

HRESULT CSkullGhost::Ready_Object()
{
	Set_ObjectTag(OBJECTTAG::MONSTER);
	Set_MonsterState(MONSTERTAG::SKULLGHOST);
	m_fSpeed = 30.f;

	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	CState* pState = CMonster_Move::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::ROMIMG, pState);
	pState = CGhost_Attack::Create(m_pGraphicDev, m_pStateMachine);
	m_pStateMachine->Add_State(STATE::ATTACK, pState);

	CAnimation* pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ROMIMG], STATE::ROMIMG, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ROMIMG, pAnimation);
	pAnimation = CAnimation::Create(m_pGraphicDev,
		m_pTexture[(_uint)STATE::ATTACK], STATE::ATTACK, 5.f, TRUE);
	m_pAnimator->Add_Animation(STATE::ATTACK, pAnimation);

	Set_CenterPos(_vec3(-70.f, 0.f, -40.f));

	m_pStateMachine->Set_Animator(m_pAnimator);
	m_pStateMachine->Set_State(STATE::ROMIMG);

	m_pTransform->Scale(_vec3(1.f, 1.f, 1.f));
	m_pCollider->InitOBB(m_pTransform->m_vInfo[INFO_POS], &m_pTransform->m_vInfo[INFO_RIGHT], m_pTransform->LocalScale());

	m_pTransform->Translate(_vec3(-10.f, 3.f, 10.f));

#pragma region SKULLGHOST
	Init_Stat();
#pragma endregion

	return S_OK;
}

_int CSkullGhost::Update_Object(const _float& fTimeDelta)
{
	Engine::Renderer()->Add_RenderGroup(RENDER_ALPHA, this); // ! ������ �� ���� �÷� ������ �ǵ��� �ϰ�


	if (SceneManager()->Get_GameStop()) { return 0; } // ! Esc �� MŰ ���� �� ������Ʈ ���߰� �ϴ� �뵵 �Դϴ�.

	_int iExit = __super::Update_Object(fTimeDelta);

	if (m_pBasicStat->Get_Stat()->fHealth <= 0)
	{
		m_pStateMachine->Set_State(STATE::DEAD);

		if (!m_bDieEffect)
		{
			CGameObject* pGameObject = CEffectBlood::Create(m_pGraphicDev);
			pGameObject->m_pTransform->Translate(_vec3(m_pTransform->m_vInfo[INFO_POS].x, m_pTransform->m_vInfo[INFO_POS].y - 1.f, m_pTransform->m_vInfo[INFO_POS].z));
			dynamic_cast<CTempEffect*>(pGameObject)->Set_EffectColor(ECOLOR_RED);
			Engine::EventManager()->CreateObject(pGameObject, LAYERTAG::GAMELOGIC);

			m_bDieEffect = true;
		}
	}

	m_pStateMachine->Update_StateMachine(fTimeDelta);


	return iExit;
}

void CSkullGhost::LateUpdate_Object()
{
	if (SceneManager()->Get_GameStop()) { return; } // ! Esc �� MŰ ���� �� ������Ʈ ���߰� �ϴ� �뵵 �Դϴ�.

	__super::LateUpdate_Object();
	//m_pTransform->Scale(_vec3(5.f, 5.f, 5.f));
}

void CSkullGhost::Render_Object()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &m_pTransform->WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pStateMachine->Render_StateMachine();
	m_pBuffer->Render_Buffer();

	
#if _DEBUG
	m_pCollider->Render_Collider();
#endif
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CSkullGhost::Init_Stat()
{
	m_pBasicStat->Get_Stat()->fMaxHP = 4.f;
	m_pBasicStat->Get_Stat()->fHP = 4.f;
	m_pBasicStat->Get_Stat()->iDamageMin = 1;
	m_pBasicStat->Get_Stat()->iDamageMax = 2;
	m_pBasicStat->Get_Stat()->fSpeed = 4.f;
	m_pBasicStat->Get_Stat()->fAgility = 4.f;
	m_pBasicStat->Get_Stat()->fDeffense = 4.f;
	m_pBasicStat->Get_Stat()->fMagic = 4.f;
	m_pBasicStat->Get_Stat()->fAttack = 4.f;
	m_pBasicStat->Get_Stat()->iExp = 6.f;
}

void CSkullGhost::OnCollisionEnter(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }


	if (_pOther->GetHost()->Get_ObjectTag() != OBJECTTAG::PLAYER
		&& this->Get_StateMachine()->Get_State() != STATE::DEAD &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::ITEM)
		__super::OnCollisionEnter(_pOther);

	// �浹 �о�� �� �̺�Ʈ : �����Ͻø� �˴ϴ�.

	if (_pOther->GetHost()->Get_ObjectTag() == OBJECTTAG::PLAYER
		&& this->Get_StateMachine()->Get_State() == STATE::ATTACK)
	{
		CPlayerStat& PlayerState = *static_cast<CPlayer*>(_pOther->GetHost())->Get_Stat();

		if (!this->Get_AttackTick())
		{
			PlayerState.Take_Damage(this->Get_BasicStat()->Get_Stat()->fAttack);
			this->Set_AttackTick(true);

			cout << "�ذ� ����" << endl;
		}

	}
}

void CSkullGhost::OnCollisionStay(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }
	// �浹 �о�� �� �̺�Ʈ : �����Ͻø� �˴ϴ�.
	if (_pOther->GetHost()->Get_ObjectTag() != OBJECTTAG::PLAYER &&
		this->Get_StateMachine()->Get_State() != STATE::DEAD &&
		_pOther->Get_Host()->Get_ObjectTag() != OBJECTTAG::ITEM)
		__super::OnCollisionStay(_pOther);


}

void CSkullGhost::OnCollisionExit(CCollider* _pOther)
{
	if (SceneManager()->Get_GameStop()) { return; }

	__super::OnCollisionExit(_pOther);
}

HRESULT CSkullGhost::Add_Component()
{
	CComponent* pComponent = nullptr;

	pComponent = m_pBuffer = dynamic_cast<CRcTex*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BUFFER, pComponent);


	// Monster Animation Texture 
	//pComponent = m_pTexture[(_uint)STATE::IDLE] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_SKULLGHOST"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::ROMIMG] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_SKULLGHOST"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::ATTACK] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_SKULLGHOSTAttack"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::HIT] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_SKULLGHOSTHit"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);
	pComponent = m_pTexture[(_uint)STATE::DEAD] = dynamic_cast<CTexture*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Texture_SKULLGHOSTDead"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::TEXTURE0, pComponent);


	pComponent = m_pTransform = dynamic_cast<CTransform*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::TRANSFORM, pComponent);

	pComponent = m_pCollider = dynamic_cast<CCollider*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Collider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::COLLIDER, pComponent);

	pComponent = m_pRigidBody = dynamic_cast<CRigidBody*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_RigidBody"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::RIGIDBODY, pComponent);

	pComponent = dynamic_cast<CBillBoard*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BillBoard"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].emplace(COMPONENTTAG::BILLBOARD, pComponent);

	pComponent = m_pStateMachine = dynamic_cast<CStateMachine*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_State"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::STATEMACHINE, pComponent);

	pComponent = m_pBasicStat = dynamic_cast<CBasicStat*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_BasicStat"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::BASICSTAT, pComponent);

	pComponent = m_pAnimator = dynamic_cast<CAnimator*>(Engine::PrototypeManager()->Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].emplace(COMPONENTTAG::ANIMATOR, pComponent);


	for (_uint i = 0; i < ID_END; ++i)
		for (auto& iter : m_mapComponent[i])
			iter.second->Init_Property(this);

	return S_OK;
}

CSkullGhost* CSkullGhost::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkullGhost* pInstance = new CSkullGhost(pGraphicDev);

	if (FAILED(pInstance->Ready_Object()))
	{
		Safe_Release<CSkullGhost*>(pInstance);

		MSG_BOX("Create Ghost Failed");
		return nullptr;
	}

	return pInstance;
}

void CSkullGhost::Free()
{
	__super::Free();
}