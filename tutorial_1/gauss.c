/* row wise gaus algorithm
 * pattern for practical course
 * -------------------------
 * autor: Markus Brenk
 * date: 2002-09-25
 * =================================================== */

//#define NGLS 32768
#define NGLS 32800

#define NUM_LOOPS 4

#include <stdio.h>
#include <math.h>
#include "timer.h"


/* print a 3x3 matrix */
void print_matrix(char* name, double matrix[3][3]);


/* print a 3d vector */
void print_vector(char* name, double vec[3]);


/**
 *  initialisation: generates the following LGS:
 *  ( 3  1  1)       (5)                         (1)
 *  ( 1  4  1) * X = (6)        => solution  X = (1)
 *  ( 1  1  5)       (7)                         (1)
 */
void init(double a[3][3], double b[3], double x[3]);

/** performs gauss elimination */
void gauss_elimination(double a[3][3], double b[3], double x[3]);

int lu_decomposition(double a[3][3], double l[3][3], double u[3][3]);

struct SoA{ //SoA
    double x[NGLS];
	double y[NGLS];
	double z[NGLS]; 
	};

int main() {
	// COMPILE WITH
	//icc -std=c99 -O3 -qopt-report=5 -qopt-report-phase=vec gauss.c timer.h timer.c -o gauss
	double a[NGLS][3][3];
	double b[NGLS][3];
	double x[NGLS][3], L[3][3], U[3][3];
	double y[3];
	
	struct SoA new_b;
	struct SoA new_x;
	
	int i, j,k;
	int n=3;
	double sum;
	time_marker_t time;

	for (i = 0; i < NGLS; i++) {
		init(a[i], b[i], x[i]);
	}
	for (i = 0; i < NGLS; i++) {
		new_b.x[i] = b[i][0];
		new_b.y[i] = b[i][1];
		new_b.z[i] = b[i][2];
		new_x.x[i] = x[i][0];
		new_x.y[i] = x[i][1];
		new_x.z[i] = x[i][2];
	}

	time = get_time();
	int iteration;
	for (iteration = 0; iteration < NGLS; iteration++) {
		
		for (i = 0; i < n; i++) {

			for (j = i+1; j < n; j++) {
		  		a[iteration][i][j] = a[iteration][i][j] / a[iteration][i][i];
			}
			
			if (i==0) new_b.x[iteration] = new_b.x[iteration] / a[iteration][i][i];
			else if (i==1) new_b.y[iteration] = new_b.y[iteration] / a[iteration][i][i];
			else new_b.z[iteration] = new_b.z[iteration] / a[iteration][i][i];

			for (j = i+1; j < n; j++) {
				double factor = a[iteration][j][i];
				for (k = i; k < n; k++) {
					a[iteration][j][k] = a[iteration][j][k] - a[iteration][i][k] * factor;
				}
				
				double subtraction;
				
				if (i==0) subtraction = factor*new_b.x[iteration];
				else if (i==1) subtraction = factor*new_b.y[iteration];
				else subtraction = factor*new_b.z[iteration];
				
				if (j==0) new_b.x[iteration] = new_b.x[iteration] - subtraction;
				else if (j==1) new_b.y[iteration] = new_b.y[iteration] - subtraction;
				else new_b.z[iteration] = new_b.z[iteration] - subtraction;
			}

		}
	

		for (i = n-1; i >= 0; i--) {
			if (i==0) new_x.x[iteration] = new_b.x[iteration];
			else if (i==1) new_x.y[iteration] = new_b.y[iteration];
			else new_x.z[iteration] = new_b.z[iteration];
			
			for(j = i+1; j < n; j++) {
				double value;
				if (j==0) value = a[iteration][i][j] * new_x.x[iteration];
				else if (j==1) value = a[iteration][i][j] * new_x.y[iteration];
				else value = a[iteration][i][j] * new_x.z[iteration];
				
				if (i==0) new_x.x[iteration] = new_x.x[iteration] - value;
				else if (i==1) new_x.y[iteration] = new_x.y[iteration] - value;
				else new_x.z[iteration] = new_x.z[iteration] - value;
			}
		}

	}
	printf("Vectorized Gauss: %f   ticks: %f\n", get_ToD_diff_time(time), get_ticks_diff_time(time));
	
	// RE - INITIALIZE
	/*for (i = 0; i < NGLS; i++) {
		init(a[i], b[i], x[i]);
	}
	
	
	time = get_time();
	lu_decomposition(a[0],L,U);
	for (k = 0; k < NGLS; k++)
	{
		y[0] = 1; y[1] = 1; y[2] = 1; 
		
		// THIS CAN BE VECTORIZED
		// L x Y = B
		for(i=0; i<n; i++)
    	{
        	sum = 0; 
        	for(j=0; j<=i; j++)
        	{
            	sum+=L[i][j]*y[j]; 
        	}
        	y[i]=b[k][i] / sum;
    	}
    	
    	// THIS CAN BE VECTORIZED
		// U x X = Y
		for(i=n-1; i>=0; i--)
    	{
        	x[k][i]= y[i];
        	for(j=i+1; j<n; j++)
        	{
            	x[k][i]-=U[i][j]*x[k][j];
        	}
        	x[k][i]/=U[i][i];
    	}
    	
		//print_vector("X solved", x[0]);
	}
	printf("LU FACTORIZATION: Time elapsed. time: %f   ticks: %f\n", get_ToD_diff_time(time), get_ticks_diff_time(time));*/

	return(0);
}

