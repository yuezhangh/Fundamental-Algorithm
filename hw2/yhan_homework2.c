#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double A[600000][4],  b[600000], c[4] ;
double result[4];

int calcresult(double arr[4][5], double *result)
{
	double arr1[4][5];
	int i = 0,j = 0,k = 0;
	int a = 0,b = 0;
	double x4,x3,x2,x1;
	double m;
	i = 0,j = 0;
	for(j = 0;j < 5;j++)
	{
		if(arr[i][k] == 0)
			arr1[i][j] = 0;
		else 
			arr1[i][j] = arr[i][j] * (arr[i+1][k] / arr[i][k]);
	}
	i = 0,j = 0;
	for(j = 0;j < 5;j++)
	{
		arr[i+1][j] = arr[i+1][j] - arr1[i][j];
	}
	i = 0,j = 0;
	for(j = 0;j < 5;j++)
	{
		if(arr[i][k] == 0)
			arr1[i][j] = 0;
		else 
			arr1[i][j] = arr[i][j] * (arr[i+2][k] / arr[i][k]);
	}
	i = 0,j = 0;
	for(j = 0;j < 5;j++)
	{
		arr[i+2][j] = arr[i+2][j] - arr1[i][j];
	}
	i = 0,j = 0;
	for(j = 0;j < 5;j++)
	{
		if(arr[i][k] == 0)
			arr1[i][j] = 0;
		else 
			arr1[i][j] = arr[i][j] * (arr[i+3][k] / arr[i][k]);
	}
	i = 0;j = 0;
	for(j=0;j<5;j++)
	{
		arr[i+3][j] = arr[i+3][j] - arr1[i][j];
	}
	i = 1;j = 1;k = 1;
	for(a = 1;a < 5;a++)
	{
		if(arr[i][k] == 0)
			arr1[i][a] = 0;
		else 
			arr1[i][a] = arr[i][a] * (arr[i+1][k] / arr[i][k]);
	}
	i = 1;j = 1;k = 1;
	for(b = 1;b < 5;b++)
	{
		arr[i+1][b] = arr[i+1][b] - arr1[i][b];
	}
	i = 1;j = 1;k = 1;
	for(a = 1;a < 5;a++)
	{
		if(arr[i][k] == 0)
			arr1[i][a] = 0;
		else 
			arr1[i][a] = arr[i][a] * (arr[i+2][k] / arr[i][k]);
	}
	i = 1;j = 1;k = 1;
	for(b = 1;b < 5;b++)
	{
		arr[i+2][b] = arr[i+2][b] - arr1[i][b];
	}
	i = 2,j = 2,k = 2;
	for(a = 2;a < 5;a++)
	{
		if(arr[i][k] == 0)
			arr1[i][a] = 0;
		else 
			arr1[i][a] = arr[i][a] * (arr[i+1][k] / arr[i][k]);
	}
	i = 2,j = 2,k = 2;
	for(b = 2;b < 5;b++)
	{
		arr[i+1][b] = arr[i+1][b] - arr1[i][b];
	}
	k = 0;
	for(i = 0;i < 4;i++)
	{
		for(j = 0;j < 4;j++)
		{
			if(arr[i][j] != 0)
			{
				k++;
				break;
			}
		}
	}
	if(k < 4)
	{
		return 0;
	}
	else
	{
		x4 = arr[3][4] / arr[3][3];
		x3 = (arr[2][4] - x4 * arr[2][3]) / arr[2][2];
		x2 = (arr[1][4] - arr[1][3] * x4 - arr[1][2] * x3) / arr[1][1];
		x1 = (arr[0][4] - x4 * arr[0][3] - x3 * arr[0][2] - x2 * arr[0][1]) / arr[0][0];
		result[0] = x1;
		result[1] = x2;
		result[2] = x3;
		result[3] = x4;
		return 1;
	}
}

int update(double *equa, double *result)
{
	double m;
	m = equa[0] * result[0] + equa[1] * result[1] + equa[2] * result[2] + equa[3] * result[3];
	if(m <= equa[4])
		return 1;
	else
		return 0;
}

