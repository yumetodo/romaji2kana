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
* @brief ���[�}���𔼊p�J�^�J�i�ɕϊ����܂�
* @param[in] src �ϊ����镶����
* @param src_len �ϊ����镶����̒���
* @param[out] buf �ϊ����ʂ��i�[����̈�̐擪�ւ̃|�C���^
* @param buf_len �ϊ����ʂ��i�[����̈�̑傫��
* @param[out] rest_src ���ϊ������̐擪�ւ̃|�C���^���i�[����ϐ��ւ̃|�C���^�Boptional
* @param[out] rest_buf �ϊ���̕�����̏I�[(NULL����)�ւ̃|�C���^���i�[����ϐ��ւ̃|�C���^�Boptional
* @return �G���[�R�[�h�B�����񉻂���ɂ�``format_char_cvt_error``���g��
*/
char_cvt_error_t char_cvt_romaji2kana(
	const char* RESTRICT src,
	const size_t src_len,
	char* RESTRICT buf,
	const size_t buf_len,
	const char** RESTRICT const rest_src,//���ϊ������̐擪�ւ̃|�C���^���i�[����ϐ��ւ̃|�C���^
	char** RESTRICT const rest_buf//�ϊ���̕�����̏I�[(NULL����)�ւ̃|�C���^���i�[����ϐ��ւ̃|�C���^
);
/**
* @brief �G���[�R�[�h�𕶎��񉻂��܂�
* @param e �G���[�R�[�h
* @return �G���[������
*/
const char* format_char_cvt_error(char_cvt_error_t e);
#endif //INC_ROMAJI2KANA_H_
