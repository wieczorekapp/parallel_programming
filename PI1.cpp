#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;

int main(int argc, char* argv[])
{
	clock_t start, stop;
	//double start_r, stop_r;

	double x, pi, sum=0.0;
	int i;
	step = 1./(double)num_steps;

	//start_r = omp_get_wtime();
	start = clock();

	for (i=0; i<num_steps; i++)
	{
		x = (i + .5)*step;
		sum = sum + 4.0/(1.+ x*x);
	}
	
	pi = sum*step;
	stop = clock();
	//stop_r = omp_get_wtime();

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania wynosi %f sekund czasu procesora\n",((double)(stop - start)/1000.0));
	//printf("Czas przetwarzania wynosi %f sekund czasu rzeczysistego\n",((double)(stop_r - start_r)));
	return 0;
}