/*
	$ pow.c, 0.2.1221
*/

#include "base.h"


/* ʹ�� ALU ���г˷����� */
size_t powWithALU(bigint_t data, int_t ratio, int_t exp) {
	__asm {
			mov		edi, data

					; ���ó���
			mov		ebx, 43046721

					; д�볤��
			mov		DWORD PTR [esp-4], 1
					; д��ϵ��
			mov		eax, ratio
			mov		[edi], eax

					; ����ָ���Ƿ�Ϊ 0
			cmp		exp, 0
			je		loopDone

		loopStart:
					; ��ս�λ��Ϣ
			xor		esi, esi

					; ���˴���, ����ǰ��Ŀ
			mov		ecx, [esp-4]

		loopMul:
					; ��ȡ��ǰҪ����ı�����
			mov		eax, [edi]

					; EAX = EAX * 3
			mul		ebx
			add		eax, esi
			
					; ������λ
			jnc		loopSave
			inc		edx

		loopSave:
					; �����λ
			mov		esi, edx
			mov		[edi], eax

			add		edi, 8

					; �ж��Ƿ�ѭ������
			loop	loopMul

					; �жϸ�λ���
			test	edx, edx
			jz		loopContinue

					; ������Ŀ
			mov		[edi], edx

			inc		DWORD PTR [esp-4]

		loopContinue:
					; ��ָ��
			mov		ecx, exp
			dec		ecx

			jz		loopDone

					; ������ָ��
			mov		exp, ecx
					; �ָ� edi
			mov		edi, data

			jmp		loopStart

		loopDone:
			mov		eax, [esp-4]
	}
}


/* ʹ�� SSE2 ���г˷����� */
size_t powWithSSE(bigint_t data, int_t ratio, int_t exp) {
	__asm {
			mov		edi, data

					; д��ϵ��
			mov		eax, ratio
			mov		[edi], eax

					; д�볤��
			mov		DWORD PTR [esp-4], 1

					; �ж�ָ���Ƿ�Ϊ 0
			cmp		exp, 0
			je		loopDone

					; ���ó���
			mov		eax, 43046721
			movd	xmm1, eax
			movlhps	xmm1, xmm1

		loopStart:
					; �� 1 λ��λ��Ϣ
			xor		edx, edx

					; ���˴���, ����ǰ��Ŀ
			mov		ecx, [esp-4]

		loopMul:
					; �� 1 λ������λ��Ϣ
			xor		ebx, ebx

					; ��ȡ������
			movapd	xmm0, [edi]

					; EAX = EAX * N
			pmuludq	xmm0, xmm1

					; 0~31 = 0:[0~31]
			movd	eax, xmm0
			add		eax, edx
					; 31~63 = 0:[31~63]
			psrldq	xmm0, 4
			movd	edx, xmm0

					; ����� 32 λ
			mov		[edi], eax

					; �жϼ���ǰһ�εĸ� 32 λ�Ľ�λ���Ƿ��ٴν�λ
			jnc		loopHigh
			inc		ebx

		loopHigh:
					; �ճ� 1 ���յ� DW
			add		edi, 8
			psrldq	xmm0, 4

					; 64~95 = 1:[0~31]
			movd	eax, xmm0
			add		eax, edx
					; 96~127 = 1:[32~64]
			psrldq	xmm0, 4
			movd	edx, xmm0

					; �жϼ��ϵ� 32 λ�Ľ�λ���Ƿ��ٴν�λ
			jnc		loopCarry

			inc		edx

		loopCarry:
					; ���ϵ� 32 λ�ĸ�����λ
			add		eax, ebx
			jnc		loopSave

			inc		edx

		loopSave:
					; ����� 32 λ
			mov		[edi], eax

			add		edi, 8

					; ѭ������
			sub		ecx, 2
			jc		loopNextOdd
			jnz		loopMul
			jmp		loopNextCarry

		loopNextOdd:
			test	eax, eax
			jz		loopNextCarry

			inc		DWORD PTR [esp-4]

		loopNextCarry:
			test	edx, edx
			jz		loopContinue

			mov		[edi], edx

			inc		DWORD PTR [esp-4]

		loopContinue:
					; ��ָ��
			mov		ecx, exp
			dec		ecx

			jz		loopDone

					; ������ָ��
			mov		exp, ecx
					; �ָ� edi
			mov		edi, data

			jmp		loopStart

		loopDone:
					; ���س���
			mov		eax, [esp-4]
	}
}
