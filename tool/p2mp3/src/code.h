/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ code.h, 0.1.1005
*/

#ifndef _CODE_H
#define _CODE_H_

/*
	$ rc4						RC4 �����

	@ char *source				Դ����
	@ const seek_t length		Դ���ݳ���
	@ char *key					��Կ

	# char *					����������
*/
char *rc4(char *source, const seek_t length, char *key);


/*
	$ crc32						CRC32 ����

	@ char *source				����
	@ seek_t length				���ݳ���

	# crc32_t					��ϣֵ
*/
crc32_t crc32(char *source, seek_t length);


/*
	$ md5						MD5 ����

	@ char *source				MD5 �����, source ���ᱻ free() ��

	# char *
*/
char *md5(char *source);


#endif
