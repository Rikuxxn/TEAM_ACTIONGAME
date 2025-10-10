//=============================================================================
//
// �Q�[�W���� [guage.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GUAGE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _GUAGE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"

//*****************************************************************************
// �Q�[�W�N���X
//*****************************************************************************
class CGuage : public CObject2D
{
public:
	// ���
	enum GUAGETYPE
	{
		TYPE_NONE = 0,	// �������Ă��Ȃ����
		TYPE_GUAGE,
		TYPE_FRAME,
		TYPE_MAX
	};

	CGuage();
	~CGuage();

	static CGuage* Create(D3DXVECTOR3 pos, GUAGETYPE type);
	HRESULT Init(D3DXVECTOR3 pos, GUAGETYPE type);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;			// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;							// �ʒu
	GUAGETYPE m_type;							// �Q�[�W�̎��
	D3DXCOLOR m_Col;							// �Q�[�W�̐F
	static constexpr float m_fWidth = 470.0f;	// ����
	static constexpr float m_fHeight = 65.0f;	// �c��
	static constexpr int m_nMax = 1000;			// �ő�l
	int m_nHp;									// HP
	int m_nFrame;
};

#endif