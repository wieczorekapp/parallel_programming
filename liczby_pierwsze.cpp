#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <iostream>

// PARAMETRY DO USTAWIENIA ====================================================================================================================
// liczba wątków w przetwarzaniu równoległym
const int THREAD_NUMBER = 16;

// zakres przeszukiwania liczb pierwszych <M;N>
const long long M = 2;
const long long N = 100000000;

// wielkość zakresów podczas wykreślania
const long long SIZE = 15000;

// czy wyswitlać liczby pierwsze
const bool SHOW_ALL = false;

// PARAMETRY DO USTAWIENIA ====================================================================================================================

// liczba obszarów do wykreślania
const long long NUMBER_AREAS = ceil((double)((N - M) + 1) / SIZE);

// zakres liczb służących do wykreślania
const long long SQRT_N = long(sqrt(N));

// tablice do obszarów
long long* START = (long long*)calloc(NUMBER_AREAS, sizeof(long long));
long long* END = (long long*)calloc(NUMBER_AREAS, sizeof(long long));


// wyznaczenie pierwszej wielokrotnosci w zakresie dla nieparzystych
long long first_odd(long long number, long long area_start) {
	long long tmp = (long long)ceil((double)area_start / number);
	
	if(tmp % 2 == 0) {
		return tmp + 1;
	}
	else {
		// gdy wielkortnosc przypadek gdy mniejsza od progu dolnego
		if (tmp <= number)
		{
			return number;
		}

		return tmp;
	}
}

// wyznaczenie pierwszej wielokrotnosci w zakresie dla parzystych
long long first_even(long long area_start) {
	long long tmp = (long long)ceil((double)area_start / 2);
	
	if(tmp == 1) {
		return tmp + 1;
	}
	else {
		if(tmp )

		return tmp;
	}
}


// wyznaczenie obszarów do przetwarzania ====================================================================================================
void areas(void) {
	START[0] = M;
	if (NUMBER_AREAS == 1) {
		END[0] = START[0] + ((N - M + 1) % SIZE) - 1;
	}
	else {
		END[0] = START[0] + SIZE - 1;
	}
	
	//printf("S: %lld E: %lld \n", START[0], END[0]);

	for (long long i = 1; i < NUMBER_AREAS; i++) {
		START[i] = START[i - 1] + SIZE;

		if (i == (NUMBER_AREAS - 1)) {
			if (((N - M + 1) % SIZE) == 0) {
				END[i] = START[i] + SIZE - 1;
			}
			else {
				END[i] = START[i] + ((N - M + 1) % SIZE) - 1;
			}
		}
		else {
			END[i] = START[i] + SIZE - 1;
		}

		//printf("S: %lld E: %lld \n", START[i], END[i]);
	}

}
// wyznaczenie obszarów do przetwarzania ====================================================================================================


// sekwencyjne sito =========================================================================================================================
void sequential_sieve(void) {
	// zmienne do pomiaru czasu działania algorytmu
	clock_t start, stop;
	long ile_pierwszych = 0;

	// tablica zawierająca informacje o wykreśleniach, liczba obliczna jest z indeksu(dodajemy M)
	bool *zlozona = (bool*) calloc (N - M + 1,sizeof(bool));


	start = clock();

	for (long long k = 0; k < NUMBER_AREAS; k++) {

		for (long long j = first_even(START[k]); j * 2 <= END[k]; j++) 
		{
			if (!zlozona[j * 2 - M])
				zlozona[j * 2 - M] = true;
		}

		for (long long i = 3; i <= SQRT_N; i += 2) 
		{	
			for (long long j = first_odd(i, START[k]); j * i <= END[k]; j += 2)
			{
				if (!zlozona[j * i - M])
					zlozona[j * i - M] = true;
			}
		}		


	}

	stop = clock();


	printf("SITO: Czas przetwarzania sekwencyjnego %f sekund\n",((double)(stop - start)/1000.0));

	// wyświtlanie
	for (long long i = 0; i < N - M + 1; i++)
	{
		if (!zlozona[i])
		{
			ile_pierwszych++;
		}
	}
	printf("Liczba liczb pierwszych: %d \n", ile_pierwszych);
	free(zlozona);
}
// sekwencyjne sito =========================================================================================================================



