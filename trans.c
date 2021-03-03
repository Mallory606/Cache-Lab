/*
 * Ashley Krattiger - 101797698
 * 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose32x32
 * 
 * Helper that transposes matrix of size 32x32
 *
 * Variables:
 * i - index for the N value in the inner loop
 * ii - index for the N value in the outer loop
 * j - index for the M value in the inner loop
 * jj - index for the M value in the outer loop
 * temp - temporary variable that holds the values from A
 * enI - value representing size of blocks for N loops
 * enJ - value representing size of blocks for M loops
 */
void transpose32x32(int A[32][32], int B[32][32])
{
	int i, ii, j, jj, temp;
	int enI = 8*(32/8);
	int enJ = 4*(32/4);
	for(ii = 0; ii < enI; ii += 8)
	{
		for(jj = 0; jj < enJ; jj += 4)
		{
			for(i = ii; i < ii+8; i++)
			{
				for(j = jj; j < jj+4; j++)
				{
					temp = A[i][j];
					B[j][i] = temp;
				}
			}
		}
	}
}

/*
 * transpose64x64
 *
 * Helper that transposes matrix of size 64x64
 *
 * Variables:
 * i - index for the N value in the inner loop
 * ii - index for the N value in the outer loop
 * j - index for the M value in the inner loop
 * jj - index for the M value in the outer loop
 * temp - temporary variable that holds the values from A
 * enI - value representing size of blocks for N loops
 * enJ - value representing size of blocks for M loops
 */
void transpose64x64(int A[64][64], int B[64][64])
{
	int i, ii, j, jj, temp;
	int enI = 8*(64/8);
	int enJ = 4*(64/4);
	for(ii = 0; ii < enI; ii += 8)
	{
		for(jj = 0; jj < enJ; jj += 4)
		{
			for(i = ii; i < ii+8; i++)
			{
				for(j = jj; j < jj+4; j++)
				{
					temp = A[i][j];
					B[j][i] = temp;
				}
			}
		}
	}
}

/*
 * transpose61x67
 *
 * Helper that transposes matrix of size 61x67
 *
 * Variables:
 * i - index for the N value in the inner loop
 * ii - index for the N value in the outer loop
 * j - index for the M value in the inner loop
 * jj - index for the M value in the outer loop
 * temp - temporary variable that holds the values from A
 * enI - value representing size of blocks for N loops
 * enJ - value representing size of blocks for M loops
 */
void transpose61x67(int A[67][61], int B[61][67])
{
	int i, ii, j, jj, temp;
	int enI = 16*(67/16);
	int enJ = 2*(61/2);
	for(ii = 0; ii < enI; ii += 16)
	{
		for(jj = 0; jj < enJ; jj += 2)
		{
			for(i = ii; i < ii+16; i++)
			{
				for(j = jj; j < jj+2; j++)
				{
					temp = A[i][j];
					B[j][i] = temp;
				}
			}
		}
	}
	for(j = 64; j < 67; j++)
	{
		for(i = 0; i < 61; i++)
		{
			temp = A[j][i];
			B[i][j] = temp;
		}
	}
	for(j = 0; j < 67; j++)
	{
		temp = A[j][60];
		B[60][j] = temp;
	}
}

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
	if(M == 32 && N == 32){ transpose32x32(A, B); }
	else if(M == 64 && N == 64){ transpose64x64(A, B); }
	else{ transpose61x67(A, B); }
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
    registerTransFunction(trans, trans_desc); 

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

