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
	btScalar GetMass(void) const { return 8.0f; }  // ���ʂ̎擾

private:
	static constexpr float RESPAWN_HEIGHT = -300.0f;
	D3DXVECTOR3 m_ResPos;	// ���X�|�[���ʒu
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

//*****************************************************************************
// �M�A�u���b�N�N���X
//*****************************************************************************
class CGearBlock : public CBlock
{
public:
	CGearBlock();
	~CGearBlock();

	HRESULT Init(void);
	void Update(void);

	// �R���C�_�[
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		// half extents�i���a�ƍ����̔����j
		btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.x * 0.5f);

		// Y�������̉~��
		return new btCylinderShape(halfExtents);
	}

	void SetRotDir(float dir) { m_rotDir = dir; } // �O��������w��p
	void SetRotSpeed(float fSpeed) { m_rotSpeed = fSpeed; }
	void SetAmplitude(float fAmplitude) { m_MoveAmplitude = fAmplitude; }
	void SetPeriod(float fPeriod) { m_MovePeriod = fPeriod; }

	float GetRotDir(void) const { return m_rotDir; }
	float GetRotSpeed(void) const { return m_rotSpeed; }
	float GetAmplitude(void) const { return m_MoveAmplitude; }
	float GetPeriod(void) const { return m_MovePeriod; }

private:
	D3DXVECTOR3 m_initPos;	// �����ʒu
	float m_rotDir;			// ��]�����i+1:����] / -1:�t��]�j
	float m_rotSpeed;		// ��]�X�s�[�h
	int m_nMoveCounter;		// �ړ��J�E���^�[
	float m_MoveAmplitude;	// �}�ړ���
	float m_MovePeriod;		// �����t���[��
};

//*****************************************************************************
// �M�A���u���b�N�N���X
//*****************************************************************************
class CGearPillarBlock : public CBlock
{
public:
	CGearPillarBlock();
	~CGearPillarBlock();

	void Update(void);

	// �R���C�_�[
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		// half extents�i���a�ƍ����̔����j
		btVector3 halfExtents(size.x * 0.5f, size.y * 0.5f, size.x * 0.5f);

		// Y�������̉~��
		return new btCylinderShape(halfExtents);
	}

private:

};

//*****************************************************************************
// �v���X�u���b�N�N���X
//*****************************************************************************
class CPressBlock : public CBlock
{
public:
	CPressBlock();
	~CPressBlock();

	// �v���X�@�̏��
	typedef enum
	{
		IDLE = 0,	// �ҋ@
		PRESSING,	// �����o��
		RETRACTING,	// �߂�
		MAX
	}STATE;

	HRESULT Init(void);
	void Update(void);

private:
	static constexpr float PRESS_DISTANCE	= 180.0f;	// �����o������
	static constexpr float PRESS_SPEED		= 6.0f;		// �ړ��X�s�[�h
	static constexpr float BACK_SPEED		= 3.0f;		// �߂�X�s�[�h
	static constexpr float WAIT_TIME		= 60.0f;	// �߂�����̑ҋ@���ԁi�t���[���j

	D3DXVECTOR3 m_initPos;	// �����ʒu
	float m_offSet;			// �I�t�Z�b�g�̒l
	float m_waitTimer;		// �ҋ@�^�C�}�[
	STATE m_state;	// ���
};

//*****************************************************************************
// �v���y���{�f�B�u���b�N�N���X
//*****************************************************************************
class CPropellerBodyBlock : public CBlock
{
public:
	CPropellerBodyBlock();
	~CPropellerBodyBlock();

	void Update(void);

	// �R���C�_�[
	btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size) override
	{
		// half extents�i���a�ƍ����̔����j
		btVector3 halfExtents(size.x * 0.5f, size.x * 0.5f, size.y * 0.2f);

		// Y�������̉~��
		return new btCylinderShapeZ(halfExtents);
	}

private:

};

//*****************************************************************************
// �v���y���u���b�N�N���X
//*****************************************************************************
class CPropellerWingBlock : public CBlock
{
public:
	CPropellerWingBlock();
	~CPropellerWingBlock();

	void Update(void);

private:

};

//*****************************************************************************
// �x���g�R���x�A�u���b�N�N���X
//*****************************************************************************
class CConveyerBlock : public CBlock
{
public:
	CConveyerBlock();
	~CConveyerBlock();

	void Update();

private:

};

//*****************************************************************************
// �������u���b�N�N���X
//*****************************************************************************
class CMoveBlock : public CBlock
{
public:
	CMoveBlock();
	~CMoveBlock();

	HRESULT Init(void);
	void Update(void);

private:
	D3DXVECTOR3 m_initPos;	// �����ʒu
	int m_nMoveCounter;		// �ړ��J�E���^�[
	float m_MoveAmplitude;	// �}�ړ���
	float m_MovePeriod;		// �����t���[��
};

#endif
