/**
 * @file   BinaryFile.cpp
 *
 * @brief  バイナリ形式で保存されたファイルの読み込みや管理に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.02.07
 */


 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "BinaryFile.h"

#include <fstream>
#include <assert.h>

// メンバ関数の定義 ===========================================================
/**
 * @brief ファイルの読み込み処理
 *
 * @param[in] fileName ファイル名
 *
 * @return なし
 */
std::unique_ptr<BinaryFile> BinaryFile::LoadFile(const wchar_t * fileName)
{
	std::ifstream ifs;

	//	ファイルオープン
	ifs.open(fileName, std::ios::in | std::ios::binary);

	//	読み込み失敗時、強制終了
	assert(ifs);

	//	実際にファイル内容を読み取る
	BinaryFile* bin = new BinaryFile();

	//	ファイルサイズを取得
	ifs.seekg(0, std::fstream::end);
	std::streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::fstream::beg);
	std::streamoff begPos = ifs.tellg();
	bin->m_size = (unsigned int)(eofPos - begPos);

	//	読み込むためのメモリを確保
	bin->m_data.reset(new char[bin->m_size]);

	//	ファイル先頭からバッファへコピー 
	ifs.read(bin->m_data.get(), bin->m_size);

	//	ファイルクローズ
	ifs.close();

	return std::make_unique<BinaryFile>(bin);
}



/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
BinaryFile::BinaryFile()
{
	m_size = 0;
}



/**
 * @brief デストラクタ
 */
BinaryFile::BinaryFile(BinaryFile* in)
{
	m_data = std::move(in->m_data);
	m_size = in->m_size;
}
