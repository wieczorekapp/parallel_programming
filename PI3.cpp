#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;

int main(int argc, char* argv[])
{
	clock_t start, stop;
	double pi, sum = 0.0;
	step = 1./(double)num_steps;
	int i;
	start = clock();


	#pragma omp parallel num_threads(16)
	{
		double x;

		#pragma omp for 
		for (i = 0; i < num_steps; i++)
		{
			x = (i + .5) * step;
			#pragma omp atomic
			sum += 4.0 / (1. + x * x);

		}
	}


	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania wynosi %f sekund\n",((double)(stop - start)/1000.0));
	return 0;
}