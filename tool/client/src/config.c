/*
	$ MF Client   (C) 2005-2013 MF
	$ config.c
*/

#include "base.h"


/* �����ļ� */
#define CONFIG				"client.dat"

/* �ļ�ͷ��ʶ�� */
#define MAGIC				0xAB89FF00



/* �ļ�ͷ */
typedef struct {
	DWORD	magic;					/* ��ʶ�� */
	BYTE	len_username;			/* �û������� */
	BYTE	len_password;			/* ���볤�� */
	BYTE	adapter[6];				/* MAC ��ַ */
	BOOL	hidden;
	BOOL	login;
} config_header_t;



/* �˺� */
static char	*username	= NULL;
static char	*password	= NULL;

/* �����豸 */
static BYTE	*adapter	= NULL;

/* ���� */
static BOOL	hidden		= FALSE;
static BOOL	login		= FALSE;



/*
	configRead										��ȡ������Ϣ

	# int											�Ƿ�ɹ���ȡ��Ϣ
*/
int configRead() {
	int				result = 0;

	FILE			*fp;
	config_header_t	header;

	if(fopen_s(&fp, CONFIG, "rb")) {
		return result;
	}

	if(fread(&header, sizeof header, 1, fp)) {
		if(header.magic == MAGIC) {
			result	= 1;

			hidden	= header.hidden;
			login	= header.login;

			MEMORY_ALLOC	(adapter,	BYTE,				6);
			memcpy			(adapter,	header.adapter,		6);

			do {
				/* ��ȡ�û��� */
				if(header.len_username && (header.len_username < 64)) {
					MEMORY_ALLOC(username, char, header.len_username + 2);

					if(fread(username, sizeof(char), header.len_username, fp) != header.len_username) {
						MEMORY_FREE(username);

						break;
					}
				}

				/* ��ȡ���� */
				if(header.len_password && (header.len_password < 64)) {
					MEMORY_ALLOC(password, char, header.len_password + 2);

					if(fread(password, sizeof(char), header.len_password, fp) != header.len_password) {
						MEMORY_FREE(password);

						break;
					}
				}
			} while(0);
		}
	}

	fclose(fp);

	return result;
}

/*
	configWrite										д��������Ϣ
*/
void configWrite() {
	FILE			*fp;
	config_header_t	header;

	ZeroMemory(&header, sizeof header);

	header.magic		= MAGIC;
	header.hidden		= hidden;
	header.login		= login;

	if(adapter) {
		memcpy_s	(header.adapter, sizeof header.adapter, adapter, 6);
	}  else {
		ZeroMemory	(header.adapter, sizeof header.adapter);
	}

	/* �û������볤�� */
	if(username) {
		header.len_username	= strnlen_s(username, 0xFF);
	}
	if(password) {
		header.len_password = strnlen_s(password, 0xFF);
	}

	if(fopen_s(&fp, CONFIG, "wb")) {
		return;
	}

	fwrite(&header, sizeof header, 1, fp);

	/* д���û������� */
	if(header.len_username) {
		fwrite(username, sizeof(char), header.len_username, fp);
	}
	if(header.len_password) {
		fwrite(password, sizeof(char), header.len_password, fp);
	}

	fclose(fp);
}

/*
	configClear										ɾ��������Ϣ
*/
void configClear() {
	_unlink(CONFIG);
}


/*
	configGetUsername								����������
	configGetPassword
	configGetAdapter
	configGetHidden
	configGetLogin
*/
char *	configGetUsername	() { return username;	}
char *	configGetPassword	() { return password;	}
BYTE *	configGetAdapter	() { return adapter;	}
BOOL	configGetHidden		() { return hidden;		}
BOOL	configGetLogin		() { return login;		}


/*
	configSetUsername								����������
	configSetPassword
	configSetAdapter
	configSetHidden
	configSetLogin
*/
void	configSetUsername	(char	*value)	{ if(username)	{ MEMORY_FREE (username);	}	username	= value; }
void	configSetPassword	(char	*value)	{ if(password)	{ MEMORY_FREE (password);	}	password	= value; }
void	configSetAdapter	(BYTE	*value)	{ if(adapter)	{ MEMORY_FREE (adapter);	}	adapter		= value; }
void	configSetHidden		(BOOL	value)	{ hidden	= value; }
void	configSetLogin		(BOOL	value)	{ login		= value; }
