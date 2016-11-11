#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "restrict.h"
#include "romaji2kana.h"
/**
* @brief 文字列が文字を持っているか調べます。
* @param io 書き換えるbool型変数へのポインタ、呼び出し後はポインタが指す変数にnew_valueが代入される
* @param new_value 新しい値
* @return ioが指すbool変数がもともと持っていた値
*/
static inline bool exchange_bool(bool* RESTRICT const io, const bool new_value) {
	const bool tmp = *io;
	*io = new_value;
	return tmp;
}
/**
* @brief fgetsで失敗したときにストリームをクリアしてループする関数
* @param s ストリームから読み取った文字列を格納するための領域へのポインタ
* @param buf_size ストリームから読み取った文字列を格納するための領域の大きさ
* @param stream FILE構造体へのポインタかstdin
* @param message_on_error エラー時に表示してループする
* @return 成功時は0, EOFのときはEOF
*/
static inline int fgets_wrap(char* RESTRICT const s, size_t buf_size, FILE* RESTRICT const stream, const char* RESTRICT message_on_error) {
	bool first_flg = true;
	size_t i;
	for (i = 0; i < 100 && NULL == fgets(s, (int)(buf_size), stream); ++i) {
		if (feof(stdin)) return EOF;
		if (!exchange_bool(&first_flg, false)) puts((message_on_error) ? message_on_error : "再入力してください");
		//改行文字が入力を受けた配列にない場合、入力ストリームにごみがある
		size_t j;
		for (j = 0; j < 100 && '\0' == s[j]; j++);//strlenもどき
		if ('\n' != s[j - 1]) while (fgetc(stream) != '\n');//入力ストリームを掃除
	}
	if (100 == i) exit(1);//無限ループ防止
	return 0;
}
int main(void) {
	char buf[512] = { 0 };		//	ローマ字変換結果格納メモリ
	char result[512] = { 0 };		//	ローマ字格納メモリ
#ifdef _DEBUG
	memset(buf, 0xcc, sizeof(buf));
	memset(result, 0xcc, sizeof(result));
#endif
	if (0 == fgets_wrap(buf, sizeof(buf), stdin, NULL)) {
		const char* rest_src = NULL;
		char* rest_result = NULL;
		const char_cvt_error_t e = char_cvt_romaji2kana(buf, strlen(buf), result, sizeof(result), &rest_src, &rest_result);
		printf(
			"入力:%s\n"
			"変換後:%s\n"
			"変換文字数(入力):%d\n"
			"変換文字数(出力):%d\n"
			"エラー:%s(%d)\n",
			buf, result, (int)(rest_src - buf), (int)(rest_result - result), format_char_cvt_error(e), e
		);
	}
	return 0;
}
