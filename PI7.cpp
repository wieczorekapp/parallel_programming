#include <stdio.h>
#include <time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;

int main(int argc, char* argv[])
{
	clock_t start, stop;
	double pi, sum = 0.0;
	volatile double tab[50];
	int i;


	step = 1. / (double)num_steps;
	for (int j = 0; j < 30; j++) {
		sum = 0.0;
		start = clock();
		#pragma omp parallel num_threads(2)
		{
			double x = 0.0;
			int id = omp_get_thread_num();
			tab[id + j] = 0;

			#pragma omp for nowait
			for (i = 0; i < num_steps; i++)
			{
				x = (i + .5) * step;
				tab[id + j] += 4.0 / (1. + x * x);
			}

			#pragma omp atomic
			sum += tab[j + id];

		}

		pi = sum * step;
		stop = clock();

		printf("%d Czas przetwarzania wynosi %f sekund\n", j, ((double)(stop - start) / 1000.0));

	}
	return 0;
}