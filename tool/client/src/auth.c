/*
	$ MF Client   (C) 2005-2013 MF
	$ auth.c
*/

#include "pcap/pcap.h"
#include "pcap/Packet32.h"

#include <stdint.h>
#include <string.h>

#include "base.h"



/* 8 �ֽ������Կ */
#define KEY_RANDOM					"00000000"

/* ���ӷ������ȴ����� */
#define RETRY_CONNECT				50
/* �ȴ�Ӧ����Ϣ��ʱ�� (s) */
#define RETRY_LOOP					600



/* EAP ���� */
typedef enum { REQUEST=1, RESPONSE=2, SUCCESS=3, FAILURE=4, H3CDATA=10 }	eap_code_t;
/* EAP ���� */
typedef enum { IDENTITY=1, NOTIFICATION=2, MD5=4, AVAILABLE=20 }			eap_type_t;



/* �㲥 MAC ��ַ */
static const BYTE address_broadcast[6]	= {0xff,0xff,0xff,0xff,0xff,0xff};
/* �ಥ MAC ��ַ */
static const BYTE address_multicast[6]	= {0x01,0x80,0xc2,0x00,0x00,0x03};

/* ��Ϊ�ͻ��˰汾�� */
static const char h3c_version[16]		= "EN V2.40-0335";
/* H3C �̶���Կ */
static const char h3c_key[]				= "HuaWei3COM1X";


/* ������Ϣ */
static pcap_t	*connect_handle;
static BYTE		*connect_mac;



/*
	XOR
*/
static void XOR(uint8_t *data, size_t data_len, const char *key, size_t key_len) {
	size_t i, j;

	/* ����һ�� */
	for(i = 0; i < data_len; i++) {
		data[i] ^= key[i % key_len];
	}

	/* ����һ�� */
	for(i = data_len - 1, j = 0; j < data_len; i--, j++) {
		data[i] ^= key[j % key_len];
	}
}


/*
	getIpFromDevice
*/
static void getIpFromDevice(BYTE *ip, char *device) {
	char	hostname[255];
	struct	hostent *host = NULL;

	/* ��ñ��������� */
	gethostname(hostname, sizeof hostname);

	/* ������������ȡ IP */
	host = gethostbyname(hostname);

	memcpy(ip, (struct in_addr *) host->h_addr_list[0], 4);
}


/*
	fillClient
*/
static void fillClient(uint8_t *area) {
	/* �� 1 ���������, �� 00000000 Ϊ��Կ���� 16 �ֽ� */
	memcpy	(area,	h3c_key,	sizeof(h3c_key));
	XOR		(area,	16,			KEY_RANDOM,			8);

	/* ���� 4 �ֽ���Կ */
	memset	(area + 16,			0,					4);

	/* �� 2 ���������, �� h3c_key Ϊ��Կ����ǰ�����ɵ� 20 �ֽ� */
	XOR		(area, 20,			h3c_key,			strlen(h3c_key));
}

/*
	fillWindows
*/
static void fillWindows(uint8_t *area) {
	/* �̶��ַ��� */
	memset	(area, 0,			20);
	memcpy	(area, "r70393861",	20);

	XOR		(area, 20,			h3c_key, strlen(h3c_key));
}

