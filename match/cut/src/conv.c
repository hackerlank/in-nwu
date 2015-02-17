/*
	$ conv.c, 0.2.1221
*/

#include "base.h"


/*
	����ת��

	@ FROM:	base on 2 ^ 32
	@ TO:	base on 10 ^ 10
*/
size_t convert(bigint_t data, size_t count) {
	__asm {
					; ����������Դ
			mov		esi, data

					; ���������Դ, ��Ϊ����ƫ�� 32 λ
			mov		edi, data
			add		edi, 4

					; ����, ��������
			mov		ebx, NOTATION

					; �������������Ŀ
			mov		DWORD PTR [esp-4], 0
					; ��������������Ŀ
			mov		ecx, count

			jecxz	convDone
			loop	convHigh

			jmp		convReady

		convHigh:
					; �ƶ� esi ���������ݵ���� DW
			add		esi, 8
			loop	convHigh

		convReady:
					; ���� esi ���ָ� ecx
			mov		[esp-8], esi
			mov		ecx, count

		convStart:
			xor		edx, edx

		convDiv:
					; ������ = ���� 4 λ���� * ÿ�δ�С + ���δ�С
			mov		eax, [esi]
					; EDX:EAX / EBX = EAX ... EDX
			div		ebx

					; ������
			mov		[esi], eax

					; �ƶ� esi ��ǰ 32 λ
			sub		esi, 8

			loop	convDiv

					; ��������, ����������ݵ���Ŀ
			mov		[edi], edx
			add		edi, 8
			inc		DWORD PTR [esp-4]

					; �ж��Ƿ��С�������ݵ���Ŀ
			mov		eax, [esp-8]
			mov		eax, [eax]
			test	eax, eax
			jnz		convNext

					; ���� esi
			sub		DWORD PTR [esp-8], 8

					; ��С��Ŀ
			mov		ecx, count
			dec		ecx
			mov		count, ecx

			jz		convDone

		convNext:
					; �ָ� esi ָ�����λ
			mov		esi, [esp-8]
					; �ָ� ecx
			mov		ecx, count

			jmp		convStart

		convDone:
					; ������Ŀ
			mov		eax, [esp-4]
	}
}
