/*
	$ io.c, 0.2.1221
*/

#include "base.h"


/* Ĭ�ϴ򿪺ͱ�����ļ��� */
#define FILE_OPEN		"file.1"
#define FILE_SAVE		"file.2"

/* ����������������С�� (����) */
#define MAX			1e10 + 1
#define MIN			0



/* ���ļ� */
static char * open() {
	char *buffer;
	OPENFILENAME file;

	NEW(buffer, char, MAX_PATH + 1);
	ZeroMemory(&file, sizeof file);

	file.lStructSize	= sizeof file;
	file.hwndOwner		= win;
	file.lpstrTitle		= FILE_OPEN;
	file.lpstrFilter	= FILE_OPEN "\0" FILE_OPEN "\0�����ļ�\0*.*\0";
	file.nFilterIndex	= 1;
	file.lpstrFile		= buffer;
	file.nMaxFile		= MAX_PATH;
	file.Flags			= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&file) == FALSE) {
		DEL(buffer);
	}

	return buffer;
}

/* �����ļ� */
static char * save() {
	char *buffer;
	OPENFILENAME file;

	NEW(buffer, char, MAX_PATH + 1);
	ZeroMemory(&file, sizeof file);

	strcpy(buffer, FILE_SAVE);

	file.lStructSize	= sizeof(OPENFILENAME);
	file.hwndOwner		= win;
	file.lpstrTitle		= FILE_SAVE;
	file.lpstrFilter	= FILE_SAVE "\0" FILE_SAVE "\0�����ļ�\0*.*\0";
	file.nFilterIndex	= 1;
	file.lpstrFile		= buffer;
	file.lpstrDefExt	= "2";
	file.nMaxFile		= MAX_PATH;
	file.Flags			= OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&file) == FALSE) {
		DEL(buffer);
	}

	return buffer;
}


void input() {
	/* ���뻺�� */
	int			number;
	char		buffer[80];
	/* ������ָ�� */
	int_t		remainder, exponent;

	/* �ļ� */
	char		*file;
	FILE		*fp;
	/* ���� */
	number_s	*num = NUM;

	/* ����������� */
	if(NUM->next) {
		num		= NUM->next;
		number	= 0;

		/* δ��ɼ����û����Ľ�� */
		while(num) {
			if((num->doneCalc == 0) || (num->doneOutput == 0)) {
				number = 1; break;
			}

			num = num->next;
		}

		if(number) {
			if(!CONFIRM("��������û�м��㣬�����Ľ��û�б��棬��ʱ��ѡ��һ���ļ�����������ζ�ŷ�����Щ��Ϣ���Ƿ������")) {
				return;
			}
		}

		num = NUM;
	}

	if((file = open()) == NULL) {
		return;
	}

	if(fp = fopen(file, "r")) {
		DEL(file);
	} else {
		ALERT("��ѡ�����ļ�ʧ�ܣ������ļ������ڣ�û�з���Ȩ�޻������ڱ�����Ӧ�ó���ʹ�á�");
		DEL(file);

		return;
	}

	/* ɾ�����е��������� */
	if(NUM->next) {
		while(NUM->next) {
			num = NUM->next->next;

			DELA(NUM->next->data);
			DEL(NUM->next);

			NUM->next = num;
		}

		num = NUM;

		listClear();
	}

	while(!feof(fp)) {
		number = -1;
		
		/* ��ȡһ�� */
		if(fgets(buffer, sizeof buffer, fp) == NULL) {
			break;
		}

		/* ��������Ƿ���Ч */
		if(sscanf(buffer, "%d", &number) < 1) {
			continue;
		}
		if((number <= MIN) || (number >= MAX)) {
			continue;
		}

		/* ��λ */
		remainder	= (int_t) number;
		exponent	= 0;

		/* �и�����Ϊ X + 3 * N */
		if(remainder % 3) {
			if(remainder > 4) {
				exponent	= remainder / 3;
				remainder	= remainder % 3;

				if(remainder == 1) {
					exponent--;
					remainder = 4;
				}
			}
		} else {
			exponent	= remainder / 3;
			remainder	= 0;
		}

		NEW(num->next, number_s, 1);

		num				= num->next;
		num->number		= (int_t) number;
		num->remainder	= remainder;
		num->exponent	= exponent;

		listAppend(num);
	}

	if(NUM->next == NULL) {
		ALERT("����ѡ���ļ���û�ж�ȡ���κ���ȷ�Ļ��ڴ���Χ�ڵ����֡�");

		calcEnable(0);
	} else {
		calcEnable(1);
	}

	saveEnable(0);

	fclose(fp);
}

void output() {
	/* ������Ƴ��� */
	size_t		limit;
	/* ������� */
	size_t		count, length;
	/* ��������� */
	char		buffer[16];

	/* �ļ� */
	char		*file;
	FILE		*fp;

	/* ��ǰλָ�� */
	int_t		*number;
	/* ���� */
	number_s	*num = NUM->next;

	/* ����Ƿ����Ѵ�����ɵ����� */
	do {
		while(num) {
			if(num->doneCalc) {
				break;
			}

			num = num->next;
		}
		
		if((num == NULL) || (num->doneCalc == 0)) {
			return;
		}

		num = NUM->next;
	} while(0);

	/* ��ȡ������� */
	if((limit = textRead()) == 0) {
		if(!CONFIRM("����ı��泤����Ч���Ƿ񱣴�ȫ���ȵļ�������")) {
			return;
		}
	}

	if((file = save()) == NULL) {
		return;
	}

	if(fp = fopen(file, "wb")) {
		DEL(file);
	} else {
		ALERT("��ѡ�����ļ�·��ʧ�ܣ�����û�з���Ȩ�ޡ�");
		DEL(file);

		return;
	}

	while(num) {
		if(num->doneCalc) {
			count	= num->countDec;
			length	= num->lenHigh;

			/* ���λ */
			number	= num->data + ((count - 1) * OFFSET) + 1;
			sprintf(buffer, "%lu", *number);

			/* ������� */
			fprintf(fp, "%lu\r\n", num->number);

			/* �����λ���ȴ���������� */
			if(limit && (length > limit)) {
				/* ���� NULL */
				buffer[limit] = 0;

				/* ֱ����� */
				fprintf(fp, "%s", buffer);
			} else {
				/* ��������λ */
				fprintf(fp, "%s", buffer);

				/* ���ʣ��λ */
				if(--count) {
					while(count--) {
						/* ������ȼ�� */
						if(limit) {
							/* ���� */
							if(length >= limit) {
								break;
							}

							/* ���� */
							if((limit - length) < 9) {
								/* ����������� */
								sprintf(buffer, "%09lu", *(num->data + (count * OFFSET) + 1));
								/* �������� */
								buffer[limit - length] = 0;

								fprintf(fp, "%s", buffer);

								break;
							}
						}

						fprintf(fp, "%09lu", *(num->data + (count * 2) + 1));
						length += 9;
					}
				}
			}

			fprintf(fp, "\r\n%lu\r\n\r\n", num->lenAll);

			num->doneOutput = 1;
		}

		num = num->next;
	}

	fclose(fp);
}
