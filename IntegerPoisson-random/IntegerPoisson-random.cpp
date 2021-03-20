// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// IntegerPoisson-random.cpp 
//
//  浮動小数を使用しせず32bit整数型を使用た疑似乱数生成プログラム 
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


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// 倍率をかけて 0x00100000 を1.0とします。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t multFloat20bit(uint32_t Arg1, uint32_t Arg2)
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
uint32_t uiporand(uint32_t Lambda)
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

			pr = multFloat20bit(pr, nep);
		}
		else{
			ra = (rand() * rand()) & 0x000FFFFF;	// ランダム 使用すする環境に応じて書き換えて下さい。
			pr = multFloat20bit(pr, ra);
			k++;
		}
	}
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// ヒストグラムを作成してランダム生成が正しい事を確かめる。
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
uint32_t uiporandTest(int n, int m, double a)
{
	int32_t	i;
	uint32_t prand;
	uint32_t hist[256];

	memset(hist, 0, sizeof(hist));

	for (i = 0; i<=m; i++){
		prand = uiporand(10);
		if (prand < (sizeof(hist) / sizeof(uint32_t))){
			hist[prand]++;
		}
	}

	for (i = 0;i<n; i++){
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

	printf("整数演算でランダム生成したヒストグラム \n");
	uiporandTest(30, 100000, 10);


	getchar();

	return 0;
}

