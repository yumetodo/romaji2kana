#ifndef INC_ROMAJI2KANA_H_
#define INC_ROMAJI2KANA_H_
#include "restrict.h"
typedef enum {
	CHAR_CVT_OK = 0,
	CHAR_CVT_INVALID_ARGUMENT = 1,
	CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER = 2,
	CHAR_CVT_NO_CONVERSION = 3,
	CHAR_CVT_ERROR_SIZE
} char_cvt_error_t;
/**
* @brief ローマ字を半角カタカナに変換します
* @param[in] src 変換する文字列
* @param src_len 変換する文字列の長さ
* @param[out] buf 変換結果を格納する領域の先頭へのポインタ
* @param buf_len 変換結果を格納する領域の大きさ
* @param[out] rest_src 未変換部分の先頭へのポインタを格納する変数へのポインタ。optional
* @param[out] rest_buf 変換後の文字列の終端(NULL文字)へのポインタを格納する変数へのポインタ。optional
* @return エラーコード。文字列化するには``format_char_cvt_error``を使う
*/
char_cvt_error_t char_cvt_romaji2kana(
	const char* RESTRICT src,
	const size_t src_len,
	char* RESTRICT buf,
	const size_t buf_len,
	const char** RESTRICT const rest_src,//未変換部分の先頭へのポインタを格納する変数へのポインタ
	char** RESTRICT const rest_buf//変換後の文字列の終端(NULL文字)へのポインタを格納する変数へのポインタ
);
/**
* @brief エラーコードを文字列化します
* @param e エラーコード
* @return エラー文字列
*/
const char* format_char_cvt_error(char_cvt_error_t e);
#endif //INC_ROMAJI2KANA_H_
