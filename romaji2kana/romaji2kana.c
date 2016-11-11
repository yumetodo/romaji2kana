#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <limits.h>
#include <stdint.h>
#ifndef __cplusplus
#	include <assert.h>
#	if !defined(static_assert) && !(defined(_MSC_VER) && 1600 <= _MSC_VER && !defined(__c2__))
#		define static_assert(cond, message)
#	endif
#	include <stdbool.h>
#endif
#include "romaji2kana.h"

#if !defined(__cplusplus) && !defined(nullptr)
#	define nullptr NULL
#endif

#ifndef countof
#	ifdef _MSC_VER
#		define countof(arr) _countof(arr)
#	elif defined(__cplusplus) && 201500 < __cplusplus
#		include <iterator>
#		define countof(arr) std::size(arr)
#	else
#		define countof(arr) (sizeof(arr) / sizeof(*arr))
#	endif
#endif


static inline bool char_cvt_check_range(const char* const begin, const char* const end, const size_t expect_len) {
	return (nullptr != begin) && (nullptr != end) && (begin < end) && (expect_len <= ((size_t)(end - begin)));
}
static inline char_cvt_error_t char_cvt_process_impl_write_str(
	const char* const write_str, const char** const read_ptr, char** const write_ptr, const char* const write_end, size_t read_len
) {
	if (nullptr == write_str)  return CHAR_CVT_NO_CONVERSION;
	const size_t write_len = strlen(write_str);//うまくやればコンパイル時定数にできそうだけど、メモリー律速になりかねないのでやらない
	if (!char_cvt_check_range(write_ptr[0], write_end, write_len)) return CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER;
	memcpy(write_ptr[0], write_str, write_len);//NULL文字はコピーしない
	//iterate pointer
	read_ptr[0] += read_len;
	write_ptr[0] += write_len;
	return CHAR_CVT_OK;
}

static const char char_cvt_vowels_table[] = "aiueo";//NULL文字を含む
static const char* const char_cvt_table_00[] = {
	"ｱ", 	"ｲ", 		"ｳ", 		"ｴ", 		"ｵ"
};
static_assert(countof(char_cvt_vowels_table) == countof(char_cvt_table_00) + 1, "error");
static inline size_t char_cvt_is_vowels(const char c) {//ほぼmemchrだけどindexでほしい&lowerに変換してから見たい
	for (size_t i = 0; i < countof(char_cvt_vowels_table) - 1; ++i) if (tolower(c) == char_cvt_vowels_table[i]) return i;//NULL文字除いて比較
	return SIZE_MAX;
}
static inline char_cvt_error_t char_cvt_process_vowels(
	const char** const read_ptr, const char* const read_end, char** const write_ptr, const char* const write_end
) {
	if (
		nullptr == read_ptr || nullptr == read_ptr[0] || nullptr == read_end || nullptr == write_ptr
		|| read_end <= read_ptr[0]
	) {
		return CHAR_CVT_INVALID_ARGUMENT;
	}
	const size_t table_pos = char_cvt_is_vowels(read_ptr[0][0]);
	if (SIZE_MAX == table_pos) return CHAR_CVT_NO_CONVERSION;
	return char_cvt_process_impl_write_str(char_cvt_table_00[table_pos], read_ptr, write_ptr, write_end, 1);
}
static const char char_cvt_consonants_table_1[] = "ywv";
static const char* const char_cvt_table_01[][countof(char_cvt_vowels_table)] = {
	//		 a		 i			 u		 	 e			 o
	{ "ﾔ", 	nullptr, 	"ﾕ", 		nullptr, 	"ﾖ" },	//y
	{ "ﾜ", 	"ｳｨ", 		nullptr, 	"ｳｪ", 		"ｦ" },	//w
	{ "ｳﾞｧ", 	"ｳﾞｨ", 		"ｳﾞ", 		"ｳﾞｪ", 		"ｳﾞｫ" }	//v
};
static_assert(countof(char_cvt_consonants_table_1) == countof(char_cvt_table_01) + 1, "error");
static inline size_t char_cvt_is_consonants_next_1(const char c) {
	for (size_t i = 0; i < countof(char_cvt_consonants_table_1); ++i) if (tolower(c) == char_cvt_consonants_table_1[i]) return i;
	return SIZE_MAX;
}
static inline char_cvt_error_t char_cvt_process_consonants_next_1(
	const char** const read_ptr, const char* const read_end, char** const write_ptr, const char* const write_end
) {
	if (
		nullptr == read_ptr || nullptr == read_ptr[0] || nullptr == read_end || nullptr == write_ptr
		|| read_end <= read_ptr[0] + 1
	) {
		return CHAR_CVT_INVALID_ARGUMENT;
	}
	const size_t table_pos[] = { char_cvt_is_consonants_next_1(read_ptr[0][0]), char_cvt_is_vowels(read_ptr[0][1]) };
	if (SIZE_MAX == table_pos[0] || SIZE_MAX == table_pos[1]) return CHAR_CVT_NO_CONVERSION;
	return char_cvt_process_impl_write_str(char_cvt_table_01[table_pos[0]][table_pos[1]], read_ptr, write_ptr, write_end, 1);
}

