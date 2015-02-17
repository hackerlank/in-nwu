/*
	$ MF Client   (C) 2005-2013 MF
	$ call.c
*/

#include "base.h"

#include <iphlpapi.h>


/* ������Դ��� */
#define MAX_RETRY				1

/* ����ʱ������ */
#define ERROR_TIME				63027


/* �������� */
static int		connected	= 0;
/* ���Դ��� */
static int		retried		= 0;
/* ���� */
static int		suspend		= 0;

/* �����߳� */
static HANDLE	thread		= NULL;


/*
	errid
*/
static int errid(char *message) {
	int result;

	if(message[0] == 'E') {
		if(sscanf_s(message + 1, "%u", &result) == 1) {
			return result;
		}
	}

	return 0;
}


/*
	threadCall
*/
static DWORD WINAPI threadCall(LPVOID dummy) {
	BYTE	*mac;

	int		basic,			index;
	char	*username,		*password,		*device;
	DWORD	username_len,	password_len;
	
	if(CHECK_GET(hidden)) {
		basic = 1;
	} else {
		basic = 0;
	}

	if((username_len = GetWindowTextLengthA(handle_object_username)) == 0) {
		WARNING("����������������Ҫ���û�����");
	}
	if((password_len = GetWindowTextLengthA(handle_object_password)) == 0) {
		WARNING("����������������Ҫ�����롣");
	}

	if(username_len > 20) {
		WARNING("������û���̫�������������롣");
	}
	if(password_len > 20) {
		WARNING("���������̫�������������롣");
	}

	if((index = SendMessage(handle_object_adapter, CB_GETCURSEL, 0, 0)) == CB_ERR) {
		WARNING("��ѡ��һ�������豸�Խ������硣");
	}
	if((device = deviceSelect(basic, index, &mac)) == NULL) {
		WARNING("��ѡ��һ����ȷ�������豸��");
	}
	
	MEMORY_ALLOC	(username, char, username_len + 2);
	MEMORY_ALLOC	(password, char, password_len + 2);
	
	GetWindowTextA	(handle_object_username, username, username_len + 1);
	GetWindowTextA	(handle_object_password, password, password_len + 1);

	configSetUsername	(username);
	configSetPassword	(password);
	configSetAdapter	(mac);
	
	if(CHECK_GET(hidden)) {
		configSetHidden	(TRUE);
	} else {
		configSetHidden	(FALSE);
	}
	if(CHECK_GET(login)) {
		configSetLogin	(TRUE);
	} else {
		configSetLogin	(FALSE);
	}

	if(CHECK_GET(remember)) {
		configWrite();
	} else {
		configClear();
	}

	guiHide();

	/* ������ʾ */
	iconCreate();
	iconTip(TEXT("���Ժ�"), TEXT("����Ŭ�������������硣"), 0);

	/* ������ */
	{
		int		status;
		char	*error;

		/* ������������״̬ */
		connected = 2;

		while(1) {
			status = authStart(username, password, device, mac, &error);

			/* �����Ͽ� */
			if(status == 0) {
				connected = 0;

				iconTip(TEXT("���ӶϿ�"), TEXT("���������Ѿ��Ͽ���"), 1);

				return 0;
			}

			/* ���ִ��� */
			if(status == 6) {
				if(error) {
					/* ����ʱ������ */
					if(errid(error) == ERROR_TIME) {
						if(connected == 1) {
							iconTip(TEXT("����Ͽ�"), TEXT("�����ܵ�����ʱ�ε����ƣ������Ѿ��Ͽ����ӡ�"), 1);
						} else {
							iconTip(TEXT("����ʧ��"), TEXT("�����ܵ�����ʱ�ε����ƣ������������硣"), 2);
						}

						MEMORY_FREE(error);

						connected = 0;

						return 0;
					}

					guiShow(error);

					MEMORY_FREE(error);
				} else {
					guiShow("���ӳ���δ֪�Ĵ���");
				}

				break;
			}

			if((retried++) >= MAX_RETRY) {
				switch(status) {
					case 1:
						guiShow("�޷���ʼ�������豸��");
						break;

					case 2:
					case 3:
						guiShow("���ӷ�������ʱ��");
						break;

					case 4:
						guiShow("�޷��������������ص����ݡ�");
						break;

					case 5:
						guiShow("�������Զ��Ͽ����ӡ�");
						break;
				}

				break;
			}

			Sleep(1000);
		}
	}

	connected = 0;

	iconDestroy();

	return 0;
}


