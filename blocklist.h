//=============================================================================
//
// �u���b�N���X�g���� [blocklist.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"

//*****************************************************************************
// �ؔ��u���b�N�N���X
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	CWoodBoxBlock();
	~CWoodBoxBlock();

	HRESULT Init(void);
	void Update(void);
	void Respawn(D3DXVECTOR3 resPos) override;
	void Set(D3DXVECTOR3 pos);

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 4.0f; }  // ���ʂ̎擾

private:
	static constexpr float RESPAWN_HEIGHT = -810.0f;
	D3DXVECTOR3 m_ResPos;	// ���X�|�[���ʒu
};


//*****************************************************************************
// ��u���b�N�N���X
//*****************************************************************************
class CRockBlock : public CBlock
{
public:
	CRockBlock();
	~CRockBlock();

	void Update(void);
	void Respawn(D3DXVECTOR3 resPos) override;
	void AddPathPoint(const D3DXVECTOR3& point);// �`�F�b�N�|�C���g�ǉ� (�ʏ펞�p)
	void MoveToTarget(void);					// �]��������
	void IsPlayerHit(void);						// �v���C���[�Ƃ̐ڐG����
	D3DXVECTOR3 GetVelocity(void) override { return m_dir; }

	// �R���C�_�[
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		float radius = std::max({ size.x, size.y, size.z }) * 0.5f;
		return new btSphereShape(radius);
	}
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	bool IsDynamicBlock(void) const override { return true; }
	btScalar GetMass(void) const { return 100.0f; }  // ���ʂ̎擾

private:
	static constexpr float RESET_HEIGHT = -480.0f;// ���X�|�[�����鍂��
	std::vector<D3DXVECTOR3> m_pathPoints;		// �`�F�b�N�|�C���g�̔z�� (����p)
	int m_currentTargetIndex;					// ���̖ڕW�n�_�C���f�b�N�X
	float m_speed;								// �͂̋����i���x�̑���j
	bool m_isBridgeSwitchOn;
	bool m_isHit;
	bool m_isPrevHit;
	bool m_isThrough;							// �ʉ߂�����
	bool m_isPrevThrough;						// ���O�ɒʉ߂�����
	int m_effectTimer;							// �^�C�}�[
	const int ROLL_EFFECT_INTERVAL = 5;			// �G�t�F�N�g�����Ԋu�i�t���[�����j
	int m_playedRollSoundID;					// �Đ����̉�ID
	D3DXVECTOR3 m_dir;							// ��̐i�s�����x�N�g��
};

//*****************************************************************************
// �V�[�\�[�u���b�N�N���X
//*****************************************************************************
class CSeesawBlock : public CBlock
{
public:
	CSeesawBlock();
	~CSeesawBlock();

	HRESULT Init(void);
	void Uninit(void);
	void RemoveHinge(void);
	void Update(void);
	void OnPhysicsCreated(void) override
	{
		// �q���W�̐ݒ菈��
		SetHinge();
	}
	void OnPhysicsReleased(void) override
	{
		// �q���W�̔j��
		RemoveHinge();
	}

	void SetHinge(void);
	btHingeConstraint* GetHinge(void) { return m_pHinge; }

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetLinearFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetFriction(void) const { return 5.7f; }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	btScalar GetMass(void) const { return 10.0f; }// ���ʂ̎擾

private:
	btHingeConstraint* m_pHinge;
};

#endif
