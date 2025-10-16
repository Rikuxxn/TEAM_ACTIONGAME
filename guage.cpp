//=============================================================================
//
// ゲージ処理 [guage.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "guage.h"
#include "manager.h"
#include "score.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
int CGuage::m_nIdxTexture = 0;

//=============================================================================
// コンストラクタ
//=============================================================================
CGuage::CGuage()
{
	// 値のクリア
	m_nIdxTexture = 0;
	m_type = TYPE_NONE;	// ゲージの種類
	m_nHp = NULL;
	m_nFrame = NULL;
}
//=============================================================================
// デストラクタ
//=============================================================================
CGuage::~CGuage()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CGuage* CGuage::Create(D3DXVECTOR3 pos, GUAGETYPE type)
{
	CGuage* pGuage = nullptr;

	pGuage = new CGuage;

	// 初期化処理
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
// 初期化処理
//=============================================================================
HRESULT CGuage::Init(void)
{
	if (m_type == TYPE_FRAME)
	{
		// テクスチャ設定
		m_nIdxTexture = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/BatteryFrame.png");
	}

	m_nHp = m_nMax;

	// 2Dオブジェクトの初期化
	CObject2D::Init();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGuage::Uninit(void)
{
	// 2Dオブジェクトの終了処理
	CObject2D::Uninit();
}
//=============================================================================
// 更新処理
//=============================================================================
void CGuage::Update(void)
{
	static int nCount = 0;
	if (m_type == TYPE_GUAGE)
	{
		nCount++;
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得
		float f, f3, f4;
		f = (float)m_nHp / (float)m_nMax;
#if 0
		f2 = f / 0.1f;
		f3 = f2;
#endif
#if 1
		int n = f / 0.1f;
		if (m_nHp >= m_nMax ||
			(int)(((float)m_nHp / (float)m_nMax) * 100) <= 0)
		{
			f3 = (float)n;
		}
		else
		{
			f3 = (float)n + 1;
		}
#endif
		if (m_nHp >= 1)
		{
			f4 = f3 / 10;
		}
		else
		{
			f4 = 0.0f;
		}

#if _DEBUG
		if (pKeyboard->GetTrigger(DIK_1) == true)
		{
			m_nHp = m_nMax;
		}

		if (pKeyboard->GetTrigger(DIK_2) == true)
		{
			m_nHp -= (int)((float)m_nMax * 0.1f);
		}

		if (pKeyboard->GetTrigger(DIK_SPACE) == true)
		{
			m_nHp -= (int)((float)m_nMax * 0.03f);
		}

		if (pKeyboard->GetPress(DIK_3) == true)
		{
			if (m_nHp < m_nMax)
			{
				m_nHp += (int)((float)m_nMax * 0.003f);
			}
			else if (m_nHp >= m_nMax)
			{
				m_nHp = m_nMax;
			}
		}
#endif
		else
		{
			if (m_nHp > 0)
			{
				m_nHp--;
			}
		}

		// のこりHP30%以上
		if (m_nHp >= (int)(m_nMax * 0.3f))
		{
			SetCol(D3DCOLOR_RGBA(0, 255, 0, 255));
		}
		// それ以外
		else
		{
			SetCol(D3DCOLOR_RGBA(255, 0, 0, 255));
		}

		CScore::SetScore((int)(((float)m_nHp / (float)m_nMax) * 100));

		//// のこりHP20%以上
		//if (m_nHp >= (int)(m_nMax * 0.2f))
		//{
		//	float Col = 200.0f * ((float)m_nHp / (float)m_nMax);
		//	SetCol(D3DCOLOR_RGBA(255 - (int)Col, (int)Col + 55, 0, 255));
		//}
		//// それ以外
		//else
		//{
		//	if (nCount % 2 == 0)
		//	{
		//		SetCol(D3DCOLOR_RGBA(255, 0, 0, 255));
		//	}
		//	else
		//	{
		//		SetCol(D3DCOLOR_RGBA(255, 255, 255, 255));
		//	}
		//}

		// 2Dオブジェクトの更新処理
		CObject2D::UpdateUI(f4, 52.0f);
	}
	else if (m_type == TYPE_FRAME)
	{
		// 2Dオブジェクトの更新処理
		CObject2D::UpdateFrame(52.0f);
	}
}
//=============================================================================
// 描画処理
//=============================================================================
void CGuage::Draw(void)
{
	if (m_type == TYPE_FRAME)
	{
		// テクスチャの取得
		CTexture* pTexture = CManager::GetTexture();

		// デバイスの取得
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		// テクスチャの設定
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));
	}

	// 2Dオブジェクトの描画処理
	CObject2D::Draw();
}