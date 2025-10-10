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

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGuage::CGuage()
{
	// �l�̃N���A
	m_pVtxBuff = nullptr;	// ���_�o�b�t�@
	m_pos = INIT_VEC3;		// �ʒu
	m_type = TYPE_NONE;		// �Q�[�W�̎��
	m_Col = INIT_XCOL;		// �Q�[�W�J���[
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
	pGuage->Init(pos, type);

	return pGuage;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CGuage::Init(D3DXVECTOR3 pos, GUAGETYPE type)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// ������
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_type = type;

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	if (type == TYPE_FRAME)
	{
		m_nFrame = m_nMax;
		m_Col = D3DCOLOR_RGBA(0, 0, 0, 255);

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DCOLOR_RGBA(0, 0, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(0, 0, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(0, 0, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(0, 0, 0, 255);
	}
	else if (type == TYPE_GUAGE)
	{
		m_nHp = m_nMax;
		m_Col = D3DCOLOR_RGBA(0, 255, 0, 255);

		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(0, 255, 0, 255);
	}

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CGuage::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CGuage::Update(void)
{
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();		// �L�[�{�[�h�̎擾
	VERTEX_2D* pVtx;

	float f = (float)m_nHp / (float)m_nMax;
	//float f2 = f / 0.1f;
	//float f3 = f2 + 1;
	int n = f / 0.1f;
	float f3 = (float)n + 1;
	float f4 = f3 / 10;

	if (pKeyboard->GetTrigger(DIK_1) == true)
	{
		m_nHp = m_nMax;
	}

	if (pKeyboard->GetTrigger(DIK_2) == true)
	{
		m_nHp -= 50;
	}

	if (pKeyboard->GetRepeat(DIK_3) == true)
	{
		if (m_nHp <= m_nMax)
		{
			m_nHp += 2;
		}
	}
	else
	{
		m_nHp--;
	}


	//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth + (f4 * 500.0f), m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_fWidth, m_fHeight + 13.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth + (f4 * 500.0f), m_fHeight + 13.0f, 0.0f);

	if (m_nHp <= 300)
	{
		//���_�J���[�̐ݒ�
		pVtx[0].col = D3DCOLOR_RGBA(255, 0, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 0, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 0, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 0, 0, 255);
	}
	else if (m_nHp > 300)
	{
		// ���_�J���[�̐ݒ�
		pVtx[0].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(0, 255, 0, 255);
	}

	//���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGuage::Draw(void)
{
	// 2D�I�u�W�F�N�g�̕`�揈��
	CObject2D::Draw();
}