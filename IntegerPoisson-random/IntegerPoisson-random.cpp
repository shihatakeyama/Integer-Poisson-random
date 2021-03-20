// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// IntegerPoisson-random.cpp 
//
//  浮動小数を使用しせず32bit整数型を使用して疑似乱数生成プログラム 
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
//  ランダムが正しく生成された事を確かめる為の分布計算 
// ---------------------------------------------------------------------

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ポアソン分布のリファレンス
// 引用 : http://www1.cts.ne.jp/~clab/hsample/Math/Math5.html
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/* 階乗を計算する */
double Fact(double n)
{
	return((n == 0.0) ? 1 : n * Fact(n - 1.0));
}
/* a分間にn回起きる確率（ポアッソン分布）を計算する */
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
// unsigned int 32bitを可能な限り使用する場合
// ---------------------------------------------------------------------

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 倍率をかけて 0x00100000 を1.0とします。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui20Mult(uint32_t Arg1, uint32_t Arg2)
{
	// 10bitづつ高位、下位に分けて計算。
	uint32_t ans = (Arg1 * (Arg2 >> 10)) + ((Arg1 * (Arg2 & 0x00003FF)) >> 10);

	return ans >> 10;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ポアソン分布に従う乱数
// ネイピア数を一度にべき乗すると桁あふれするので、
// ループしながらprが0x00100000を下回ったらネイピア数をかけ合わせる。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui20porand(uint32_t Lambda)
{
	const uint32_t nep = 2850325;	//	e × 0x00100000
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
			ra = (rand() * rand()) & 0x000FFFFF;	// ランダム 使用すする環境に応じて書き換えて下さい。
			pr = ui20Mult(pr, ra);
			k++;
		}
	}
}


// ---------------------------------------------------------------------
// rand()が15bitで返す環境の場合の小規模版
// ---------------------------------------------------------------------

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 倍率をかけて 0x00008000 を1.0とします。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui15Mult(uint32_t Arg1, uint32_t Arg2)
{
	uint32_t ans = (Arg1 * (Arg2 >> 15)) + ((Arg1 * (Arg2 & 0x00007FFF)) >> 15);

	return ans ;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ポアソン分布に従う乱数
// ネイピア数を一度にべき乗すると桁あふれするので、
// ループしながらprが0x00008000を下回ったらネイピア数をかけ合わせる。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t ui15porand(uint32_t Lambda)
{
	const uint32_t nep = 89073;	//	e × 0x00008000
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
			ra = rand() & 0x00007FFF;	// ランダム 使用すする環境に応じて書き換えて下さい。
			pr = ui15Mult(pr, ra);
			k++;
		}
	}
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ヒストグラムを作成してランダム生成が正しい事を確かめる。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t uiporandTest()
{
	int32_t	i;
	uint32_t prand;
	uint32_t hist[256];
	int n = 100000;
	int m = 30;
	double a = 10;

	printf("整数20bit演算でランダム生成した分布 \n");

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

	printf("整数15bit演算でランダム生成した分布 \n");

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

	printf("==== ポアソン分布 ====\n");
	srand(2021);

	printf("\n");

	printf("リファレンス\n");
	RandomRef(30, 10);

	printf("\n");

	// ランダム生成
	uiporandTest();

	getchar();

	return 0;
}

