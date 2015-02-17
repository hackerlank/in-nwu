/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ mp3.c, 0.1.1005
*/

#include "base.h"
#include "mp3.h"

/* �ļ���ȡ��������С */
#define BUFFER_READ_SIZE		4096
/* д�ļ���������С */
#define BUFFER_WRITE_SIZE		4 * 1024 * 1024
/* ��չ������� */
#define EXTEND_SEPARATE	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"


/* ����֡���ȼ���ϵ�� */
static unsigned int tableRatio[][4] = {
	/* MPEG2.5 */		{0,  72000,  72000,  24000},
	/* undefined */		{0,      0,      0,      0},
	/* MPEG2 */			{0,  72000,  72000,  24000},
	/* MPEG1 */			{0, 144000, 144000,  48000}
};

/* �����ʱ� */
static unsigned int tableBitrates[][16] = {
	/*  0 = 00 00, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/*  1 = 00 01, MPEG2.5 LY3 */	{0,   8,  16,  24,  32,  64,  80,  56,  64, 128, 160, 112, 128, 256, 320, 0},
	/*  2 = 00 10, MPEG2.5 LY2 */	{0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0},
	/*  3 = 00 11, MPEG2.5 LY1 */	{0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0},
	/*  4 = 01 00, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/*  5 = 01 01, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/*  6 = 01 10, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/*  7 = 01 11, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/*  8 = 10 00, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/*  9 = 10 01, MPEG2 LY3 */		{0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0},
	/* 10 = 10 10, MPEG2 LY2 */		{0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0},
	/* 11 = 10 11, MPEG2 LY1 */		{0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0},
	/* 12 = 11 00, undefined */		{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0},
	/* 13 = 11 01, MPEG1 LY3 */		{0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0},
	/* 14 = 11 10, MPEG1 LY2 */		{0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0},
	/* 15 = 11 11, MPEG1 LY1 */		{0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0}
};

/* ������Ϣ�� */
static unsigned int tableFrequency[][4] = {
	/* 00 = MPEG 2.5 */		{11025, 12000,  8000, 0},
	/* 01 = undefined */	{    0,     0,     0, 0},
	/* 10 = MPEG 2 */		{22050, 24000, 16000, 0},
	/* 11 = MPEG 1 */		{44100, 48000, 32000, 0}
};


mp3_s *mp3_info(FILE *fp) {
	mp3_s *mp3;
	id3v2_s id3v2;
	frame_s frame;

	/* �ļ���Ϣ */
	count_t count = 0;
	seek_t position = 0;
	/* ֡��Ϣ */
	var_t value[4];
	seek_t length = 0;

	rewind(fp);

	/* �� id3v2, ���ҵ�, ���� */
	if(fread(&id3v2, sizeof id3v2, 1, fp) == 0) {
		return NULL;
	}
	if(id3v2_check(id3v2)) {
		fseek(fp, id3v2_length(id3v2), SEEK_CUR);
	} else {
		rewind(fp);
	}

	while(!feof(fp)) {
		/* �ҵ��� 1 ֡ */
		if(fread(value, sizeof(unsigned char), 4, fp) == 4) {
			if(frame_check(value)) {
				frame_decode(value, frame);
				frame_length(frame, length);

				if(length > 4) {
					position = (count == 0) ? (ftell(fp) - 4) : position;
					count++;

					/* �ƶ�����һ֡ */
					fseek(fp, length - 4, SEEK_CUR);

					continue;
				}
			}
		}

		/* û���ҵ�����֡ */
		if(count == 0) {
			fseek(fp, -3, SEEK_CUR);
		} else {
			break;
		}
	}

	/* û���ҵ�����֡, ���ܲ��� MP3 �ļ� */
	if(count == 0) {
		return NULL;
	}

	m_new(mp3, mp3_s, 1);

	mp3->count		= count;
	mp3->position	= position;

	return mp3;
}

