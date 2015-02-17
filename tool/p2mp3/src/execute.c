/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ code.c, 0.1.1005
*/

#include "base.h"
#include "execute.h"
#include "win32.h"
#include "code.h"
#include "mp3.h"
#include "gui.h"

/* ���ƻ�������С */
#define COPY_BUFFER		4 * 1024 * 1024
/* MP3 ����ɸѡ�� */
#define MP3_FILTER		"MP3 �ļ� (*.mp3)\0*.mp3\0"


void push_callback(double progress) {
	static int oldpercent = 0;

	int curpercent = (int) (progress * 100);
	char text[5];

	curpercent = (curpercent < 0) ? 0 : curpercent;
	curpercent = (curpercent > 100) ? 100 : curpercent;

	if(curpercent != oldpercent) {
		oldpercent = curpercent;

		sprintf(text, "%d%%", curpercent);

		buttonPushText(text);
	}
}

int pop_callback(seek_t length, char read, char *buffer, void *arg) {
	header_s *header = (header_s *) arg;

	/* ��ȡ��һ���ֽ� */
	if(length == 0) {
		if(((unsigned char) read) == 0xA9) {
			return 1;
		} else {
			return 0;
		}
	}

	/* �ȴ���Ϣͷ */
	if(length < sizeof(header_s)) {
		return 1;
	}
	/* ��Ϣͷ���� */
	if(length == sizeof(header_s)) {
		m_copy(header, buffer, length);
	}

	/* �ж��Ƿ��ȡ��� */
	if(length < (header->size + sizeof(header_s))) {
		return 1;
	} else {
		return 0;
	}
}


t_register(push) {
	void **argv		= (void **) arg;

	mp3_s *mp3		= (mp3_s *) argv[0];	/* �ļ���Ϣ */
	FILE *mp3fp		= (FILE *) argv[1];		/* MP3 �ļ���� */
	FILE *dstfp		= (FILE *) argv[2];		/* Ŀ���ļ���� */
	char *data		= (char *) argv[3];		/* ��Ҫ��������� */
	seek_t size		= (seek_t) argv[4];		/* ��Ҫ��������ݳߴ� */
	char *dstfile	= (char *) argv[5];		/* Ŀ���ļ�·�� */

	char *buffer;
	int result;

	seek_t bufsize = COPY_BUFFER;

	m_new(buffer, char, COPY_BUFFER);

	buttonEnable(0);
	running = 1;

	/* �����ļ� */
	while(!feof(mp3fp)) {
		if(bufsize = (seek_t) fread(buffer, sizeof(char), COPY_BUFFER, mp3fp)) {
			fwrite(buffer, sizeof(char), bufsize, dstfp);
		}

		bufsize = COPY_BUFFER;
	}

	result = mp3_save(dstfp, mp3, data, size, push_callback);

	fclose(mp3fp);
	fclose(dstfp);

	m_del(buffer);
	m_del(data);

	buttonEnable(1);
	buttonPushText(NULL);
	running	= 0;

	switch(result) {
		case 0:
			alert("�ϲ��ļ�ʧ�ܣ������ļ��Ƿ���ȷ��");
			unlink(dstfile);

			return;

		case 1: success("�ļ��ϲ��ɹ���ɡ�"); break;
		case 2: success("�ļ��ϲ��ɹ���ɣ����κϲ�ʹ������չģʽ��"); break;
	}

	explorer(dstfile);
}

