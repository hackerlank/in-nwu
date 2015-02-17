/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ mp3.h, 0.1.1005
*/

#ifndef _MP3_H_
#define _MP3_H_

/* ��ȡ v �ĵ� p λ��ʼ�����ĵ� N λ */
#define BR1(v, p) (((v) >> (p)) & 0x01)
#define BR2(v, p) (((v) >> (p)) & 0x03)
#define BR3(v, p) (((v) >> (p)) & 0x07)
#define BR4(v, p) (((v) >> (p)) & 0x0F)
/* ���� v �ĵ� p λΪ s */
#define BS(v, p, s) v = v & ~(0x01 << (p)) | ((s) << (p))
/* У�� v */
#define BP(v) ( \
		(((v) & 0x01) >> 0) ^ (((v) & 0x02) >> 1) ^ \
		(((v) & 0x04) >> 2) ^ (((v) & 0x08) >> 3) ^ \
		(((v) & 0x10) >> 4) ^ (((v) & 0x20) >> 5) ^ \
		(((v) & 0x40) >> 6) ^ (((v) & 0x80) >> 7) \
	)


/* �ṹ������������ */
#pragma pack(1)


/* MP3 �ļ���Ϣ */
typedef struct {
	count_t count;				/* �ļ��п�ʶ���������֡��Ŀ */
	seek_t position;			/* �� 1 ֡����֡��λ�� */
} mp3_s;

/* id3v1 */
typedef struct {
	var_t header[3];			/* �̶�ֵ "TAG" */
	var_t data[125];			/* ʣ��� 125 �ֽ� */
} id3v1_s;

/* id3v2 ͷ */
typedef struct {
	var_t header[3];			/* �̶�ֵ "ID3" */
	var_t version;				/* ���汾 */
	var_t revision;				/* �Ӱ汾 */
	var_t flag;					/* ��ʶ�� */
	var_t size[4];				/* ��ǩ���� */
} id3v2_s;

/* ����֡֡ͷ */
typedef struct {
	var_t version;				/* [v1, 3-4] MPEG �汾 */
	var_t layer;				/* [v1, 5-6] MPEG Layer */
	var_t crc;					/* [v1, 7]   CRC У�� */
	var_t bitrate;				/* [v2, 0-3] ������ */
	var_t frequency;			/* [v2, 4-5] ����Ƶ�� */
	var_t padding;				/* [v2, 6]   ֡������ */
} frame_s;

/* �ϲ���Ϣͷ */
typedef struct {
	var_t id;					/* �̶�ֵ 0xA9 */
	seek_t size;				/* �������ݵĳߴ� */
	crc32_t crcraw;				/* ԭʼ���� CRC32 */
	crc32_t crcdat;				/* �������� CRC32 */
	char ext[5];				/* �ļ���չ�� */
} header_s;

/* ��չ��Ϣ�� */
typedef struct {
	var_t before[2];			/* �̶�ֵ 0x395E */
	seek_t start;				/* ��չ���ݿ�ʼλ�� */
	var_t after;				/* �̶�ֵ 0x9B */
} extend_s;


/* �ָ����� */
#pragma pack()


/* �ж��ǲ��� ID3v1 ��ǩ */
#define id3v1_check(v) ((fread(&v, sizeof v, 1, fp) == 1) && (ms_compare(v.header, "TAG") == 0))
/* �ж��ǲ��� ID3v2 ��ǩ */
#define id3v2_check(v) (ms_compare(v.header, "ID3") == 0)
/* ���� ID3v2 ���б�ǩ֡���� */
#define id3v2_length(v) ((((int) (v.size[0] & 0x7F)) << 21) | (((int) (v.size[1] & 0x7F)) << 14) | (((int) (v.size[2] & 0x7F)) << 7) | ((int) (v.size[3] & 0x7F)))

/* �ж��ǲ�������֡ */
#define frame_check(v) ((v[0] == 0xFF) && ((v[1] & 0xE0) == 0xE0))
/* ����֡���� */
#define frame_decode(v, f) \
	f.version	= BR2(v[1], 3); \
	f.layer		= BR2(v[1], 1); \
	f.crc		= BR1(v[1], 0); \
	f.bitrate	= BR4(v[2], 4); \
	f.frequency	= BR2(v[2], 2); \
	f.padding	= BR1(v[2], 1);
