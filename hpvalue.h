//=============================================================================
//
// �X�R�A���� [hpvalue.h]
// Author : RIKU TANEKAWA �� NAGAO YUSEI
//
//=============================================================================
#ifndef _HPVAL_H_// ���̃}�N����`������Ă��Ȃ�������
#define _HPVAL_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "number.h"
#include "object.h"


//*****************************************************************************
// �X�R�A�N���X
//*****************************************************************************
class CHPValue : public CObject
{
public:
	CHPValue(int nPriority = 7);
	~CHPValue();

	static CHPValue* Create(float baseX, float baseY, float digitWidth, float digitHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	void SetHPValue(int nHPValue);
	static void AddHPValue(int nValue);
	int DigitNum(int nHPValue);

private:
	static const int MAX_DIGITS = 3;		// ����
	CNumber* m_apNumber[MAX_DIGITS];		// �e���̐����\���p
	static int m_nHPValue;					// �X�R�A
	static int m_nDig[MAX_DIGITS];
	float m_digitWidth;						// ����1��������̕�
	float m_digitHeight;					// ����1��������̍���
	D3DXVECTOR3 m_basePos;					// �\���̊J�n�ʒu
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X

};

#endif