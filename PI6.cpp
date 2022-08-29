#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;

int main(int argc, char* argv[])
{
	clock_t start, stop;
	double pi, sum = 0.0;
	//double tab[16];
	volatile double tab[16];

	int i;
	step = 1./(double)num_steps;
	start = clock();


	#pragma omp parallel num_threads(16)
	{
		double x;
		int id = omp_get_thread_num();
		tab[id] = 0;


		#pragma omp for 
		for (i = 0; i < num_steps; i++)
		{
			x = (i + .5) * step;
			tab[id] += 4.0 / (1. + x * x);
		}

		#pragma omp atomic
		sum += tab[id];
	}


	pi = sum*step;
	stop = clock();

	printf("Wartosc liczby PI wynosi %15.12f\n",pi);
	printf("Czas przetwarzania wynosi %f sekund\n",((double)(stop - start)/1000.0));
	return 0;
}