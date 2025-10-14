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
	m_nHp = NULL;
	m_nFrame = NULL;
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
	pGuage->SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
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

	m_nHp = m_nMax;

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
	if (m_type == TYPE_GUAGE)
	{
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();		// �L�[�{�[�h�̎擾
		float f, f2, f3, f4;
		f = (float)m_nHp / (float)m_nMax;
#if 0
		f2 = f / 0.1f;
		f3 = f2;
#endif
#if 1
		int n = f / 0.1f;
		f3 = (float)n + 1;
		//f3 = (float)n;
#endif
		f4 = f3 / 10;

#if _DEBUG
		if (pKeyboard->GetTrigger(DIK_1) == true)
		{
			m_nHp = m_nMax;
		}

		if (pKeyboard->GetTrigger(DIK_2) == true)
		{
			m_nHp -= 50;
		}

		if (pKeyboard->GetPress(DIK_3) == true)
		{
			if (m_nHp <= m_nMax)
			{
				m_nHp += 3;
			}
		}
#endif
		if (m_nHp > 0)
		{
			m_nHp--;
		}

		// �c��HP�ɉ����ĐF�ݒ�
		if (m_nHp <= (int)(m_nMax * 0.3f))
		{// �̂���HP30%�ȉ�
			SetCol(D3DCOLOR_RGBA(255, 0, 0, 255));
		}
		else if (m_nHp > (int)(m_nMax * 0.3f))
		{// �̂���HP30%�ȏ�
			SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
		}

		// 2D�I�u�W�F�N�g�̍X�V����
		CObject2D::UpdateUI(f4);
	}
	else if (m_type == TYPE_FRAME)
	{
		// 2D�I�u�W�F�N�g�̍X�V����
		CObject2D::UpdateFrame();
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