/*
	fillBase64
*/
static void fillBase64(uint8_t *area) {
	/* base64 �ַ�ӳ��� */
	static char table[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	uint8_t	version[20];

	uint8_t	c1, c2, c3;
	size_t	i, j;

	i = 0;
	j = 0;

	/* ��ȡ���ܹ��� h3c �汾 */
	fillClient(version);

	/* base64(version) */
	while(j < 24) {
		c1 = version[i++];
		c2 = version[i++];
		c3 = version[i++];

		area[j++] = table[ (c1 & 0xFC) >> 2														];
		area[j++] = table[((c1 & 0x03) << 4)	| ((c2 & 0xF0) >> 4)							];
		area[j++] = table[						  ((c2 & 0x0F) << 2)	| ((c3 & 0xC0) >> 6)	];
		area[j++] = table[												    c3 & 0x3F			];
	}

	c1 = version[i++];
	c2 = version[i++];

	area[24] = table[ (c1 & 0xFC) >> 2							];
	area[25] = table[((c1 & 0x03) << 4)	| ((c2 & 0xF0) >> 4)	];
	area[26] = table[					  ((c2 & 0x0F) << 2)	];
	area[27] = '=';
}

/*
	fillMd5
*/
static void fillMd5(uint8_t *area, uint8_t id, char *password, uint8_t *source) {
	uint8_t	buffer[128];

	size_t	length;
	size_t	password_len;

	password_len	= strlen(password);
	length			= 17 + password_len;

	buffer[0] = id;

	memcpy	(buffer + 1,				password,	password_len);
	memcpy	(buffer + 1 + password_len,	source,		16);

	md5		((char *) buffer, length, (char *) area);
}


/*
	sendStartPackage
*/
static void sendStartPackage(pcap_t *handle, BYTE *mac) {
	uint8_t packet[18];

	memcpy(packet +  0, address_broadcast,	6);			/* �㲥 */
	memcpy(packet +  6, mac,				6);

	packet[12] = 0x88;
	packet[13] = 0x8E;

	/* EAPOL */
	packet[14] = 0x01;			/* version	= 1 */
	packet[15] = 0x01;			/* type		= start */
	packet[16] = 0x00;			/* length	= 0 */
	packet[17] = 0x00;

	pcap_sendpacket(handle, packet, sizeof packet);

	/* �ಥ */
	memcpy(packet + 0, address_multicast,	6);
	pcap_sendpacket(handle, packet, sizeof packet);
}

/*
	sendLogoffPackage
*/
void sendLogoffPackage(pcap_t *handle, BYTE *mac) {
	uint8_t packet[18];

	memcpy(packet +  0, address_multicast,	6);
	memcpy(packet +  6, mac,				6);

	packet[12] = 0x88;
	packet[13] = 0x8E;

	packet[14] = 0x01;			/* version	= 1 */
	packet[15] = 0x02;			/* type		= start */
	packet[16] = 0x00;			/* length	= 0 */
	packet[17] = 0x00;

	pcap_sendpacket(handle, packet, sizeof packet);
}


/*
	sendResponseNotification
*/
static void sendResponseNotification(pcap_t *handle, uint8_t *request, uint8_t *ethernet) {
	uint8_t	response[67];

	memcpy(response, ethernet, 14);

	/* 802.1x ��֤ */
	response[14] = 0x01;			/* 802.1x version 1 */
	response[15] = 0x00;			/* type=0 (EAP Packet) */
	response[16] = 0x00;			/* length */
	response[17] = 0x31;

	/* ��չ��֤Э�� */
	response[18] = RESPONSE;
	response[19] = request[19];		/* id */
	response[20] = 0x00;			/* length */
	response[21] = 0x31;
	response[22] = NOTIFICATION;	/* EAP type */

	/* ��֤��Ϣ, ǰ 2+20 �ֽ�Ϊ�ͻ��˰汾*/
	response[23] = 0x01;			/* type */
	response[24] = 22;				/* length */
	/* ��� 2+20 �ֽڴ洢���ܺ�� Windows ����ϵͳ�汾�� */
	response[45] = 0x02;			/* type */
	response[46] = 22;				/* length */

	fillClient	(response + 25);
	fillWindows	(response + 47);

	pcap_sendpacket(handle, response, sizeof response);
}

/*
	sendResponseIdentity
*/
static void sendResponseIdentity(pcap_t *handle, uint8_t *request, uint8_t *ethernet, BYTE *ip, char *username) {
	size_t		i;
	size_t		username_len;

	uint8_t		response[128];
	uint16_t	length;

	username_len = strlen(username);

	memcpy(response, ethernet, 14);

	/* 802.1x ��֤ */
	response[14] = 0x01;			/* 802.1x version 1 */
	response[15] = 0x00;			/* type=0 (EAP Packet) */

	/* ��չ��֤Э�� */
	response[18] = RESPONSE;
	response[19] = request[19];		/* id */
	response[22] = request[22];		/* EAP type */

	i = 23;

	/* AVAILABLE ���ϱ��Ƿ�ʹ�ô��� */
	if(request[22] == AVAILABLE) {
		response[i++] = 0x00;
	}

	/* �ϴ� IP ��ַ */
	response[i++] = 0x15;
	response[i++] = 0x04;
	memcpy(response + i, ip, 4);

	i += 4;

	/* �ϴ��汾�� */
	response[i++] = 0x06;
	response[i++] = 0x07;
	fillBase64(response + i);

	i += 28;

	/* �û��� */
	response[i++] = 0x20;			/* �����ո� */
	response[i++] = 0x20;
	memcpy(response + i, username, username_len);

	i		= i + username_len;
	length	= htons(i - 18);

	/* ��䳤�� */
	memcpy(response + 16, &length, sizeof length);
	memcpy(response + 20, &length, sizeof length);

	pcap_sendpacket(handle, response, i);
}

/*
	sendResponseMD5
*/
static void sendResponseMD5(pcap_t *handle, uint8_t *request, uint8_t *ethernet, char *username, char *password) {
	uint8_t		response[128];

	uint16_t	length;
	size_t		username_len;
	size_t		packet_len;

	username_len	= strlen(username);
	packet_len		= 14 + 4 + 22 + username_len;		/* ethhdr + EAPOL + EAP + username_len */
	length			= htons(22 + username_len);

	memcpy(response, ethernet, 14);

	/* 802.1x ��֤ */
	response[14] = 0x01;			/* 802.1x version 1 */
	response[15] = 0x00;			/* type=0 (EAP Packet) */
	/* length */
	memset(response + 16, length, sizeof length);

	/* ��չ��֤Э�� */
	response[18] = RESPONSE;
	response[19] = request[19];		/* id */
	response[20] = response[16];	/* length */
	response[21] = response[17];
	response[22] = MD5;				/* EAP type */
	response[23] = 16;				/* MD5 length */

	/* MD5 */
	fillMd5		(response + 24, request[19], password, request + 24);
	memcpy		(response + 40, username, username_len);

	pcap_sendpacket(handle, response, packet_len);
}



/*
	authStart										��������

	@ char		*username
	@ char		*password
	@ char		*device
	@ BYTE		*mac
	@ char		**error								����

	# int											0 = �����Ͽ�����
													1 = �޷����������豸
													2 = ��֤��ʱ����
													3 = ѭ����ʱ
													4 = δ֪����
													5 = �Զ��Ͽ�����
													6 = ����ʧ�� (���� error)
*/
int authStart(char *username, char *password, char *device, BYTE *mac, char **error) {
	char	errbuf	[PCAP_ERRBUF_SIZE];
	pcap_t	*handle;

	char	filter		[100];
	struct	bpf_program	filter_code;

	/* ������ */
	if((handle = pcap_open_live(device, 65536, 1, 500, errbuf)) == NULL) {
		return 1;
	}

	/* ���������� */
	sprintf_s (
		filter, sizeof filter,
		"(ether proto 0x888e) and (ether dst host %02x:%02x:%02x:%02x:%02x:%02x)",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
	);

	pcap_compile	(handle, &filter_code, filter, 1, 0xFF);
	pcap_setfilter	(handle, &filter_code);

	connect_handle	= handle;
	connect_mac		= mac;

	/* ��֤ */
	{
		int		retry;
		int		connected	= 0;

		uint8_t	ethernet[14]		= {0};
		uint8_t	*captured			= NULL;

		struct	pcap_pkthdr *header	= NULL;

		/* ����������֤�Ự */
		sendStartPackage(handle, mac);

		/* ��ʼ�����Լ����� */
		retry = 0;

		/* �ȴ���֤�������Ļ�Ӧ */
		while(1) {
			int code;

			code = pcap_next_ex(handle, &header, &captured);

			/* ��֤�ɹ� */
			if((code == 1) && (captured[18] == REQUEST)) {
				break;
			}

			/* ���Դ��� */
			if((retry++) >= RETRY_CONNECT) {
				pcap_close(handle);

				return 2;
			}

			Sleep(10);
			sendStartPackage(handle, mac);
		}

		/* ��дӦ��ͷ */
		memcpy(ethernet +  0, captured + 6,	6);
		memcpy(ethernet +  6, mac,			6);
		memset(ethernet + 12, 0x88,			1);
		memset(ethernet + 13, 0x8E,			1);

		/* ��Ӧ��֤ */
		switch(captured[22]) {
			case NOTIFICATION:
				sendResponseNotification(handle, captured, ethernet);
				/* ������һ���� */
				pcap_next_ex(handle, &header, &captured);

				break;

			case IDENTITY:
			case AVAILABLE:
				{
					BYTE ip[4];

					/* ��һ�ν��յ� AVAILABLE ���ظ� IDENTITY */
					captured[22] = IDENTITY;

					getIpFromDevice			(ip, device);
					sendResponseIdentity	(handle, captured, ethernet, ip, username);
				}
				break;
		}

		/* ��������ɸѡ�� */
		sprintf_s (
			filter, sizeof filter,
			"(ether proto 0x888e) and (ether src host %02x:%02x:%02x:%02x:%02x:%02x)",
			captured[6], captured[7], captured[8], captured[9], captured[10], captured[11]
		);

		pcap_compile	(handle, &filter_code, filter, 1, 0xFF);
		pcap_setfilter	(handle, &filter_code);

		/* Ӧ��ѭ�� */
		while(1) {
			int		max;
			DWORD	wait;

			/* ��ʼ�����Լ����� */
			retry = 0;

			/* ���Դ����͵ȴ�ʱ�� */
			if(connected) {
				max		= RETRY_LOOP;
				wait	= 1000;
			} else {
				max		= RETRY_CONNECT;
				wait	= 10;
			}

			/* �ȴ����ݰ� */
			while(1) {
				if(pcap_next_ex(handle, &header, &captured) == 1) {
					break;
				}

				/* ��ʱ */
				if((retry++) >= max) {
					pcap_close(handle);

					return 3;
				}

				Sleep(wait);
			}

			/* ������Ӧ */
			switch(captured[18]) {
				/* Ӧ������ */
				case REQUEST:
					switch(captured[22]) {
						case IDENTITY:
						case AVAILABLE:
							{
								BYTE ip[4];

								getIpFromDevice			(ip, device);
								sendResponseIdentity	(handle, captured, ethernet, ip, username);
							}
							break;

						case MD5:
							sendResponseMD5				(handle, captured, ethernet, username, password);
							break;

						case NOTIFICATION:
							sendResponseNotification	(handle, captured, ethernet);
							break;

						default:
							pcap_close(handle);

							return 4;
					}
					break;

				/* ��֤�ɹ� */
				case SUCCESS:
					connected = 1;

					/* ˢ�� DHCP IP */
					callRefresh();

					break;

				/* ��֤ʧ�� */
				case FAILURE:
					*error = NULL;

					{
						uint8_t type = captured[22];
						uint8_t size = captured[23];

						/* ������Ϣ */
						if((type == 0x09) && (size > 0)) {
							MEMORY_ALLOC	(*error, char, size + 2);
							memcpy			(*error, (void *) &captured[24], size);
						}
					
						pcap_close(handle);

						/* �������Զ��Ͽ����� */
						if(type == 0x08) {
							return 5;
						}
					}

					return 6;

				/* �Զ������ݰ� */
				default:
					break;
			}
		}
	}

	return 0;
}

/*
	authStop										�Ͽ�����
*/
void authStop() {
	/* ���� 2 �� */
	sendLogoffPackage(connect_handle, connect_mac);
	sendLogoffPackage(connect_handle, connect_mac);
}