static const char char_cvt_consonants_table_1y[] = "knmrgzjbpfx";
static const char* const char_cvt_table_1y[][countof(char_cvt_vowels_table)] = {
	//		 a		 i		 u		 e		 o
	{ "ｶ", 	"ｷ", 	"ｸ", 	"ｹ", 	"ｺ" },	//k
	{ "ﾅ", 	"ﾆ", 	"ﾇ", 	"ﾈ", 	"ﾉ" },	//n
	{ "ﾏ", 	"ﾐ", 	"ﾑ", 	"ﾒ", 	"ﾓ" },	//m
	{ "ﾗ", 	"ﾘ", 	"ﾙ", 	"ﾚ", 	"ﾛ" },	//r

	{ "ｶﾞ", 	"ｷﾞ", 	"ｸﾞ", 	"ｹﾞ", 	"ｺﾞ" },	//g
	{ "ｻﾞ", 	"ｼﾞ", 	"ｽﾞ", 	"ｾﾞ", 	"ｿﾞ" },	//z
	{ "ｼﾞｬ", 	"ｼﾞ", 	"ｼﾞｭ", 	"ｼﾞｪ", 	"ｼﾞｮ" },	//j
	{ "ﾊﾞ", 	"ﾋﾞ", 	"ﾌﾞ", 	"ﾍﾞ", 	"ﾎﾞ" },	//b

	{ "ﾊﾟ", 	"ﾋﾟ", 	"ﾌﾟ", 	"ﾍﾟ", 	"ﾎﾟ" },	//p

	{ "ファ", 	"フィ", 	"ﾌ", 	"ﾌｪ", 	"ﾌｫ" },	//f
	{ "ｧ", 	"ｨ", 	"ｩ", 	"ｪ", 	"ｫ" }	//x
};
static_assert(countof(char_cvt_consonants_table_1y) == countof(char_cvt_table_1y) + 1, "error");
static const char* const char_cvt_table_1y_y[][countof(char_cvt_vowels_table)] = {
	//		 a		 i			 u		 e			 o
	{ "ｷｬ", 	nullptr, 	"ｷｭ", 	nullptr, 	"ｺ" },	//k
	{ "ﾆｬ", 	nullptr, 	"ﾆｭ", 	"ﾆｪ", 		"ﾆｮ" },	//n
	{ "ﾐｬ", 	nullptr, 	"ﾐｭ", 	nullptr, 	"ﾐｮ" },	//m
	{ "ﾘｬ", 	nullptr, 	"ﾘｭ", 	nullptr, 	"ﾘｮ" },	//r

	{ "ｷﾞｬ", 	nullptr, 	"ｷﾞｭ", 	nullptr, 	"ｷﾞｮ" },	//g
	{ "ｼﾞｬ", 	nullptr, 	"ｼﾞｭ", 	"ジェ", 		"ｼﾞｮ" },	//z
	{ "ｼﾞｬ", 	nullptr, 	"ｼﾞｭ", 	"ｼﾞｪ", 		"ｼﾞｮ" },	//j
	{ "ﾋﾞｬ", 	nullptr, 	"ﾋﾞｭ", 	nullptr, 	"ﾋﾞｮ" },	//b

	{ "ﾋﾟｬ", 	nullptr, 	"ﾋﾟｭ", 	nullptr, 	"ﾋﾟｮ" },	//p

	{ "ﾌｬ", 	nullptr, 	"ﾌｭ", 	nullptr, 	"ﾌｮ" },	//f
	{ "ｬ", 	nullptr, 	"ｭ", 	nullptr, 	"ｮ" }	//x
};
static_assert(countof(char_cvt_consonants_table_1y) == countof(char_cvt_table_1y_y) + 1, "error");
static inline size_t char_cvt_is_consonants_next_1y(const char c) {
	for (size_t i = 0; i < countof(char_cvt_consonants_table_1y); ++i) if (tolower(c) == char_cvt_consonants_table_1y[i]) return i;
	return SIZE_MAX;
}
static inline char_cvt_error_t char_cvt_process_consonants_next_1y(
	const char** const read_ptr, const char* const read_end, char** const write_ptr, const char* const write_end
) {
	if (
		nullptr == read_ptr || nullptr == read_ptr[0] || nullptr == read_end || nullptr == write_ptr
		|| read_end <= read_ptr[0] + 1 || (('y' == tolower(read_ptr[0][1])) && (read_end <= read_ptr[0] + 2))
	) {
		return CHAR_CVT_INVALID_ARGUMENT;
	}
	const bool has_y = ('y' == tolower(read_ptr[0][1]));
	const size_t table_pos[] = { char_cvt_is_consonants_next_1y(read_ptr[0][0]), char_cvt_is_vowels(read_ptr[0][((has_y) ? 2 : 1)]) };
	if (SIZE_MAX == table_pos[0] || SIZE_MAX == table_pos[1]) return CHAR_CVT_NO_CONVERSION;
	const char* const write_str = (has_y) ? char_cvt_table_1y_y[table_pos[0]][table_pos[1]] : char_cvt_table_1y[table_pos[0]][table_pos[1]];
	return char_cvt_process_impl_write_str(write_str, read_ptr, write_ptr, write_end, (has_y) ? 3 : 2);
}

