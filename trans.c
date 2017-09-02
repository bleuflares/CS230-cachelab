/* 
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
#include <math.h>
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
int i, j, n, m;
int a0, a1, a2, a3, a4, a5, a6, a7;
	if(N==32)	
	{
		for(m=0; m<M/8; m++)
		{
			for(n=0; n<N/8; n++)
			{
				for(i=0; i<8; i++)
				{
					for(j=0; j<8; j++)
					{
						if(i!=j||m!=n)
							B[m*8+j][n*8+i]=A[n*8+i][m*8+j];/*diagonal entries set aside to incerase cache hit*/
					
						else
						{
							a1=A[n*8+i][m*8+j];
						}
					}
					if(m==n)
						B[n*8+i][n*8+i]=a1; /*elements at the same row succeeding; more cache hits!*/
				}
			}
		}
	}			
	else if(N==64)
	{
		for(n=0;n<8;n++)
		{
			for(m=0;m<8;m++)
			{
					for(i=0;i<8;i++)
					{
						a0=A[8*m+i][8*n];
						a1=A[8*m+i][8*n+1];
						a2=A[8*m+i][8*n+2];
						a3=A[8*m+i][8*n+3];
						if(i==0)	/*set aside for later use to increase cache hit*/
						{
							a4=A[8*m+i][8*n+4];
							a5=A[8*m+i][8*n+5];
							a6=A[8*m+i][8*n+6];
							a7=A[8*m+i][8*n+7];
						}
						B[8*n][8*m+i]=a0;	
						B[8*n][8*m+i+64]=a1;
						B[8*n][8*m+i+128]=a2;
						B[8*n][8*m+i+192]=a3;
					}
					for(j=7; j>0;j--)
					{
						a0=A[8*m+j][8*n+4];
						a1=A[8*m+j][8*n+5];
						a2=A[8*m+j][8*n+6];
						a3=A[8*m+j][8*n+7];
						B[8*n+4][8*m+j]=a0;
						B[8*n+4][8*m+j+64]=a1;
						B[8*n+4][8*m+j+128]=a2;
						B[8*n+4][8*m+j+192]=a3;
					}
					/*elements at the same row succeeding; more cache hits!*/
					B[8*n+4][8*m]=a4;
					B[8*n+4][8*m+64]=a5;
					B[8*n+4][8*m+128]=a6;
					B[8*n+4][8*m+192]=a7;
				}
			
		}
	}
	else
	{
		for(m=0;m<8; m++)
		{
			for(n=0;n<9; n++)
			{
				for(j=0;j<8&&j<(N-8*n); j++)
				{
					for(i=0;i<8&&i<(M-8*m);i++)
					{
						B[8*m+i][8*n+j]=A[8*n+j][8*m+i];
					}
				}
			}
		}
	}
}
void transpose_test1(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, n, m;
	int row, col, temp, block, block2;
	int r, s;
	if (N == 32)
	{
		for (m = 0; m<M / 8; m++)
		{
			for (n = 0; n<N / 8; n++)
			{
				for (i = 0; i<8; i++)
				{
					for (j = 0; j<8; j++)
					{
						if (i != j || m != n)
							B[m * 8 + j][n * 8 + i] = A[n * 8 + i][m * 8 + j];

						else
						{
							temp = A[n * 8 + i][m * 8 + j];
						}
					}
					if (m == n)
						B[n * 8 + i][n * 8 + i] = temp;
				}
			}
		}
	}
	else if (N == 64)/*double blocking*/
	{
		block=32;
		block2 = 8;
		for (m = 0; m < M / block; m++)
		{
			for (n = 0; n < N / block; n++)
			{
				row = m*block;
				col = n*block;
				for (r = 0; r < block2; r++)
				{
					for (s = 0; s < block2; s++)
					{
						for (i = 0; i < block / block2; i++)
						{
							for (j = 0; j < block / block2; j++)
							{
								if (i != j || m != n)
								{
									B[col + j + s*block / block2][row + i + r*block / block2] = A[row + i + r*block / block2][col + j + s*block / block2];
								}
								else
								{
									temp = A[row + i + r*block / block2][col + j + s*block / block2];
								}
							}
							if (m == n)
								B[col + i + s*block / block2][row + i + r*block / block2] = temp;
						}
					}
				}
			}
		}
	}
	
	else
	{
		for (m = 0; m<8; m++)
		{
			for (n = 0; n<9; n++)
			{
				for (j = 0; j<8 && j<(N - 8 * n); j++)
				{
					for (i = 0; i<8 && i<(M - 8 * m); i++)
					{
						B[8 * m + i][8 * n + j] = A[8 * n + j][8 * m + i];
					}
				}
			}
		}
	}
}
char transpose_test1_desc[] = "test1";
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
	registerTransFunction(transpose_test1, transpose_test1_desc);
    /* Register any additional transpose functions */

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