/*
	refresh
*/
static int refresh(int allocate) {
	LONG	i;
	ULONG	size = 0;

	PIP_INTERFACE_INFO	table = NULL;

	if(GetInterfaceInfo(NULL, &size) != ERROR_INSUFFICIENT_BUFFER) {
		return 0;
	}

	MEMORY_ALLOCS(table, IP_INTERFACE_INFO, size);

	if(GetInterfaceInfo(table, &size) == NO_ERROR) {
		for(i = 0; i < table->NumAdapters; i++) {
			/* �ҵ��豸 */
			if(deviceCompare(table->Adapter[i].Name)) {
				goto execute;
			}
		}
	}

	MEMORY_FREE(table);

	return 0;

execute:
	{
		DWORD result;

		if(allocate) {
			result = IpRenewAddress		(&table->Adapter[i]);
		} else {
			result = IpReleaseAddress	(&table->Adapter[i]);
		}
		
		MEMORY_FREE(table);

		if(result == NO_ERROR) {
			return 1;
		} else {
			return 0;
		}
	}
}


/*
	disconnect
*/
static void disconnect() {
	/* ��ֹ�����߳� */
	TerminateThread(thread, 0);

	/* �ȴ��߳���ֹ */
	if(connected == 1) {
		WaitForSingleObject(thread, INFINITE);
	} else {
		WaitForSingleObject(thread, 2000);
	}

	/* �ͷ��̶߳��� */
	CloseHandle(thread);

	/* ���ͶϿ����ݰ� */
	if(connected) {
		authStop();
		Sleep(500);
	}

	/* �ͷ� IP */
	refresh(0);

	connected = 0;
}


/*
	callStart										ִ�п�ʼ
*/
void callStart() {
	thread = CreateThread(NULL, 0, threadCall, NULL, 0, NULL);
}

/*
	callRefresh										��֤�ɹ���Ҫˢ�� IP
*/
void callRefresh() {
	if(refresh(1) == 0) {
		iconTip(TEXT("����ʧ��"), TEXT("�����Ѿ�ͨ���˵�¼��Ϣ����֤�����޷��ӷ����������뵽 IP ��ַ����������ڷ������ϵ� IP �ѷ�����ϡ�\r\n\r\n�Ժ����ֶ�ʹ�� ipconfig /renew ������������ӡ�"), 1);
	} else {
		if(suspend) {
			iconTip(TEXT("���ӻָ�"), TEXT("������������л��Ѻ������Ѿ��Զ��ָ����ӡ�"), 0);
		} else {
			if(connected == 1) {
				iconTip(TEXT("���ӻָ�"), TEXT("�����Ѿ��Զ��ָ����ӡ�"), 0);
			} else {
				iconTip(TEXT("���ӳɹ�"), TEXT("�Ѿ��ɹ������������硣"), 0);
			}
		}
	}

	connected	= 1;
	retried		= 0;
	suspend		= 0;
}

/*
	callDisconnect									��ֹ����
*/
void callDisconnect() {
	disconnect();

	iconDestroy();
	guiShow("");
}

/*
	callClose										��ֹ�����˳�
*/
void callClose() {
	disconnect();

	iconDestroy();
	guiHide();

	/* �˳� */
	SendMessage(handle_window, WM_DESTROY, 0, 0);
}


/*
	callStatus										��ǰ����״̬

	# int											0=δ����
													1=������
*/
int callStatus() {
	return connected;
}


/*
	callPower										��Դ��������

	@ WPARAM		action
*/
void callPower(WPARAM action) {
	switch(action) {
		/* ���� */
		case PBT_APMSUSPEND:
			suspend = 1;

			/* �Ͽ����� */
			if(connected) {
				disconnect();
			}

			break;

		/* ���� */
		case PBT_APMRESUMEAUTOMATIC:
			disconnect();
			callStart();

			break;
	}
}