static const char char_cvt_consonants_table_1yh[] = "scthd";
static const char* const char_cvt_table_1yh[][countof(char_cvt_vowels_table)] = {
	//		 a		 i		 u		 e		 o
	{ "ｻ", 	"ｼ", 	"ｽ", 	"ｾ", 	"ｿ" },	//s
	{ "ｶ", 	"ｼ", 	"ｸ", 	"ｾ", 	"ｺ" },	//c
	{ "ﾀ", 	"ﾁ", 	"ﾂ", 	"ﾃ", 	"ﾄ" },	//t
	{ "ﾊ", 	"ﾋ", 	"ﾌ", 	"ﾍ", 	"ﾎ" },	//h

	{ "ﾀﾞ", 	"ﾁﾞ", 	"ﾂﾞ", 	"ﾃﾞ", 	"ﾄﾞ" },	//d
};
static_assert(countof(char_cvt_consonants_table_1yh) == countof(char_cvt_table_1yh) + 1, "error");
static const char* const char_cvt_table_1yh_y[][countof(char_cvt_vowels_table)] = {
	//		 a		 i			 u		 e			 o
	{ "ｼｬ", 	nullptr, 	"ｼｭ", 	"ｼｪ", 		"ｼｮ" },	//s
	{ "ﾁｬ", 	nullptr, 	"ﾁｭ", 	"ﾁｪ", 		"ﾁｮ" },	//c
	{ "ﾁｬ", 	nullptr, 	"ﾁｭ", 	"ﾁｪ", 		"ﾁｮ" },	//t
	{ "ﾋｬ", 	nullptr, 	"ﾋｭ", 	"ﾋｪ", 		"ﾋｮ" },	//h

	{ "ﾁﾞｬ", 	nullptr, 	"ﾁﾞｭ", 	nullptr, 	"ﾁﾞｮ" },	//d
};
static_assert(countof(char_cvt_consonants_table_1yh) == countof(char_cvt_table_1yh_y) + 1, "error");
static const char* const char_cvt_table_1yh_h[][countof(char_cvt_vowels_table)] = {
	//		 a		 i			 u		 e			 o
	{ "ｼｬ", 	nullptr, 	"ｼｭ", 	"ｼｪ", 		"ｼｮ" },	//s
	{ "ﾁｬ", 	"ﾁ", 		"ﾁｭ", 	"ﾁｪ", 		"ﾁｮ" },	//c
	{ "ﾁｬ", 	nullptr, 	"ﾁｭ", 	"ﾁｪ", 		"ﾁｮ" },	//t
	{ "ﾋｬ", 	nullptr, 	"ﾋｭ", 	"ﾋｪ", 		"ﾋｮ" },	//h

	{ "ﾁﾞｬ", 	nullptr, 	"ﾁﾞｭ", 	nullptr, 	"ﾁﾞｮ" },	//d
};
static_assert(countof(char_cvt_consonants_table_1yh) == countof(char_cvt_table_1yh_h) + 1, "error");
static inline size_t char_cvt_is_consonants_next_1yh(const char c) {
	for (size_t i = 0; i < countof(char_cvt_consonants_table_1yh); ++i) if (tolower(c) == char_cvt_consonants_table_1yh[i]) return i;
	return SIZE_MAX;
}
static inline char_cvt_error_t char_cvt_process_consonants_next_1yh(
	const char** const read_ptr, const char* const read_end, char** const write_ptr, const char* const write_end
) {
	if (
		nullptr == read_ptr || nullptr == read_ptr[0] || nullptr == read_end || nullptr == write_ptr
		|| read_end <= read_ptr[0] + 1
		|| ((('y' == tolower(read_ptr[0][1])) || ('h' == tolower(read_ptr[0][1]))) && (read_end <= read_ptr[0] + 2))
	) {
		return CHAR_CVT_INVALID_ARGUMENT;
	}
	const bool has_y = ('y' == tolower(read_ptr[0][1]));
	const bool has_h = ('h' == tolower(read_ptr[0][1]));
	const size_t table_pos[] = { char_cvt_is_consonants_next_1yh(read_ptr[0][0]), char_cvt_is_vowels(read_ptr[0][((has_y || has_h) ? 2 : 1)]) };
	if (SIZE_MAX == table_pos[0] || SIZE_MAX == table_pos[1]) return CHAR_CVT_NO_CONVERSION;
	const char* const write_str = (has_h) ? char_cvt_table_1yh_h[table_pos[0]][table_pos[1]]
		: (has_y) ? char_cvt_table_1yh_y[table_pos[0]][table_pos[1]]
		: char_cvt_table_1yh[table_pos[0]][table_pos[1]];
	return char_cvt_process_impl_write_str(write_str, read_ptr, write_ptr, write_end, (has_y || has_h) ? 3 : 2);
}