int rand_lp(int n, double a[600000][4], double b[], double c[], double result[])
{
	int num,sign1,sign2,i,j;
	double opt,left;
	int ineq1,ineq2,ineq3,ineq4;
	double im_a[4][5],im_b[5];
	num = 0;
	for(i = 0; i < 4; i++)
	{
		result[i] = 10000;
	}
	opt = c[0] * result[0] + c[1] * result[1] + c[2] * result[2] + c[3] * result[3];
	ineq1 = 0; ineq2 = 1; ineq3 = 2; ineq4 = 3;
	for(i = 4; i < n; i++)
	{
		left = a[i][0] * result[0] + a[i][1] * result[1] + a[i][2] * result[2] + a[i][3] * result[3];
		if(left > b[i])
		{
			for(j = 0; j < 4; j++)
			{
                switch(j){
					case 0:
					{
						im_a[0][0] = a[ineq2][0]; im_a[0][1] = a[ineq2][1]; im_a[0][2] = a[ineq2][2]; im_a[0][3] = a[ineq2][3]; im_a[0][4] = b[ineq2];
						im_a[1][0] = a[ineq3][0]; im_a[1][1] = a[ineq3][1]; im_a[1][2] = a[ineq3][2]; im_a[1][3] = a[ineq3][3]; im_a[1][4] = b[ineq3];
						im_a[2][0] = a[ineq4][0]; im_a[2][1] = a[ineq4][1]; im_a[2][2] = a[ineq4][2]; im_a[2][3] = a[ineq4][3]; im_a[2][4] = b[ineq4];
						im_a[3][0] = a[i][0]; im_a[3][1] = a[i][1]; im_a[3][2] = a[i][2]; im_a[3][3] = a[i][3]; im_a[3][4] = b[i];
						im_b[0] = a[ineq1][0]; im_b[1] = a[ineq1][1]; im_b[2] = a[ineq1][2]; im_b[3] = a[ineq1][3]; im_b[4] = b[ineq1];
						sign1 = calcresult(im_a,result);
						if(sign1 == 1) 
						{
							sign2 = update(im_b,result);
							if(sign2 == 1)
							{
								ineq1 = i; 
								break;
							}
						}
					}
					case 1:
					{
						im_a[0][0] = a[ineq1][0]; im_a[0][1] = a[ineq1][1]; im_a[0][2] = a[ineq1][2]; im_a[0][3] = a[ineq1][3]; im_a[0][4] = b[ineq1];
						im_a[1][0] = a[ineq3][0]; im_a[1][1] = a[ineq3][1]; im_a[1][2] = a[ineq3][2]; im_a[1][3] = a[ineq3][3]; im_a[1][4] = b[ineq3];
						im_a[2][0] = a[ineq4][0]; im_a[2][1] = a[ineq4][1]; im_a[2][2] = a[ineq4][2]; im_a[2][3] = a[ineq4][3]; im_a[2][4] = b[ineq4];
						im_a[3][0] = a[i][0]; im_a[3][1] = a[i][1]; im_a[3][2] = a[i][2]; im_a[3][3] = a[i][3]; im_a[3][4] = b[i];
						im_b[0] = a[ineq2][0]; im_b[1] = a[ineq2][1]; im_b[2] = a[ineq2][2]; im_b[3] = a[ineq2][3]; im_b[4] = b[ineq2];
						sign1 = calcresult(im_a,result);
						if(sign1 == 1) 
						{
							sign2 = update(im_b,result);
							if(sign2 == 1)
							{
								ineq2 = i; 
								break;
							}
						}
					}
					case 2:
					{
						im_a[0][0] = a[ineq1][0]; im_a[0][1] = a[ineq1][1]; im_a[0][2] = a[ineq1][2]; im_a[0][3] = a[ineq1][3]; im_a[0][4] = b[ineq1];
						im_a[1][0] = a[ineq2][0]; im_a[1][1] = a[ineq2][1]; im_a[1][2] = a[ineq2][2]; im_a[1][3] = a[ineq2][3]; im_a[1][4] = b[ineq2];
						im_a[2][0] = a[ineq4][0]; im_a[2][1] = a[ineq4][1]; im_a[2][2] = a[ineq4][2]; im_a[2][3] = a[ineq4][3]; im_a[2][4] = b[ineq4];
						im_a[3][0] = a[i][0]; im_a[3][1] = a[i][1]; im_a[3][2] = a[i][2]; im_a[3][3] = a[i][3]; im_a[3][4] = b[i];
						im_b[0] = a[ineq3][0]; im_b[1] = a[ineq3][1]; im_b[2] = a[ineq3][2]; im_b[3] = a[ineq3][3]; im_b[4] = b[ineq3];
						sign1 = calcresult(im_a,result);
						if(sign1 == 1) 
						{
							sign2 = update(im_b,result);
							if(sign2 == 1)
							{
								ineq3 = i; 
								break;
							}
						}
					}
					case 3:
					{
						im_a[0][0] = a[ineq1][0]; im_a[0][1] = a[ineq1][1]; im_a[0][2] = a[ineq1][2]; im_a[0][3] = a[ineq1][3]; im_a[0][4] = b[ineq1];
						im_a[1][0] = a[ineq2][0]; im_a[1][1] = a[ineq2][1]; im_a[1][2] = a[ineq2][2]; im_a[1][3] = a[ineq2][3]; im_a[1][4] = b[ineq2];
						im_a[2][0] = a[ineq3][0]; im_a[2][1] = a[ineq3][1]; im_a[2][2] = a[ineq3][2]; im_a[2][3] = a[ineq3][3]; im_a[2][4] = b[ineq3];
						im_a[3][0] = a[i][0]; im_a[3][1] = a[i][1]; im_a[3][2] = a[i][2]; im_a[3][3] = a[i][3]; im_a[3][4] = b[i];
						im_b[0] = a[ineq4][0]; im_b[1] = a[ineq4][1]; im_b[2] = a[ineq4][2]; im_b[3] = a[ineq4][3]; im_b[4] = b[ineq4];
						sign1 = calcresult(im_a,result);
						if(sign1 == 1) 
						{
							sign2 = update(im_b,result);
							if(sign2 == 1)
							{
								ineq4 = i; 
								break;
							}
                            else
                            {
								ineq4 = i;
								result[0]=100000;
								result[1]=100000;
								result[2]=100000;
								result[3]=100000;
								break;
                            } 
						}
					}
				}
			}
			opt = c[0] * result[0] + c[1] * result[1] + c[2] * result[2] + c[3] * result[3];
			num++;
		}
	}
	return num;
}