/* ����֡���� */
#define frame_length(f, len) do { \
		if(tableFrequency[f.version][f.frequency] == 0) break; \
		if((len = tableRatio[f.version][f.layer] * tableBitrates[(f.version << 2) | f.layer][f.bitrate]) == 0) break; \
		len /= tableFrequency[f.version][f.frequency]; \
		len += (f.crc == 0) ? 2 : 0; \
		len += (f.padding == 0) ? 0 : 1; \
	} while(0);
/* ѭ����Ѱ����һ֡ */
#define frame_next(v, f, len) \
	if(fread(v, sizeof(unsigned char), 4, fp) < 4) break; \
	if(!frame_check(v)) break; \
	frame_decode(v, f); \
	frame_length(f, len); \
	if(len <= 4) break; \
	len -= 4;
/* ���ֽڵ� 4 λѹ��֡ͷ */
#define frame_pack_low(v, s, c) \
	BS(v[2], 0, BR1(s, 0)); \
	BS(v[3], 3, BR1(s, 1)); \
	BS(v[3], 2, BR1(s, 2)); \
	BS(v[3], 1, BR1(s, 3)); \
	BS(v[3], 0, c);
/* ���ֽڸ� 4 λѹ��֡ͷ */
#define frame_pack_high(v, s, c) \
	BS(v[2], 0, BR1(s, 4)); \
	BS(v[3], 3, BR1(s, 5)); \
	BS(v[3], 2, BR1(s, 6)); \
	BS(v[3], 1, BR1(s, 7)); \
	BS(v[3], 0, c);
/* ��֡ͷ��������, д��� 4 λ */
#define frame_unpack_low(v, s, c) \
	s |= BR1(v[2], 0) << 0; \
	s |= BR1(v[3], 3) << 1; \
	s |= BR1(v[3], 2) << 2; \
	s |= BR1(v[3], 1) << 3; \
	c = BR1(v[3], 0);
/* ��֡ͷ��������, д��� 4 λ */
#define frame_unpack_high(v, s, c) \
	s |= BR1(v[2], 0) << 4; \
	s |= BR1(v[3], 3) << 5; \
	s |= BR1(v[3], 2) << 6; \
	s |= BR1(v[3], 1) << 7; \
	c = BR1(v[3], 0);

/* �ж��ǲ��Ǻϲ�ͷ */
#define header_check(h) (h.id == 0xA9)
/* �ж��ǲ�����չͷ */
#define extend_check(e) ((e.before[0] == 0x5E) && (e.before[1] == 0x39) && (e.after == 0x9B))


/*
	$ mp3_info					��ȡ MP3 �ļ���Ϣ

	@ FILE *fp

	# mp3_s *					�ɹ����� t_mp3 �ṹ��ָ��, ʧ�ܷ��� NULL
*/
mp3_s *mp3_info(FILE *fp);

/*
	$ mp3_read					�� MP3 �ж�ȡ����
	
	@ FILE *fp
	@ mp3_s *mp3
	@ seek_t *size				��ȡ��Ϣ�ĳ���
	@ void *arg					�ṩ���ص������Ĳ���
	@ int (*callback)(seek_t, char, char *, void *)

	# char *					�ɹ���������ָ��, ʧ�ܷ��� NULL
*/
char *mp3_read(FILE *fp, mp3_s *mp3, seek_t *size, void *arg, int (*callback)(seek_t, char, char *, void *));

/*
	$ mp3_save					�� MP3 �б�����Ϣ

	@ FILE *fp
	@ mp3_s *mp3				MP3 �ļ���Ϣ
	@ char *data				��Ҫ�������Ϣ
	@ seek_t size				��Ҫ�������Ϣ�ĳ���
	@ void (*callback)(double)

	# int						0=ʧ��, 1=�ɹ�, 2=�ɹ� (ʹ������չ��ʽ)
*/
int mp3_save(FILE *fp, mp3_s *mp3, char *data, seek_t size, void (*callback)(double));


#endif