void init(double a[3][3], double b[3], double x[3]) {
	int i,j;
	int n = 3;

	for (j=0;j<n;j++)
	{
		b[j]=(float)(2*n-2)+(float)(j+1);
		a[j][j]=(float)(n-1)+(float)(j+1);
		x[j]=0.;

		for (i=j+1;i<n;i++) {
			a[i][j]=1.;
		}
		for (i=0;i<j;i++) {
			a[i][j]=1.;
		}
	}
}


void gauss_elimination(double a[3][3], double b[3], double x[3]) {
	int n = 3;
	int i,j,k;
	//print_matrix("before",a);
	
	for (i = 0; i < n; i++) {

		for (j = i+1; j < n; j++) {
		  	a[i][j] = a[i][j] / a[i][i];
		}
		b[i] = b[i] / a[i][i];
		//print_matrix("first substituiton",a);

		for (j = i+1; j < n; j++) {
			double factor = a[j][i];
			for (k = i; k < n; k++) {
				a[j][k] = a[j][k] - a[i][k] * factor;
			}
			b[j] = b[j] - factor * b[i];
		}
		
		//print_matrix("after 1 cycle a",a);
		//print_vector("after 1 cycle b",b);
	}
	

	for (i = n-1; i >= 0; i--) {
		x[i] = b[i];
		for(j = i+1; j < n; j++) {
			//printf("a,i,j: %f\n",a[i][j]);
			x[i] -= a[i][j] * x[j];
		}
	}
}


/**
*
* Code reference "C++ Program to Perform LU Decomposition of any Matrix" 
* http://www.sanfoundry.com/cpp-program-perform-lu-decomposition-any-matrix/
*
**/

int lu_decomposition(double a[3][3], double l[3][3], double u[3][3]) {
	int i, j, k;
	int n=3;
	
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (j < i)
                l[j][i] = 0;
            else
            {
                l[j][i] = a[j][i];
                for (k = 0; k < i; k++)
                {
                    l[j][i] = l[j][i] - l[j][k] * u[k][i];
                }
            }
        }
        for (j = 0; j < n; j++)
        {
            if (j < i)
                u[i][j] = 0;
            else if (j == i)
                u[i][j] = 1;
            else
            {
                u[i][j] = a[i][j] / l[i][i];
                for (k = 0; k < i; k++)
                {
                    u[i][j] = u[i][j] - ((l[i][k] * u[k][j]) / l[i][i]);
                }
            }
        }
    }

    return 0;
}

void print_matrix(char* name, double matrix[3][3]) {
	int i, j;
	printf("Matrix %s: \n", name);
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			printf(" %f ", matrix[i][j]);
		}
		printf(" ;\n");
	}
}

void print_vector(char* name, double vec[3]) {
	int i;
	printf("vector %s: \n", name);
	for (i=0;i<3;i++)
	{
		printf(" %f \n",vec[i]);
	}
}

