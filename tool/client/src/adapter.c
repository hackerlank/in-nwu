/*
	$ MF Client   (C) 2005-2013 MF
	$ config.c
*/

/* pcap */
#include "pcap/pcap.h"
#include "pcap/Packet32.h"

#include "base.h"

#include <iphlpapi.h>



/* �����豸 */
typedef struct client_adapter_t {
	char	*name;					/* �豸���� */
	char	*handle;				/* �򿪾�� */

	BYTE	mac[6];					/* MAC ��ַ */

	BOOL	basic;					/* �������� (�����ʾ�϶�Ϊ����/��������) */
	BOOL	pcap;					/* �Ƿ�Ϊ winpcap �򿪾�� */

	struct client_adapter_t *next;
} client_adapter_t;



/* �豸�� */
static client_adapter_t *table		= NULL;

/* �豸��Ŀ */
static size_t	table_count_all		= 0;
static size_t	table_count_basic	= 0;

/* �����ѡ */
static int		current_basic		= 0;
static int		current_index		= 0;



/*
	stristr
*/
char * stristr(char *str, char *sub) {
	size_t len;

	if((len = strlen(sub)) == 0) {
		return NULL;
	}

	while(*str) {
		if(_strnicmp(str, sub, len) == 0) {
			return str;
		}
		
		str++;
	}

	return NULL;
}

/*
	unicode2ansi
*/
char * unicode2ansi(wchar_t *source) {
	int		length;
	char	*result;
	
	if((length = WideCharToMultiByte(CP_ACP, 0, source, -1, NULL, 0, NULL, NULL)) <= 0) {
		return NULL;
	}

	MEMORY_ALLOC(result, char, length + 1);

	WideCharToMultiByte(CP_ACP, 0, source, -1, result, length, NULL, NULL);

	return result;
}


/*
	deviceBasic
*/
static void deviceBasic(client_adapter_t *object) {
	char *name = object->name;

	table_count_all++;

	if(stristr(object->name, "virtual")) {
		return;
	}
	if(stristr(object->name, "wifi")) {
		return;
	}
	if(stristr(object->name, "wi-fi")) {
		return;
	}
	if(stristr(object->name, "wireless")) {
		return;
	}
	if(stristr(object->name, "wlan")) {
		return;
	}
	if(stristr(object->name, "bluetooth")) {
		return;
	}

	if(strstr(object->name, "����")) {
		return;
	}
	if(strstr(object->name, "����")) {
		return;
	}
	if(strstr(object->name, "����")) {
		return;
	}

	object->basic = TRUE;

	table_count_basic++;
}

/*
	deviceInitSystem
*/
static void deviceInitSystem() {
	size_t		length_desc;
	size_t		length_name;

	client_adapter_t	*object	= table;

	ULONG				output	= sizeof(IP_ADAPTER_INFO);

	PIP_ADAPTER_INFO	adapter;

	MEMORY_ALLOC(adapter, IP_ADAPTER_INFO, 1);

	/* ��ȡ�����ڴ��С */
	if(GetAdaptersInfo(adapter, &output) == ERROR_BUFFER_OVERFLOW) {
		MEMORY_FREE		(adapter);
		MEMORY_ALLOCS	(adapter, IP_ADAPTER_INFO, output);
	}

	if(GetAdaptersInfo(adapter, &output) == NO_ERROR) {
		PIP_ADAPTER_INFO current = adapter;

		while(current) {
			/* �ַ������� */
			length_desc = strnlen_s(current->Description, sizeof current->Description) + 2;
			length_name = strnlen_s(current->AdapterName, sizeof current->AdapterName) + 2;

			MEMORY_ALLOC(object->next,			client_adapter_t,	1);
			MEMORY_ALLOC(object->next->name,	char,				length_desc);
			MEMORY_ALLOC(object->next->handle,	char,				length_name);
			
			strcpy_s(object->next->name,	length_desc,	current->Description);
			strcpy_s(object->next->handle,	length_name,	current->AdapterName);

			memcpy_s(object->next->mac,		6,				current->Address,		6	);

			object	= object->next;
			current	= current->Next;
		}
	}

	MEMORY_FREE(adapter);
}

