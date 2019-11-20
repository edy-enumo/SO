# SO
Trabalho de Sistemas Operacionais UNICAMP
Instrução de compilação e execução
Para compilar basta executar o seguinte comando:
gcc read.c -o read
O programa read tem como entrada o número de Threads, os arquivos a serem lidos e nome do arquivo de saída. Exemplo:

./read 16 0.dat 1.dat 2.dat 3.dat 4.dat 5.dat saida.dat
O programa mostrará o tempo total de execução da ordenação dos vetores e salvará em um arquivo texto chamado threads_[num_thread].dat. E salvará o arquivo de saída com as linhas de cada arquivo ordenadas.