static inline char_cvt_error_t char_cvt_process_special(
	const char** const read_ptr, const char* const read_end, char** const write_ptr, const char* const write_end
) {
	if (
		nullptr == read_ptr || nullptr == read_ptr[0] || nullptr == read_end || nullptr == write_ptr
		|| read_end <= read_ptr[0]
	) {
		return CHAR_CVT_INVALID_ARGUMENT;
	}
	size_t read_len = 1;
	const char* write_str = nullptr;
	switch (tolower(read_ptr[0][0])) {
		case 'n':
			if (read_ptr[0] + 1 < read_end && 'n' == read_ptr[0][1]) ++read_len;//nnも処理
			write_str = "ﾝ";
			break;
		default:
			if (read_ptr[0] + 1 < read_end && read_ptr[0][0] == read_ptr[0][1]) {//促音
				++read_len;
				write_str = "ｯ";
			}
			break;
	}
	return char_cvt_process_impl_write_str(write_str, read_ptr, write_ptr, write_end, read_len);
}
static inline char_cvt_error_t char_cvt_set_rest(
	const char** RESTRICT const rest_src, const char* RESTRICT const read_ptr,
	char** RESTRICT const rest_buf, char* RESTRICT const write_ptr,
	const char_cvt_error_t e
) {
	write_ptr[0] = '\0';//文字列を終端させる
	if (rest_src) rest_src[0] = read_ptr;
	if (rest_buf) rest_buf[0] = write_ptr;
	return e;
}
char_cvt_error_t char_cvt_romaji2kana(
	const char* RESTRICT src,	//	変換する文字列
	const size_t src_len,//変換する文字列の長さ
	char* RESTRICT buf,	//	変換結果を格納するメモリ
	const size_t buf_len,
	const char** RESTRICT const rest_src,//未変換部分の先頭へのポインタを格納する変数へのポインタ
	char** RESTRICT const rest_buf//変換後の文字列の終端(NULL文字)へのポインタを格納する変数へのポインタ
) {
	if (nullptr == src || nullptr == buf || 0 == src_len || 0 == buf_len) return CHAR_CVT_INVALID_ARGUMENT;
	const char* read_ptr = src;
	const char* const read_end = src + src_len;
	char* write_ptr = buf;
	const char* const write_end = buf + buf_len - 1;//NULL文字分を確保
#ifdef _DEBUG
	buf[buf_len - 1] = '\0';
#endif
	char_cvt_error_t e = CHAR_CVT_OK;
	while (read_ptr < read_end && write_ptr < write_end && CHAR_CVT_OK == e) {

		e = char_cvt_process_vowels(&read_ptr, read_end, &write_ptr, write_end);
		if (CHAR_CVT_INVALID_ARGUMENT == e || CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER == e) break;
		else if (CHAR_CVT_OK == e) continue;

		e = char_cvt_process_consonants_next_1(&read_ptr, read_end, &write_ptr, write_end);
		if (CHAR_CVT_INVALID_ARGUMENT == e || CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER == e) break;
		else if (CHAR_CVT_OK == e) continue;

		e = char_cvt_process_consonants_next_1y(&read_ptr, read_end, &write_ptr, write_end);
		if (CHAR_CVT_INVALID_ARGUMENT == e || CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER == e) break;
		else if (CHAR_CVT_OK == e) continue;

		e = char_cvt_process_consonants_next_1yh(&read_ptr, read_end, &write_ptr, write_end);
		if (CHAR_CVT_INVALID_ARGUMENT == e || CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER == e) break;
		else if (CHAR_CVT_OK == e) continue;

		e = char_cvt_process_special(&read_ptr, read_end, &write_ptr, write_end);
		if (CHAR_CVT_INVALID_ARGUMENT == e || CHAR_CVT_NOT_ENOUGH_WRITE_BUFFER == e) break;
	}
	if (
		CHAR_CVT_INVALID_ARGUMENT == e
		&& read_ptr < read_end && (1 == (read_end - read_ptr) || (2 == (read_end - read_ptr) && nullptr != strchr("\r\n", read_ptr[1])))
		&& nullptr != strchr("\r\n", read_ptr[0])
	) {
		read_ptr = read_end;
		e = CHAR_CVT_OK;
	}
	return char_cvt_set_rest(rest_src, read_ptr, rest_buf, write_ptr, e);
}
const char* format_char_cvt_error(char_cvt_error_t e) {
	static const char* table[] = {
		"OK", "invalid argument", "not enough write buffer", "no conversion"
	};
	static_assert(countof(table) == CHAR_CVT_ERROR_SIZE, "error");
	return table[e];
}