/*
	deviceInitPcap
*/
static void deviceInitPcap() {
	char	error[PCAP_ERRBUF_SIZE];
	size_t	length;

	client_adapter_t	*object;

	pcap_if_t			*device;
	pcap_if_t			*current;

	pcap_findalldevs(&device, error);

	/* ���ؽ���� */
	if(device == NULL) {
		/* �Թ���ԱȨ���������� */
		restart();
	}

	current = device;

	/* �����豸 */
	while(current) {
		object = table->next;

		/* �����豸�� */
		while(object) {
			if(object->pcap == FALSE) {
				if(stristr(current->name, object->handle)) {
					object->pcap	= TRUE;

					length			= strnlen_s(current->name, 0xFFFF) + 4;

					MEMORY_FREE		(object->handle);
					MEMORY_ALLOC	(object->handle,	char,	length);

					strcpy_s		(object->handle,	length,	current->name);

					/* �����豸 */
					deviceBasic(object);

					break;
				}
			}

			object = object->next;
		}

		current = current->next;
	}

	pcap_freealldevs(device);
}


/*
	deviceInit										��ʼ�������豸
*/
void deviceInit() {
	client_adapter_t	*object;

	/* ��ʼ��������豸�� */
	if(table == NULL) {
		MEMORY_ALLOC(table, client_adapter_t, 1);
	} else {
		while(table->next) {
			object = table->next->next;

			MEMORY_FREE(table->next->name);
			MEMORY_FREE(table->next->handle);
			MEMORY_FREE(table->next);

			table->next = object;
		}
	}

	object				= table;

	table_count_all		= 0;
	table_count_basic	= 0;

	/* ��ϵͳ��ȡ�豸�� */
	deviceInitSystem	();
	/* �� winpcap ��ȡ�豸�� */
	deviceInitPcap		();

	/* �����Ч��Ŀ */
	while(object->next) {
		if(object->next->pcap == FALSE) {
			client_adapter_t *next = object->next->next;

			MEMORY_FREE(object->next);

			object->next->next;
		}

		object = object->next;
	}
}

/*
	deviceGet										��ȡ�豸����

	@ int		basic								�����豸
	@ BYTE		*mac								MAC ��ַ

	# int											δ�ҵ����� -1
*/
int deviceGet(int basic, BYTE *mac) {
	int i = 0;
	client_adapter_t *object = table->next;

	while(object) {
		/* �Ա� MAC */
		if(memcmp(object->mac, mac, sizeof object->mac) == 0) {
			if(basic && (object->basic == FALSE)) {
				return 0;
			}

			return i;
		}

		if(basic <= object->basic) {
			i++;
		}

		object = object->next;
	}

	return 0;
}

/*
	deviceList										��ȡ�����豸�б�

	@ int		basic
	@ void		*list								�����ַ�������

	# size_t										�豸��Ŀ
*/
size_t deviceList(int basic, void *list) {
	size_t	count;

	char	**p;
	client_adapter_t *object = table->next;

	if(basic) {
		count = table_count_basic;
	} else {
		count = table_count_all;
	}

	MEMORY_ALLOC(p, char *, count);

	/* ����ֵ */
	*((char ***) list) = p;

	while(object) {
		if(basic <= object->basic) {
			*(p++) = object->name;
		}

		object = object->next;
	}

	return count;
}

/*
	deviceSelect									�������豸�л�ȡ��ѡ�豸��Ϣ

	@ int		basic
	@ int		index								����
	@ BYTE		**mac								���� MAC ��ַ

	# char		*									�豸���
*/
char * deviceSelect(int basic, int index, BYTE **mac) {
	int i = 0;
	client_adapter_t *object = table->next;

	current_basic	= basic;
	current_index	= index;

	while(object) {
		if(basic <= object->basic) {
			if((i++) == index) {
				MEMORY_ALLOC	(*mac, BYTE,		6);
				memcpy			(*mac, object->mac,	6);

				return object->handle;
			}
		}

		object = object->next;
	}

	return NULL;
}

/*
	deviceCompare									�ж�ָ���豸�Ƿ�Ϊ��ѡ�豸

	@ WCHAR		*name

	# int
*/
int deviceCompare(WCHAR	*name) {
	int i = 0;
	client_adapter_t *object = table->next;

	/* �����豸 */
	while(object) {
		if(current_basic <= object->basic) {
			if((i++) == current_index) {
				goto found;
			}
		}

		object = object->next;
	}

	/* û��ѡ�豸���� 0 */
	return 0;

found:
	{
		int		result		= 0;
		char	*name_ansi;

		/* "{" ��ʼλ�� */
		char	*source_start;
		char	*target_start;

		/* ת���豸��Ϊ ANSI */
		if((name_ansi = unicode2ansi(name)) == NULL) {
			return 0;
		}

		source_start = strstr(object->handle, "{");
		target_start = strstr(name_ansi, "{");

		if(source_start && target_start) {
			if(strncmp(source_start, target_start, 256) == 0) {
				result = 1;
			}
		}

		MEMORY_FREE(name_ansi);

		return result;
	}
}
