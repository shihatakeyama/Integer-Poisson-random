// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// IntegerPoisson-random.cpp 
//
//  �����������g�p������32bit�����^���g�p���ċ^�����������v���O���� 
//  
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


// ---------------------------------------------------------------------
//  �����_�����������������ꂽ�����m���߂�ׂ̕��z�v�Z 
// ---------------------------------------------------------------------

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �|�A�\�����z�̃��t�@�����X
// ���p : http://www1.cts.ne.jp/~clab/hsample/Math/Math5.html
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/* �K����v�Z���� */
double Fact(double n)
{
	return((n == 0.0) ? 1 : n * Fact(n - 1.0));
}
/* a���Ԃ�n��N����m���i�|�A�b�\�����z�j���v�Z���� */
double CalPois(double a, double n)
{
	return (exp(-a) * pow(a, n) / Fact(n));
}

void RandomRef(int n, double a)
{
	double pois;
	int i;

	for (i = 0.0; i <= n; i++) {
		pois = CalPois(a, i);
		printf("%4d\t%10.8lf\n", i, pois);
	}
}

// ---------------------------------------------------------------------
// unsigned int 32bit���\�Ȍ���g�p����ꍇ
// ---------------------------------------------------------------------

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �{���������� 0x00100000 ��1.0�Ƃ��܂��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui20Mult(uint32_t Arg1, uint32_t Arg2)
{
	// 10bit�Â��ʁA���ʂɕ����Čv�Z�B
	uint32_t ans = (Arg1 * (Arg2 >> 10)) + ((Arg1 * (Arg2 & 0x00003FF)) >> 10);

	return ans >> 10;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �|�A�\�����z�ɏ]������
// �l�C�s�A������x�ɂׂ��悷��ƌ����ӂꂷ��̂ŁA
// ���[�v���Ȃ���pr��0x00100000�����������l�C�s�A�����������킹��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui20porand(uint32_t Lambda)
{
	const uint32_t nep = 2850325;	//	e �~ 0x00100000
	uint32_t pr = nep;
	uint32_t k = -1;
	uint32_t ra;

	while (1){
		if (pr < 0x00100000){
			Lambda--;
			if (Lambda <= 0){
				return k;
			}

			pr = ui20Mult(pr, nep);
		}
		else{
			ra = (rand() * rand()) & 0x000FFFFF;	// �����_�� �g�p��������ɉ����ď��������ĉ������B
			pr = ui20Mult(pr, ra);
			k++;
		}
	}
}


// ---------------------------------------------------------------------
// rand()��15bit�ŕԂ����̏ꍇ�̏��K�͔�
// ---------------------------------------------------------------------

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �{���������� 0x00008000 ��1.0�Ƃ��܂��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui15Mult(uint32_t Arg1, uint32_t Arg2)
{
	uint32_t ans = (Arg1 * (Arg2 >> 15)) + ((Arg1 * (Arg2 & 0x00007FFF)) >> 15);

	return ans ;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �|�A�\�����z�ɏ]������
// �l�C�s�A������x�ɂׂ��悷��ƌ����ӂꂷ��̂ŁA
// ���[�v���Ȃ���pr��0x00008000�����������l�C�s�A�����������킹��B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui15porand(uint32_t Lambda)
{
	const uint32_t nep = 89073;	//	e �~ 0x00008000
	uint32_t pr = nep;
	uint32_t k = -1;
	uint32_t ra;

	while (1){
		if (pr < 0x00008000){
			Lambda--;
			if (Lambda <= 0){
				return k;
			}

			pr = ui15Mult(pr, nep);
		}
		else{
			ra = rand() & 0x00007FFF;	// �����_�� �g�p��������ɉ����ď��������ĉ������B
			pr = ui15Mult(pr, ra);
			k++;
		}
	}
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// �q�X�g�O�������쐬���ă����_�������������������m���߂�B
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t uiporandTest()
{
	int32_t	i;
	uint32_t prand;
	uint32_t hist[256];
	int n = 100000;
	int m = 30;
	double a = 10;

	printf("����20bit���Z�Ń����_�������������z \n");

	memset(hist, 0, sizeof(hist));

	for (i = 0; i<=n; i++){
		prand = ui20porand(10);
		if (prand < (sizeof(hist) / sizeof(uint32_t))){
			hist[prand]++;
		}
	}

	for (i = 0;i<m; i++){
		printf("%4d\t%6d\n", i, hist[i]);
	}

	printf("\n");

	printf("����15bit���Z�Ń����_�������������z \n");

	memset(hist, 0, sizeof(hist));


	for (i = 0; i <= n; i++){
		prand = ui15porand(10);
		if (prand < (sizeof(hist) / sizeof(uint32_t))){
			hist[prand]++;
		}
	}

	for (i = 0; i<m; i++){
		printf("%4d\t%6d\n", i, hist[i]);
	}

	return 0;

}


int _tmain(int argc, _TCHAR* argv[])
{

	printf("==== �|�A�\�����z ====\n");
	srand(2021);

	printf("\n");

	printf("���t�@�����X\n");
	RandomRef(30, 10);

	printf("\n");

	// �����_������
	uiporandTest();

	getchar();

	return 0;
}

