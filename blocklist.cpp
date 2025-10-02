//=============================================================================
//
// �u���b�N���X�g���� [blocklist.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "blocklist.h"
#include "blockmanager.h"
#include "player.h"
#include "game.h"
#include "manager.h"
#include "particle.h"
#include <algorithm>

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �ؔ��u���b�N�̃R���X�g���N�^
//=============================================================================
CWoodBoxBlock::CWoodBoxBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;// ���X�|�[���ʒu
}
//=============================================================================
// �ؔ��u���b�N�̃f�X�g���N�^
//=============================================================================
CWoodBoxBlock::~CWoodBoxBlock()
{
	// �Ȃ�
}
//=============================================================================
// �ؔ��u���b�N�̏���������
//=============================================================================
HRESULT CWoodBoxBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu�����X�|�[���ʒu�ɐݒ�
	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// �ؔ��u���b�N�̍X�V����
//=============================================================================
void CWoodBoxBlock::Update()
{
	CBlock::Update(); // ���ʏ���
}
//=============================================================================
// ���X�|�[������
//=============================================================================
void CWoodBoxBlock::Respawn(D3DXVECTOR3 resPos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// �����Ԃ��p�[�e�B�N������
	CParticle::Create<CWaterParticle>(INIT_VEC3, GetPos(), D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, 10);
	CParticle::Create<CWaterParticle>(INIT_VEC3, GetPos(), D3DXCOLOR(0.3f, 0.5f, 1.0f, 0.5f), 50, 10);

	// ���X�|�[���ʒu�ɐݒ�
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// �R���C�_�[�̍X�V
	UpdateCollider();

	// ���I�ɖ߂�
	SetEditMode(false);
}
//=============================================================================
// �Z�b�g����
//=============================================================================
void CWoodBoxBlock::Set(D3DXVECTOR3 pos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ���u���b�N�̈ʒu���擾
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// �ݒu����ڕW�ʒu
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// ���������Z�b�g

	Pos = targetPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// �R���C�_�[�̍X�V
	UpdateCollider();
}


//=============================================================================
// ��u���b�N�̃R���X�g���N�^
//=============================================================================
CRockBlock::CRockBlock()
{
	// �l�̃N���A
	m_pathPoints = {};
	m_currentTargetIndex = 0;
	m_speed = 86500.0f;
	m_isBridgeSwitchOn = false;
	m_isHit = false;
	m_isPrevHit = false;
	m_isThrough = false;
	m_isPrevThrough = false;
	m_effectTimer = 0;
	m_playedRollSoundID = -1;
	m_dir = INIT_VEC3;
}
//=============================================================================
// ��u���b�N�̃f�X�g���N�^
//=============================================================================
CRockBlock::~CRockBlock()
{
	// �Ȃ�
}
//=============================================================================
// ��u���b�N�̍X�V����
//=============================================================================
void CRockBlock::Update(void)
{
	CBlock::Update();	// ���ʏ���

	if (!CGame::GetPlayer())
	{
		return;
	}

	if (GetPos().y < RESET_HEIGHT)
	{
		// ���X�|�[������
		Respawn(D3DXVECTOR3(2815.5f, 670.0f, -1989.0f));
	}

	CManager::GetCamera()->SetShakeTargetPos(GetPos());// �J�����U���p�̑Ώۈʒu

	if (m_currentTargetIndex < 8)
	{
		// �J�����V�F�C�NON
		CManager::GetCamera()->SetCameraShake(true);
	}
	else
	{
		// �J�����V�F�C�NOFF
		CManager::GetCamera()->SetCameraShake(false);
	}

	//MoveToTarget();		// �`�F�b�N�|�C���g�֌����Ĉړ�

	//IsPlayerHit();		// �v���C���[�Ƃ̐ڐG����
}
//=============================================================================
// ���X�|�[������
//=============================================================================
void CRockBlock::Respawn(D3DXVECTOR3 resPos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ���X�|�[���ʒu�ɐݒ�
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0, 0, 0));

	// �R���C�_�[�̍X�V
	UpdateCollider();

	// ���݂̃`�F�b�N�|�C���g�C���f�b�N�X�����Z�b�g����
	m_currentTargetIndex = 0;

	// ���I�ɖ߂�
	SetEditMode(false);
}
//=============================================================================
// �ʉ߃|�C���g�ǉ�����
//=============================================================================
void CRockBlock::AddPathPoint(const D3DXVECTOR3& point)
{
	m_pathPoints.push_back(point);
}
//=============================================================================
// �ڕW�Ɍ������Ĉړ����鏈��
//=============================================================================
void CRockBlock::MoveToTarget(void)
{
	if (m_pathPoints.empty() || m_currentTargetIndex >= (int)m_pathPoints.size())
	{
		return;
	}

	btRigidBody* pRigid = GetRigidBody();

	if (!pRigid)
	{
		return;
	}

	CModelEffect* pModelEffect = nullptr;

	if (m_currentTargetIndex >= 1)
	{
		m_effectTimer++;

		if (m_effectTimer >= ROLL_EFFECT_INTERVAL)
		{// �G�t�F�N�g�������o
			m_effectTimer = 0;

			// �����_���Ȋp�x�ŉ��ɍL����
			float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
			float speed = (rand() % 200) / 30.0f + 0.2f;

			// �ړ���
			D3DXVECTOR3 move;
			move.x = cosf(angle) * speed;
			move.z = sinf(angle) * speed;
			move.y = (rand() % 300) / 50.0f + 0.05f; // �������������

			// ����
			D3DXVECTOR3 rot;
			rot.x = ((rand() % 360) / 180.0f) * D3DX_PI;
			rot.y = ((rand() % 360) / 180.0f) * D3DX_PI;
			rot.z = ((rand() % 360) / 180.0f) * D3DX_PI;

			// �T�C�Y
			D3DXVECTOR3 size;
			size.x = 0.5f + (rand() % 5) * 0.1f;
			size.y = 0.5f + (rand() % 5) * 0.1f;
			size.z = 0.5f + (rand() % 5) * 0.1f;

			// ���f���G�t�F�N�g�̐���
			pModelEffect = CModelEffect::Create("data/MODELS/effectModel_rock.x", D3DXVECTOR3(GetPos().x, GetPos().y - 200.0f, GetPos().z),
				rot, move, size, 180, -0.12f, 0.008f);
		}
	}

	D3DXVECTOR3 currentPos = GetPos();
	D3DXVECTOR3 targetPos = m_pathPoints[m_currentTargetIndex];

	// �^�[�Q�b�g�����x�N�g��
	m_dir = targetPos - currentPos;
	float dist = D3DXVec3Length(&m_dir);

	if (dist < 100.0f)  // ������x�߂Â����玟�̃|�C���g��
	{
		m_currentTargetIndex++;
		return;
	}

	// ���K��
	D3DXVec3Normalize(&m_dir, &m_dir);

	btVector3 force(0.0f, 0.0f, 0.0f);

	if (m_currentTargetIndex >= 1)
	{
		m_isThrough = true;
	}
	else
	{
		m_isThrough = false;
	}

	bool isThroughNow = m_isThrough;

	if (isThroughNow && !m_isPrevThrough)
	{
		// ���[�v�Đ�����ID��ۑ�
		if (m_playedRollSoundID == -1)
		{
			m_playedRollSoundID = CManager::GetSound()->Play3D(CSound::SOUND_LABEL_ROLL, currentPos, 850.0f, 1650.0f);
		}
	}

	m_isPrevThrough = isThroughNow;

	if (m_currentTargetIndex >= 6)
	{// �C���f�b�N�X�� 6 �𒴂�����

		float fSpeedDown = 0.001f;

		// ����������
		force = btVector3(m_dir.x * (m_speed * fSpeedDown), 0.0f, m_dir.z * (m_speed * fSpeedDown));

		if (m_currentTargetIndex >= 8 && m_playedRollSoundID != -1)
		{
			// �]���鉹�̒�~
			CManager::GetSound()->Stop(m_playedRollSoundID);
			m_playedRollSoundID = -1;
		}
	}
	else
	{// �ʏ�
		// Z�����S�œ]����
		force = btVector3(m_dir.x * m_speed, 0.0f, m_dir.z * m_speed);
	}

	// �����̈ʒu�X�V�i�Đ����̂݁j
	if (m_playedRollSoundID != -1)
	{
		CManager::GetSound()->UpdateSoundPosition(m_playedRollSoundID, currentPos);
	}

	// �K�p���̑��x�ɉ�����
	pRigid->applyCentralForce(force);
}
//=============================================================================
// �v���C���[�Ƃ̐ڐG���菈��
//=============================================================================
void CRockBlock::IsPlayerHit(void)
{
	if (CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();

	D3DXVECTOR3 playerPos = pPlayer->GetPos();
	D3DXVECTOR3 rockPos = GetPos();

	float playerRadius = pPlayer->GetRadius();  // �v���C���[�̓����蔻�蔼�a
	float rockRadius = 240.0f;					// ��̔��a

	float playerHeight = pPlayer->GetHeight();	// �v���C���[�̍���
	float rockHeight = 240.0f;					// ��̍����͈�

	// XZ�����`�F�b�N
	D3DXVECTOR2 diffXZ = D3DXVECTOR2(playerPos.x - rockPos.x, playerPos.z - rockPos.z);

	float distXZSq = D3DXVec2LengthSq(&diffXZ);
	float hitDistXZ = playerRadius + rockRadius;

	// Y���`�F�b�N
	float dy = fabsf(playerPos.y - rockPos.y);
	float hitHeight = (playerHeight * 0.5f) + rockHeight;

	bool isNowHit = m_isHit;

	if (isNowHit && !m_isPrevHit)
	{
		// �v���C���[�q�b�gSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_HIT);
	}

	// �v���C���[���������Ă��������L�^
	m_isPrevHit = isNowHit;

	if (distXZSq < (hitDistXZ * hitDistXZ) && dy < hitHeight)
	{
		m_isHit = true;

		// �v���C���[�̃��X�|�[��
		pPlayer->RespawnToCheckpoint(D3DXVECTOR3(2810.0f, 30.0f, -1518.0f));
	}
	else
	{
		m_isHit = false;
	}
}


//=============================================================================
// �V�[�\�[�u���b�N�̃R���X�g���N�^
//=============================================================================
CSeesawBlock::CSeesawBlock()
{
	// �l�̃N���A
	m_pHinge = nullptr;
}
//=============================================================================
// �V�[�\�[�u���b�N�̃f�X�g���N�^
//=============================================================================
CSeesawBlock::~CSeesawBlock()
{
	// �Ȃ�
}
//=============================================================================
// �V�[�\�[�u���b�N�̏���������
//=============================================================================
HRESULT CSeesawBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	return S_OK;
}
//=============================================================================
// �V�[�\�[�u���b�N�̏I������
//=============================================================================
void CSeesawBlock::Uninit(void)
{
	if (m_pHinge)
	{
		CManager::GetPhysicsWorld()->removeConstraint(m_pHinge);
		delete m_pHinge;
		m_pHinge = nullptr;
	}

	// �u���b�N�̏I������
	CBlock::Uninit();
}
//=============================================================================
// �V�[�\�[�u���b�N�̃q���W�폜����
//=============================================================================
void CSeesawBlock::RemoveHinge(void)
{
	if (m_pHinge)
	{
		// ���[���h����폜
		CManager::GetPhysicsWorld()->removeConstraint(m_pHinge);
		delete m_pHinge;
		m_pHinge = nullptr;
	}
}
//=============================================================================
// �V�[�\�[�u���b�N�̍X�V����
//=============================================================================
void CSeesawBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();
}
//=============================================================================
// �V�[�\�[�u���b�N�̃q���W�ݒ菈��
//=============================================================================
void CSeesawBlock::SetHinge(void)
{
	// ���W�b�h�{�f�B�̎擾
	btRigidBody* pRigid = GetRigidBody();

	if (!pRigid)
	{// null��������
		return;
	}

	// rigidbody �� transform ���擾
	btTransform transform;
	if (pRigid->getMotionState())
	{
		pRigid->getMotionState()->getWorldTransform(transform);
	}
	else
	{
		transform = pRigid->getCenterOfMassTransform();
	}

	// Z�������[�J�����Ƃ��ăq���W�ɐݒ�
	btVector3 localLongAxis(0, 0, 1);

	btVector3 pivot(0, 0, 0); // �q���W�̃s�{�b�g�i�u���b�N���S�j

	// �q���W�쐬�i���[�J����Ԏ��j
	m_pHinge = new btHingeConstraint(*pRigid, pivot, localLongAxis, true);

	// ��]����
	m_pHinge->setLimit(-SIMD_RADS_PER_DEG * 30.0f, SIMD_RADS_PER_DEG * 30.0f);

	// ���[���h�ɒǉ�
	CManager::GetPhysicsWorld()->addConstraint(m_pHinge, true);
}