t_register(pop) {
	void **argv	= (void **) arg;

	mp3_s *mp3	= (mp3_s *) argv[0];	/* �ļ���Ϣ */
	FILE *mp3fp	= (FILE *) argv[1];		/* MP3 �ļ���� */

	FILE *fp;
	header_s header;
	seek_t size;

	char *all, *data = NULL, *temp, *key;
	char *filename;
	int done = 0;

	buttonEnable(0);
	running = 1;

	m_zero(&header, sizeof header);

	do {
		done = 1;

		if((all = mp3_read(mp3fp, mp3, &size, &header, pop_callback)) == NULL) {
			alert("��ȡ�ļ���Ϣʧ�ܣ����ܸ��ļ������ϲ����ݻ��������𻵡�"); break;
		}
		if(!header_check(header)) {
			alert("��ȡ���ĺϲ���Ϣ����ʹ�ã��������ļ����𻵡�"); break;
		}

		/* �������� */
		size -= sizeof header;

		if(size != header.size) {
			alert("��ȡ�������ݳ��Ȳ���ȷ���������ļ����𻵡�"); break;
		}
		if(header.crcdat != crc32(all + sizeof header, size)) {
			alert("��ȡ����Ϣ�޷�ͨ����֤���������ļ����𻵡�"); break;
		}

		/* �������� */
		m_new(data, char, size);
		m_copy(data, all + sizeof header, size);
		m_del(all);

		done = 0;
		temp = data;

		while(header.crcdat != header.crcraw) {
			if((key = inputbox("��Կ", "��������Կ�����ܺϲ������ݣ�", "")) == NULL) {
				done = 1; break;
			}
			if(key[0] == 0) {
				continue;
			}

			key = md5(key);

			/* ����ʧ�ܺ��ͷ��ϴεĿռ� */
			if(temp != data) {
				m_del(temp);
			}

			/* ���� */
			temp = rc4(data, size, key);
			header.crcdat = crc32(temp, size);

			m_del(key);
		}

		/* ��� done = 0, Ҫôû�м���, Ҫô���ܳɹ� */
		if(done == 0) {
			if(temp != data) {
				m_del(data);

				data = temp;
			}
		}
	} while(0);

	if(done == 0) {
		/* ����ԭ������չ������Ĭ���ļ��� */
		if(header.ext[0]) {
			ms_new(filename, strlen(header.ext) + 16);

			strcat(filename, "result.");
			strcat(filename, header.ext);
		} else {
			filename = NULL;
		}
	}

	while(done == 0) {
		if((filename = select_file_save(filename, "��ѡ���ļ�����·��")) == NULL) {
			break;
		}
		if((fp = fopen(filename, "wb")) == NULL) {
			alert("ѡ�����ļ�·���޷��򿪣�������ѡ��"); continue;
		}

		fwrite(data, sizeof(char), size, fp);
		fclose(fp);

		explorer(filename);

		break;
	}

	fclose(mp3fp);

	m_del(data);

	buttonEnable(1);
	running = 0;
}


