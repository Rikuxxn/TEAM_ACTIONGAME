//=============================================================================
//
// �Q�[�W���� [guage.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "guage.h"
#include "manager.h"
#include "score.h"
#include "player.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CGuage::m_nIdxTexture = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGuage::CGuage()
{
	// �l�̃N���A
	m_nIdxTexture = 0;
	m_type = TYPE_NONE;	// �Q�[�W�̎��
	//nHp = NULL;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CGuage::~CGuage()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CGuage* CGuage::Create(D3DXVECTOR3 pos, GUAGETYPE type)
{
	CGuage* pGuage = nullptr;

	pGuage = new CGuage;

	// ����������
	pGuage->SetPos(pos);
	pGuage->SetSize(m_fWidth, m_fHeight);
	if (type == TYPE_GUAGE)
	{
		pGuage->SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
	}
	if (type == TYPE_FRAME)
	{
		pGuage->SetCol(D3DCOLOR_RGBA(255, 255, 255, 255));
	}
	pGuage->m_type = type;
	pGuage->Init();

	return pGuage;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CGuage::Init(void)
{
	if (m_type == TYPE_FRAME)
	{
		// �e�N�X�`���ݒ�
		m_nIdxTexture = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/BatteryFrame.png");
	}

	//nHp = nMax;

	// �X�R�A�̐���
	CScore::Create(620.0f, 50.0f, 80.0f, 100.0f);

	// 2D�I�u�W�F�N�g�̏�����
	CObject2D::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CGuage::Uninit(void)
{
	// 2D�I�u�W�F�N�g�̏I������
	CObject2D::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CGuage::Update(void)
{
	static int nCount = 0;
	int nHp = CPlayer::GetHP();
	int nMax = CPlayer::GetMax();
	if (m_type == TYPE_GUAGE)
	{
		nCount++;
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();		// �L�[�{�[�h�̎擾
		float f, f3, f4;
		f = (float)nHp / (float)nMax;
#if 0
		f2 = f / 0.1f;
		f3 = f2;
#endif
#if 1
		int n = f / 0.1f;

		if (nHp >= nMax ||
			(int)(((float)nHp / (float)nMax) * 100) <= 0)
		{
			f3 = (float)n;
		}
		else
		{
			f3 = (float)n + 1;
		}

		f3 = (float)n + 1;
		if (f3 >= 11)
		{
			f3 = 10;
		}
		//f3 = (float)n;
#endif
		if (nHp >= 1)
		{
			f4 = f3 / 10;
		}
		else if(nHp > nMax)
		{
			f4 = 1.0f;
		}
		else
		{
			f4 = 0.0f;
		}

		// �̂���HP30%�ȏ�
		if (nHp >= (int)(nMax * 0.3f))
		{
			SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
		}
		// ����ȊO
		else
		{
			SetCol(D3DCOLOR_RGBA(255, 0, 0, 255));
		}

		CScore::SetScore((int)(((float)nHp / (float)nMax) * 100));

		//// �̂���HP20%�ȏ�
		//if (nHp >= (int)(nMax * 0.2f))
		//{
		//	float Col = 200.0f * ((float)nHp / (float)nMax);
		//	SetCol(D3DCOLOR_RGBA(255 - (int)Col, (int)Col + 55, 0, 255));
		//}
		//// ����ȊO
		//else
		//{
		//	// HP�̓_��
		//	if (nCount % 2 == 0)
		//	{
		//		SetCol(D3DCOLOR_RGBA(255, 0, 0, 255));
		//	}
		//	else
		//	{
		//		SetCol(D3DCOLOR_RGBA(255, 255, 255, 255));
		//	}
		//}

		// 2D�I�u�W�F�N�g�̍X�V����
		CObject2D::UpdateUI(f4, 52.0f);
	}
	else if (m_type == TYPE_FRAME)
	{
		// 2D�I�u�W�F�N�g�̍X�V����
		CObject2D::UpdateFrame(52.0f);
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGuage::Draw(void)
{
	if (m_type == TYPE_FRAME)
	{
		// �e�N�X�`���̎擾
		CTexture* pTexture = CManager::GetTexture();

		// �f�o�C�X�̎擾
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));
	}

	// 2D�I�u�W�F�N�g�̕`�揈��
	CObject2D::Draw();
}