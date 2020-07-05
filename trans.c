/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 *StudentNumber：517030910227
 *StudentName: 洪瑄锐
 *
 */

#include <stdio.h>
#include "cachelab.h"


int is_transpose(int M, int N, int A[N][M], int B[M][N]);


/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */

char transpose_submit_desc[] = "Transpose submission";

void transpose_submit(int M, int N, int A[N][M], int B[M][N])

{

	int i, j, k, l;
	int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

	if (N == 32) {
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (i == j) {
					for (k = j * 8; k < j * 8 + 8; k++) {
						for (l = i * 8; l < i * 8 + 8; l++) {
							if (k != l)
								B[l][k] = A[k][l];
							else
								tmp0 = A[k][l];
						}
						B[k][k] = tmp0;
					}
				}
				else {
					for (k = j * 8; k < j * 8 + 8; k++) {
						for (l = i * 8; l < i * 8 + 8; l++) {
							B[l][k] = A[k][l];
						}
					}
				}
			}
		}
	}

	else if (M == 64)
	{
		for (i = 0; i < N; i += 8) {
			for (j = 0; j < M; j += 8) {
				for (k = i; k < i + 4; k++) {
					tmp0 = A[k][j];
					tmp1 = A[k][j + 1];
					tmp2 = A[k][j + 2];
					tmp3 = A[k][j + 3];
					tmp4 = A[k][j + 4];
					tmp5 = A[k][j + 5];
					tmp6 = A[k][j + 6];
					tmp7 = A[k][j + 7];

					B[j][k] = tmp0;  //A左上角直接转置到B左上角
					B[j + 1][k] = tmp1;
					B[j + 2][k] = tmp2;
					B[j + 3][k] = tmp3;

					B[j][k + 4] = tmp4; //将A右上角的转置暂存B右上角
					B[j + 1][k + 4] = tmp5;
					B[j + 2][k + 4] = tmp6;
					B[j + 3][k + 4] = tmp7;
				}
				for (l = j + 4; l < j + 8; l++) {

					tmp0 = A[i + 4][l - 4]; //取A左下角一列
					tmp1 = A[i + 5][l - 4];
					tmp2 = A[i + 6][l - 4];
					tmp3 = A[i + 7][l - 4];

					tmp4 = B[l - 4][i + 4]; //取B右上角一行
					tmp5 = B[l - 4][i + 5];
					tmp6 = B[l - 4][i + 6];
					tmp7 = B[l - 4][i + 7];

					B[l - 4][i + 4] = tmp0; //将A左下角一列赋值给B右上角一行
					B[l - 4][i + 5] = tmp1;
					B[l - 4][i + 6] = tmp2;
					B[l - 4][i + 7] = tmp3;

					B[l][i] = tmp4;         //将B右上角一行赋值给B左下角一列
					B[l][i + 1] = tmp5;
					B[l][i + 2] = tmp6;
					B[l][i + 3] = tmp7;

					B[l][i + 4] = A[i + 4][l]; //A的右下角直接转置为B右下角
					B[l][i + 5] = A[i + 5][l];
					B[l][i + 6] = A[i + 6][l];
					B[l][i + 7] = A[i + 7][l];
				}
			}
		}
	}
	else {
		for (i = 0; i < N / 8 + 1; i++) {
			for (j = 0; j < M / 8 + 1; j++) {
				if (i == j) {
					for (k = j * 8; k < M && k < j * 8 + 8; k++) {
						for (l = i * 8; l < N && l < i * 8 + 8; l++) {
							if (k != l)
								B[k][l] = A[l][k];
							else
								tmp0 = A[l][k];
						}
						B[k][k] = tmp0;
					}
				}
				else {
					for (k = j * 8; k < M && k < j * 8 + 8; k++) {
						for (l = i * 8; l < N && l < i * 8 + 8; l++) {
							B[k][l] = A[l][k];
						}
					}
				}
			}
		}
	}
}



/*

 * You can define additional transpose functions below. We've defined

 * a simple one below to help you get started.

 */



 /*

  * trans - A simple baseline transpose function, not optimized for the cache.

  */

char trans_desc[] = "Simple row-wise scan transpose";

void trans(int M, int N, int A[N][M], int B[M][N])

{

	int i, j, tmp;



	for (i = 0; i < N; i++) {

		for (j = 0; j < M; j++) {

			tmp = A[i][j];

			B[j][i] = tmp;

		}

	}



}



/*

 * registerFunctions - This function registers your transpose

 *     functions with the driver.  At runtime, the driver will

 *     evaluate each of the registered functions and summarize their

 *     performance. This is a handy way to experiment with different

 *     transpose strategies.

 */

void registerFunctions()

{

	/* Register your solution function */

	registerTransFunction(transpose_submit, transpose_submit_desc);



	/* Register any additional transpose functions */

	//registerTransFunction(trans, trans_desc);

}



/*

 * is_transpose - This helper function checks if B is the transpose of

 *     A. You can check the correctness of your transpose by calling

 *     it before returning from the transpose function.

 */

int is_transpose(int M, int N, int A[N][M], int B[M][N])

{

	int i, j;



	for (i = 0; i < N; i++) {

		for (j = 0; j < M; ++j) {

			if (A[i][j] != B[j][i]) {

				return 0;

			}

		}

	}

	return 1;

}