void push() {
	static void *arg[6] = {NULL};

	mp3_s *mp3;
	header_s header;
	seek_t maxsize;
	
	FILE *mp3fp, *datfp, *dstfp;

	char *mp3file, *datfile, *dstfile, *key;
	char *data, *temp, title[50], message[256];

	if(running) {
		return;
	}

	while(1) {
		if((mp3file = select_file_open(MP3_FILTER, "��ѡ�� MP3 Դ�ļ�")) == NULL) {
			return;
		}

		if((mp3fp = fopen(mp3file, "rb+")) == NULL) {
			alert("ѡ�����ļ��޷��򿪣�������ļ��Ƿ���ڻ����ڱ�ʹ�á�"); continue;
		}

		if((mp3 = mp3_info(mp3fp)) == NULL) {
			alert("����ѡ�����ļ�ʧ�ܣ����ļ����ܲ���һ���Ϸ��� MP3 �ļ���"); fclose(mp3fp); continue;
		}
		if(mp3->count < 100) {
			alert("ѡ����ļ��а�������Ƶʱ������̫�̣������˴洢���ݡ�"); fclose(mp3fp); continue;
		}

		break;
	}

	/* ������������� */
	maxsize = (mp3->count - 2) / 2;

	/* ���ɱ��� */
	if(maxsize < 1024) {
		sprintf(title, "��ѡ����Ҫ�ϲ����ļ�������С�� %d Byte��", maxsize);
		sprintf(message, "��� MP3 Դ�ļ����˵Ĵ���������ԼΪ %d Byte�������Ժ���ѡ�ĺϲ��ļ���Ҫ������������", maxsize);
	} else {
		sprintf(title, "��ѡ����Ҫ�ϲ����ļ�������С�� %.2f KByte��", ((double) maxsize) / 1024);
		sprintf(message, "��� MP3 Դ�ļ����˵Ĵ���������ԼΪ %.2f KByte�������Ժ���ѡ�ĺϲ��ļ���Ҫ������������", ((double) maxsize) / 1024);
	}

	success(message);

	while(1) {
		if((datfile = select_file_open(NULL, title)) == NULL) {
			fclose(mp3fp); return;
		}

		if((datfp = fopen(datfile, "rb")) == NULL) {
			alert("ѡ�����ļ��޷��򿪣�������ļ��Ƿ���ڻ����ڱ�ʹ�á�"); continue;
		}

		/* ��ȡ�ļ����� */
		fseek(datfp, 0, SEEK_END);
		header.size = ftell(datfp);

		/* ������ʾ */
		if(header.size == 0) {
			alert("ѡ�����ļ���һ�����ļ�������֧�ֺϲ����ļ���������ѡ��"); fclose(datfp); continue;
		}
		if(header.size > maxsize) {
			if(!confirm("ѡ�����ļ��Ĵ�С���� MP3 Դ�ļ������ṩ����󴢴��������ϲ����ܻᵼ�� MP3 �Ľṹ�����仯���Ƿ������")) {
				fclose(datfp); continue;
			}
		}

		break;
	}

	while(1) {
		if((dstfile = select_file_save("result.mp3", "��ѡ�񱣴��·��")) == NULL) {
			fclose(mp3fp); fclose(datfp); return;
		}

		if(strcmp(dstfile, mp3file) == 0) {
			alert("����·�������� MP3 Դ�ļ�·����ͬ��"); continue;
		}
		if(strcmp(dstfile, datfile) == 0) {
			alert("����·����������Ҫ�ϲ����ļ�·����ͬ��"); continue;
		}

		if((dstfp = fopen(dstfile, "wb+")) == NULL) {
			alert("�޷�����ѡ����·����������ѡ��"); continue;
		}

		break;
	}

	if((key = inputbox("��Կ", "�������Ҫ�Ժϲ������ݽ��м��ܣ���������Կ��", "")) != NULL) {
		if(key[0] == 0) {
			key = NULL;
		}
	}

	rewind(mp3fp);
	rewind(datfp);
	rewind(dstfp);

	/* ��ȡ��Ҫ�ϲ������ݵ������� */
	m_new(data, char, header.size);
	fread(data, sizeof(char), header.size, datfp);
	fclose(datfp);

	header.id = 0xA9;
	header.crcraw = crc32(data, header.size);

	m_zero(header.ext, 5);

	/* �����Ҫ���� */
	if(key != NULL) {
		key = md5(key);

		temp = data;
		data = rc4(temp, header.size, key);

		m_del(key);
		m_del(temp);

		header.crcdat = crc32(data, header.size);
	} else {
		header.crcdat = header.crcraw;
	}

	/* Ѱ����չ�� */
	do {
		if((temp = fileext(datfile)) == NULL) {
			break;
		}
		if(temp[0] == 0) {
			break;
		}
		if(strlen(temp) > 4) {
			break;
		}

		strcpy(header.ext, temp);
	} while(0);

	/* �������� */
	m_new(temp, char, header.size + sizeof header);
	m_copy(temp, &header, sizeof header);
	m_copy(temp + sizeof header, data, header.size);
	m_del(data);
	
	data = temp;
	header.size += sizeof header;

	/* ���ò��� */
	arg[0] = mp3;
	arg[1] = mp3fp;
	arg[2] = dstfp;
	arg[3] = data;
	arg[4] = (void *) header.size;
	arg[5] = dstfile;

	t_start(push, arg);
}

void pop() {
	static void *arg[2] = {NULL};

	mp3_s *mp3;
	FILE *fp;

	char *mp3file;
	if(running) {
		return;
	}

	while(1) {
		if((mp3file = select_file_open(MP3_FILTER, "��ѡ����Ҫ��ȡ���ݵ� MP3 �ļ�")) == NULL) {
			return;
		}

		if((fp = fopen(mp3file, "rb+")) == NULL) {
			alert("ѡ�����ļ��޷��򿪣�������ļ��Ƿ���ڻ����ڱ�ʹ�á�"); continue;
		}

		if((mp3 = mp3_info(fp)) == NULL) {
			alert("����ѡ�����ļ�ʧ�ܣ����ļ����ܲ���һ���Ϸ��� MP3 �ļ���"); fclose(fp); continue;
		}
		if(mp3->count < 100) {
			alert("ѡ����ļ��а�������Ƶʱ������̫�̣������˴洢���ݡ�"); fclose(fp); continue;
		}

		break;
	}

	arg[0] = mp3;
	arg[1] = fp;

	t_start(pop, arg);
}