// równoległe sito funkcyjne ================================================================================================================
void parallel_sieve_func(void) {
	// zmienne do pomiaru czasu działania algorytmu
	clock_t start, stop;
	long long ile_pierwszych = 0;

	// tablica zawierająca informacje o wykreśleniach, liczba obliczna jest z indeksu(dodajemy M)
	bool *zlozona = (bool*) calloc (N - M + 1,sizeof(bool));


	start = clock();

	#pragma omp parallel num_threads(THREAD_NUMBER)
	{
		#pragma omp single nowait
		{
			for (long long j = first_even(M); j * 2 <= N; j++) 
			{

				if (!zlozona[j * 2 - M])
					zlozona[j * 2 - M] = true;
			}
		}

		#pragma omp for schedule(dynamic,1) nowait
		for (long long i = 3; i <= SQRT_N; i += 2) 
		{
			for (long long j = first_odd(i, M); j * i <= N; j += 2)
			{
				if (!zlozona[j * i - M])
					zlozona[j * i - M] = true;
			}
		}	
	}

	stop = clock();

	printf("SITO: Czas przetwarzania rownoleglego funkcyjengo %f sekund\n",((double)(stop - start)/1000.0));
	
	
	// wyświtlanie
	for (long long i = 0; i < N - M + 1; i++)
	{
		if (!zlozona[i])
		{
			ile_pierwszych++;
		}
	}
	printf("Liczba liczb pierwszych: %d \n", ile_pierwszych);
	free(zlozona);
	

}
// równoległe sito funkcyjne ================================================================================================================


// równoległe sito domenowe =================================================================================================================
void parallel_sieve_domen(void) {
	// zmienne do pomiaru czasu działania algorytmu
	clock_t start, stop;
	long ile_pierwszych = 0;

	// tablica zawierająca informacje o wykreśleniach, liczba obliczna jest z indeksu(dodajemy M)
	bool *zlozona = (bool*) calloc (N - M + 1,sizeof(bool));

	start = clock();

	#pragma omp parallel num_threads(THREAD_NUMBER)
	{
		#pragma omp for schedule(dynamic,1) nowait
		for (long long k = 0; k < NUMBER_AREAS; k++) {
			for (long long j = first_even(START[k]); j * 2 <= END[k]; j++) 
			{
				if (!zlozona[j * 2 - M])
					zlozona[j * 2 - M] = true;
			}

			for (long long i = 3; i <= SQRT_N; i += 2) 
			{	
				for (long long j = first_odd(i, START[k]); j * i <= END[k]; j += 2)
				{			
					if (!zlozona[j * i - M])
						zlozona[j * i - M] = true;
				}
			}
		}
	}


	stop = clock();

	printf("SITO: Czas przetwarzania rownoleglego domenowego %f sekund\n",((double)(stop - start)/1000.0));

	// wyświtlanie
	for (long long i = 0; i < N - M + 1; i++)
	{
		if (!zlozona[i])
		{
			ile_pierwszych++;
		}
	}
	printf("Liczba liczb pierwszych: %d \n", ile_pierwszych);
	free(zlozona);
}
// równoległe sito domenowe =================================================================================================================


int main(int argc, char* argv[])
{
	printf("OBSZAROW %d \n", NUMBER_AREAS);
	// wyznaczenie obszarów
	areas();

	printf("=========================================================== \n");
	sequential_sieve();
	printf("=========================================================== \n");
	parallel_sieve_func();
	printf("=========================================================== \n");
	parallel_sieve_domen();
	printf("=========================================================== \n");

	free(START);
	free(END);

	return 0;
}



