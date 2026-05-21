/**
 * @file   BinaryFile.h
 *
 * @brief  バイナリ形式で保存されたファイルの読み込みや管理に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.02.07
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <memory>

// クラスの定義 ===============================================================
/**
 * @brief バイナリーファイル
 */
class BinaryFile
{
// 変数の宣言 --------------------------------------------------------------
protected:

	// データ
	std::unique_ptr<char[]> m_data;

	// サイズ
	unsigned int m_size;

// メンバ関数の宣言 --------------------------------------------------------
// 操作
public:

	// ファイル名を指定してロード
	static std::unique_ptr<BinaryFile> LoadFile(const wchar_t* fileName);

	BinaryFile();

	// ムーブコンストラクタ
	BinaryFile(BinaryFile* in);

// 取得/設定
public:

	// データの取得
	char* GetData() { return m_data.get(); }
	
	// サイズの取得
	unsigned int GetSize() { return m_size; }

};
