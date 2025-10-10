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

//=============================================================================
// コンストラクタ
//=============================================================================
CGuage::CGuage()
{
	// 値のクリア
	m_pVtxBuff = nullptr;	// 頂点バッファ
	m_pos = INIT_VEC3;		// 位置
	m_type = TYPE_NONE;		// ゲージの種類
	m_Col = INIT_XCOL;		// ゲージカラー
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
	pGuage->Init(pos, type);

	return pGuage;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGuage::Init(D3DXVECTOR3 pos, GUAGETYPE type)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_type = type;

	VERTEX_2D* pVtx;// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	if (type == TYPE_FRAME)
	{
		m_nFrame = m_nMax;
		m_Col = D3DCOLOR_RGBA(0, 0, 0, 255);

		// 頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(0, 0, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(0, 0, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(0, 0, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(0, 0, 0, 255);
	}
	else if (type == TYPE_GUAGE)
	{
		m_nHp = m_nMax;
		m_Col = D3DCOLOR_RGBA(0, 255, 0, 255);

		// 頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(0, 255, 0, 255);
	}

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGuage::Uninit(void)
{
	// 頂点バッファの破棄
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CGuage::Update(void)
{
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();		// キーボードの取得
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


	//頂点バッファをロックし、頂点情報へのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(m_fWidth, m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_fWidth + (f4 * 500.0f), m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_fWidth, m_fHeight + 13.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_fWidth + (f4 * 500.0f), m_fHeight + 13.0f, 0.0f);

	if (m_nHp <= 300)
	{
		//頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(255, 0, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 0, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 0, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 0, 0, 255);
	}
	else if (m_nHp > 300)
	{
		// 頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[1].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[2].col = D3DCOLOR_RGBA(0, 255, 0, 255);
		pVtx[3].col = D3DCOLOR_RGBA(0, 255, 0, 255);
	}

	//頂点バッファをアンロックする
	m_pVtxBuff->Unlock();
}
//=============================================================================
// 描画処理
//=============================================================================
void CGuage::Draw(void)
{
	// 2Dオブジェクトの描画処理
	CObject2D::Draw();
}