char *mp3_read(FILE *fp, mp3_s *mp3, seek_t *size, void *arg, int (*callback)(seek_t, char, char *, void *)) {
	frame_s frame;
	id3v1_s id3v1;
	extend_s extend;

	seek_t i, j;
	seek_t length, end, extsize, bufsize = 0;
	var_t value[4];

	char read, code, *buffer = NULL;

	*size = 0;

	/* ����֡�� */
	i = 0;
	j = ((int) (mp3->count / 2)) * 2;

	/* ǰ���� 1 ֡ */
	fseek(fp, mp3->position, SEEK_SET);

	while((i++) < j) {
		frame_next(value, frame, length);

		/* ǰ 2 ֡���������� */
		if(i > 2) {
			/* ���������� 4 λ, ż�������� 4 λ */
			if((i % 2) == 0) {
				frame_unpack_high(value, read, code);

				/* ����У�� */
				if(BP(read) != code) {
					break;
				}

				/* �ص����������ж� */
				if(callback(*size, read, buffer, arg) == 0) {
					break;
				}

				if(bufsize == 0) {
					m_renew(buffer, char, *size + BUFFER_READ_SIZE);

					bufsize = BUFFER_READ_SIZE;
				}

				buffer[*size] = read;

				(*size)++;
				bufsize--;
			} else {
				read = 0;

				frame_unpack_low(value, read, code);

				/* �����ε�У�����Ϊ 1 */
				if(code != 1) {
					break;
				}
			}
		}

		fseek(fp, length, SEEK_CUR);
	}

	/* ������֡��û�ж������� */
	if(buffer == NULL) {
		return NULL;
	}
	/* �쳣�ж� */
	if(i < j) {
		return buffer;
	}

	/* �� id3v1 */
	fseek(fp, -128, SEEK_END);

	/* ��ȡ���жϱ�ǩ�Ƿ�Ϸ� */
	if(id3v1_check(id3v1)) {
		fseek(fp, -135, SEEK_END);
	} else {
		fseek(fp, -7, SEEK_END);
	}
	
	if(fread(&extend, sizeof extend, 1, fp) == 1) {
		/* �жϸ�����Ϣ */
		if(extend_check(extend)) {
			end = ftell(fp) - 7;
			extsize = end - extend.start;

			m_renew(buffer, char, *size + extsize);

			/* ��ȡ��չ�������� */
			fseek(fp, extend.start, SEEK_SET);
			fread(buffer + *size, sizeof(char), extsize, fp);

			*size += extsize;
		}
	}

	return buffer;
}

int mp3_save(FILE *fp, mp3_s *mp3, char *data, seek_t size, void (*callback)(double)) {
	frame_s frame;
	id3v1_s id3v1;
	extend_s extend;

	seek_t i, j, length;
	var_t value[4];

	/* ����ԭ��С */
	double allsize = (double) size;
	/* �����ܵı���֡�� */
	i = 0;
	j = ((int) (mp3->count / 2)) * 2;

	/* ǰ���� 1 ֡ */
	fseek(fp, mp3->position, SEEK_SET);

	while((i++) < j) {
		frame_next(value, frame, length);

		if(i > 2) {
			/* �����α���� 4 λ����ʶ��, ż���α���� 4 λ����У�� */
			if((i % 2) == 0) {
				frame_pack_high(value, *data, BP(*data));

				data++;
				size--;

				/* ���´��������ʾ */
				callback(1 - ((double) size) / allsize);
			} else {
				frame_pack_low(value, *data, 1);
			}

			/* ����֡ͷ */
			fseek(fp, -4, SEEK_CUR);
			fwrite(value, sizeof(char), 4, fp);

			/* �������ݴ������ */
			if(size == 0) {
				callback(1.);

				return 1;
			}
		}

		fseek(fp, length, SEEK_CUR);
	}

	/* ��ʱ��ʣ������, �ƶ������ id3v1 �ı�ǩ */
	fseek(fp, -128, SEEK_END);

	/* �ж�����Ƿ��� id3v1 ��ǩ */
	if(id3v1_check(id3v1)) {
		fseek(fp, -128, SEEK_END);
	} else {
		fseek(fp, 0, SEEK_END);

		id3v1.header[0] = 0;
	}

	/* д�� 16 �ֽڵĸ�����Ϣ, ��ֹ��չ�����ݱ���������������֡���� */
	fwrite(EXTEND_SEPARATE, sizeof(char), 16, fp);
	
	/* ������չ��Ϣ */
	extend.start		= ftell(fp);
	extend.before[0]	= 0x5E;
	extend.before[1]	= 0x39;
	extend.after		= 0x9B;

	/* ��ʣ������д���ļ� */
	while(size > 0) {
		i = BUFFER_WRITE_SIZE;
		i = (size < i) ? size : i;

		fwrite(data, sizeof(char), i, fp);

		size -= i;
		data += i;

		callback(1 - ((double) size) / allsize);
	}

	/* д����չ��Ϣ */
	fwrite(&extend, sizeof extend, 1, fp);

	/* �ָ� id3v1 */
	if(id3v1.header[0]) {
		fwrite(&id3v1, sizeof id3v1, 1, fp);
	}

	callback(1.);

	return 2;
}
