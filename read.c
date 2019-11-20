#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

struct timeval inicio;
struct timeval fim;
void start()
{
	gettimeofday(&inicio, 0);
}
void finish()
{
	gettimeofday(&fim, 0);
}
float timedifference_sec(struct timeval t0, struct timeval t1)
{
	return (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0f;
}
double getDelta()
{
	return timedifference_sec(inicio, fim);
}

typedef struct thread_args
{
	int id;
	char **argv;
	int **vetor;
	int vetN;
	int **ordem;
	int max;
} thread_args;

//Funcao auxiliar para o quicksort
int cmp(const void *x, const void *y)
{
	return *(int *)x - *(int *)y;
}

int countMax(char *arquivo)
{
	FILE *inputFile = fopen(arquivo, "r");
	if (inputFile == NULL)
		return -1;
	int i = 0;
	int a;
	while (!feof(inputFile))
	{
		fscanf(inputFile, "%d ", &a);
		i++;
	}
	fclose(inputFile);
	return i;
}

int **newVetor(int vetN, int max)
{
	int **vetor = (int **)malloc(sizeof(int *) * vetN);
	for (int i = 0; i < vetN; i++)
	{
		vetor[i] = (int *)malloc(sizeof(int) * max);
		//inicializa todos os valores da matriz com 0
		for (int j = 0; j < max; j++)
		{
			vetor[i][j] = 0;
		}
	}
	return vetor;
}

void salvaVetor(char *arquivo, int **vetor, int vetN, int max)
{
	FILE *output = fopen(arquivo, "w");
	for (int i = 0; i < vetN; i++)
	{
		for (int j = 0; j < max; j++)
		{
			fprintf(output, "%d\t", vetor[i][j]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
}

void *lerArquivo(void *ptr)
{
	thread_args *arg = (thread_args *)ptr;
	char **argv = arg->argv;
	int **vetor = arg->vetor;
	int vetN = arg->vetN;
	int **ordem = arg->ordem;
	int linha = -1;
	for (int i = 0; i < vetN; i++)
	{
		if (ordem[i][0] != -1)
		{
			linha = ordem[i][0];
			ordem[i][0] = -1;
			break;
		}
	}

	if (linha == -1)
	{
		pthread_exit(NULL);
	}
	char *arquivo = argv[linha + 2];

	// printf("thread: %ld\n", *(int *)(arg->args[4]);

	FILE *inputFile = fopen(arquivo, "r");
	//armazena todos os numeros do arquivo na linha da matriz
	//o restante da matriz ficará preenchido com 0
	int j = 0;
	while (!feof(inputFile))
	{
		fscanf(inputFile, "%d ", &vetor[linha][j++]);
	}
	fclose(inputFile);
	// printf("id: %d, linha: %d, arquivo: %s, num numeros: %d\n", arg->id, linha, arquivo, j);

	// qsort(vetor[linha], j, sizeof(int), cmp);
	//de maneira recursiva procura se mais alguma linha não foi concluida
	lerArquivo(arg);
}

void *ordenaVetor(void *ptr)
{
	thread_args *arg = (thread_args *)ptr;
	int **vetor = arg->vetor;
	int vetN = arg->vetN;
	int **ordem = arg->ordem;
	int linha = -1;
	int max = arg->max;
	for (int i = 0; i < vetN; i++)
	{
		if (ordem[i][0] != -1)
		{
			linha = ordem[i][0];
			ordem[i][0] = -1;
			break;
		}
	}

	if (linha == -1)
	{
		pthread_exit(NULL);
	}
	// printf("Ordenando linha:%d id: %d\n", linha, arg->id);

	qsort(vetor[linha], max, sizeof(int), cmp);
	//de maneira recursiva procura se mais alguma linha não foi concluida
	ordenaVetor(arg);
}
int main(int argc, char *argv[])
{
	/* Todas essas variáveis foram criadas para controle de argc e argv, de maneira a não importar a quantidade de arquivos de entrada DE ACORDO COM A EXIGẼNCIA DO PROFESSOR */

	int qtdThreads = atoi(argv[1]);
	int i, vetCounter = 0;														//contador de proposito geral e contador dos vetores
	int max = 0;																//variavel para armazenar a maior quantidade de numeros dos arquivos
	int buf[5000];																//buffer para auxiliar contagem de quantidade de numeros nos arquivos
	int fileID = 2;																//usado como parametro do argv[] para leitura dos arquivos
	int fileController = argc - 3;												//usado como controle do laco de leitura de arquivos
	int outputFile = argc - 1;													//parametro do argv[] do arquivo de saida
	int vetN = argc - 4;														//usado como controle do laco de escrita do arquivo de saida
	pthread_t *thread_id = (pthread_t *)malloc(sizeof(pthread_t) * qtdThreads); //alocamento de um vetor de threads

	//Funcao para achar o arquivo com a maior quantidade de numeros e armazenar em max
	for (fileID = 2; fileID <= fileController; fileID++)
	{
		int aux = countMax(argv[fileID]);
		//se nao encontrou o arquivo, encerra o programa
		if (aux == -1)
			return EXIT_FAILURE;
		if (aux > max)
		{
			max = aux;
		}
	}

	/* Com o max definido, a melhor maneira seria criar uma matrix alocada dinamicamente (int**) no lugar da linha de código estática abaixo */

	/* Se formos usar matriz alocada dinamicamente, será necessário redefinir alguns contadores talvez. Dar uma atenção para isso. */

	int **vetor = newVetor(vetN, max);

	fileID = 2;
	int **ordem = newVetor(vetN, 1); //criado um vetor para armazenar se a linha já foi ordenada ou não
	for (int i = 0; i < vetN; i++)
	{
		ordem[i][0] = i;
	}

	for (int i = 0; i < qtdThreads; i++, fileID++)
	{
		thread_args *argumentos = (thread_args *)malloc(sizeof(thread_args));

		argumentos->id = i;
		argumentos->argv = argv;
		argumentos->vetor = vetor;
		argumentos->vetN = vetN;
		argumentos->ordem = ordem;
		pthread_create(&thread_id[i], NULL, lerArquivo, argumentos);
	}
	for (int i = 0; i < qtdThreads; i++, fileID++)
	{
		pthread_join(thread_id[i], NULL);
	}

	for (int i = 0; i < vetN; i++)
	{
		ordem[i][0] = i;
	}

	//inicio do tempo
	start();

	for (int i = 0; i < qtdThreads; i++, fileID++)
	{
		thread_args *argumentos = (thread_args *)malloc(sizeof(thread_args));

		argumentos->id = i;
		argumentos->vetor = vetor;
		argumentos->vetN = vetN;
		argumentos->ordem = ordem;
		argumentos->max = max;
		pthread_create(&thread_id[i], NULL, ordenaVetor, argumentos);
	}
	for (int i = 0; i < qtdThreads; i++, fileID++)
	{
		pthread_join(thread_id[i], NULL);
	}
	//registra o fim do tempo
	finish();

	printf("Numero de threads: %d\n", qtdThreads);
	printf("Tempo total de: %lfs", getDelta());

	// //Termina de contar tempo

	// /* O que deve ser calculado no programa é o TEMPO DE ORDENACAO DOS VETORES A PARTIR DA CRIAÇÃO DA THREADS (segundo o que eu entendi e o que faz mais sentido), então deixarei sinalizado como comentário aqui acima */

	// //Impressao dos vetores
	salvaVetor(argv[outputFile], vetor, vetN, max);


	//salva em um arquivo de texto os resultados
	char arquivo[20];
	sprintf(arquivo, "threads_%d.dat", qtdThreads);
	FILE *output = fopen(arquivo, "a");
	fprintf(output, "%lfs\n", getDelta());
	fclose(output);

	return 0;
}
