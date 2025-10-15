//=============================================================================
//
// �X�R�A���� [hpvalue.cpp]
// Author : RIKU TANEKAWA �� NAGAO YUSEI
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "hpvalue.h"
#include "number.h"
#include "renderer.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CHPValue::m_nHPValue = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CHPValue::CHPValue(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		m_apNumber[nCnt] = {};
	}
	m_nHPValue = 0;						// �X�R�A
	m_digitWidth = 0.0f;					// ����1��������̕�
	m_digitHeight = 0.0f;					// ����1��������̍���
	m_basePos = INIT_VEC3;				// �\���̊J�n�ʒu
	m_nIdxTexture = 0;						// �e�N�X�`���C���f�b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CHPValue::~CHPValue()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CHPValue* CHPValue::Create(float baseX, float baseY, float digitWidth, float digitHeight)
{
	CHPValue* pHPValue;

	pHPValue = new CHPValue;

	pHPValue->m_basePos = D3DXVECTOR3(baseX, baseY, 0.0f);
	pHPValue->m_digitWidth = digitWidth;
	pHPValue->m_digitHeight = digitHeight;

	// ����������
	pHPValue->Init();

	return pHPValue;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CHPValue::Init(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		float posX = m_basePos.x + nCnt * m_digitWidth;
		float posY = m_basePos.y;

		m_apNumber[nCnt] = CNumber::Create(posX, posY, m_digitWidth, m_digitHeight);

		if (!m_apNumber[nCnt])
		{
			return E_FAIL;
		}
	}

	// �e�N�X�`�����蓖��
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->RegisterDynamic("data/TEXTURE/num001.png");

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CHPValue::Uninit(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt] != nullptr)
		{
			// �i���o�[�̏I������
			m_apNumber[nCnt]->Uninit();

			delete m_apNumber[nCnt];
			m_apNumber[nCnt] = nullptr;
		}
	}

	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CHPValue::Update(void)
{
	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt])
		{
			// �i���o�[�̍X�V����
			m_apNumber[nCnt]->Update();
		}
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CHPValue::Draw(void)
{
	int HPValue = m_nHPValue;
	bool bDraw = false;

	// �X�R�A���e���ɕ����i�E����j

	for (int nCnt = 0; nCnt < MAX_DIGITS; nCnt++)
	{
		int digit = m_nDig[nCnt];

		if (digit != 0 || nCnt == MAX_DIGITS - 1 || bDraw)
		{
			bDraw = true;

			if (m_apNumber[nCnt])
			{
				// ���ݒ菈��
				m_apNumber[nCnt]->SetDigit(digit);

				// �e�N�X�`���̎擾
				CTexture* pTexture = CManager::GetTexture();

				// �f�o�C�X�̎擾
				CRenderer* renderer = CManager::GetRenderer();
				LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

				// �e�N�X�`���̐ݒ�
				pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

				m_apNumber[nCnt]->Draw();
			}
		}
	}

	//for (int nCnt = MAX_DIGITS - 1; nCnt >= 0; nCnt--)
	//{
	//	int digit = HPValue % 10;
	//	HPValue /= 10;

	//	if (m_apNumber[nCnt])
	//	{
	//		// ���ݒ菈��
	//		m_apNumber[nCnt]->SetDigit(digit);

	//		// �e�N�X�`���̎擾
	//		CTexture* pTexture = CManager::GetTexture();

	//		// �f�o�C�X�̎擾
	//		CRenderer* renderer = CManager::GetRenderer();
	//		LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

	//		// �e�N�X�`���̐ݒ�
	//		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	//		m_apNumber[nCnt]->Draw();
	//	}
	//}
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CHPValue::GetPos(void)
{
	return D3DXVECTOR3();
}
//=============================================================================
// �X�R�A�ݒ菈��
//=============================================================================
void CHPValue::SetHPValue(int nHPValue)
{
	m_nHPValue = nHPValue;
}
//=============================================================================
// �X�R�A���Z����
//=============================================================================
void CHPValue::AddHPValue(int nValue)
{
	m_nHPValue += nValue;

	if (m_nHPValue > 999)
	{
		// 3������
		m_nHPValue = 999;
	}
}
//=============================================================================
// ����������
//=============================================================================
int CHPValue::DigitNum(int nHPValue)
{
	if (nHPValue == 0)
	{
		return 1;
	}

	int nCnt = 0;
	while (nHPValue > 0)
	{
		nHPValue /= 10;
		nCnt++;
	}

	return nCnt;
}