int main()
{  
   int i, j; double s, t;
   printf("Preparing test: 4 variables, 600000 inequalities\n");
   A[0][0] = 1.0; A[0][1] = 2.0; A[0][2] = 1.0; A[0][3] = 0.0; b[0] = 10000.0;
   A[1][0] = 0.0; A[1][1] = 1.0; A[1][2] = 2.0; A[1][3] = 1.0; b[1] = 10000.0;
   A[2][0] = 1.0; A[2][1] = 0.0; A[2][2] = 1.0; A[2][3] = 3.0; b[2] = 10000.0;
   A[3][0] = 4.0; A[3][1] = 0.0; A[3][2] = 1.0; A[3][3] = 1.0; b[3] = 10000.0;
   c[0]=1.0; c[1]=1.0; c[2]=1.0; c[3]=1.0;
   for( i=4; i< 100000; i++ )
   {  A[i][0] = (12123*i)%104729; 
      A[i][1] = (47*i)%104729; 
      A[i][2] = (2011*i)%104729; 
      A[i][3] = (7919*i)%104729;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 1 + (i%137);
   }
   A[100000][0] = 0.0; A[100000][1] = 6.0; A[100000][2] = 1.0; 
   A[100000][3] = 1.0; b[100000] = 19.0;
   for( i=100001; i< 200000; i++ )
   {  A[i][0] = (2323*i)%101111; 
      A[i][1] = (74*i)%101111; 
      A[i][2] = (2017*i)%101111; 
      A[i][3] = (7915*i)%101111;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 2 + (i%89);
   }
   A[200000][0] = 5.0; A[200000][1] = 2.0; A[200000][2] = 0.0; 
   A[200000][3] = 1.0; b[200000] = 13.0;
  for( i=200001; i< 300000; i++ )
   {  A[i][0] = (23123*i)%100003; 
      A[i][1] = (47*i)%100003; 
      A[i][2] = (2011*i)%100003; 
      A[i][3] = (7919*i)%100003;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 2 + (i%57);
   }
   A[300000][0] = 1.0; A[300000][1] = 2.0; A[300000][2] = 1.0; 
   A[300000][3] = 3.0; b[300000] = 20.0;
   A[300001][0] = 1.0; A[300001][1] = 0.0; A[300001][2] = 5.0; 
   A[300001][3] = 4.0; b[300001] = 32.0;
   A[300002][0] = 7.0; A[300002][1] = 1.0; A[300002][2] = 1.0; 
   A[300002][3] = 7.0; b[300002] = 40.0;
   for( i=300003; i< 400000; i++ )
   {  A[i][0] = (13*i)%103087; 
      A[i][1] = (99*i)%103087; 
      A[i][2] = (2012*i)%103087; 
      A[i][3] = (666*i)%103087;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 1;
   }
   for( i=400000; i< 500000; i++ )
   {  A[i][0] = 1; 
      A[i][1] = (17*i)%999983; 
      A[i][2] = (1967*i)%444443; 
      A[i][3] = 2;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + (1000000.0/(double)i);
   }
   for( i=500000; i< 600000; i++ )
   {  A[i][0] = (3*i)%111121; 
      A[i][1] = (2*i)%999199; 
      A[i][2] = (2*i)%444443; 
      A[i][3] = i;
      b[i] = A[i][0] + 2*A[i][1] + 3*A[i][2] + 4* A[i][3] + 1.3;
   }
   
   printf("Running test: 600000 inequalities, 4 variables\n");
   j = rand_lp(100000, A, b, c, result);
   printf("Test: extremal point (%lf, %lf, %lf, %lf) after %d recomputation steps\n", 
          result[0], result[1], result[2], result[3], j);
   printf("Answer should be (1,2,3,4)\n End Test\n");
return 0;
}
