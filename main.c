#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


    typedef struct { //struct para pasar los parametros a los threads
        int fila;
        int columna;
    } filacolumna;

    int valid[27] = {0};
    int sudoku[9][9];
    pthread_t threads[27];

    //Checkea si una columna es valida
    void *colValida(void* fc) {
        filacolumna *filcol = (filacolumna*) fc;
        int col = filcol->columna;
        int visto[9] = {0};
        int i;
        for (i = 0; i < 9; i++) {
            int num = sudoku[i][col];
            if (visto[num - 1] == 1) {
                pthread_exit(EXIT_SUCCESS);
            } else {
                visto[num - 1] = 1;
            }
        }
        //La columna es valida, guardo en el arreglo y salgo
        valid[18 + col] = 1;
        free(fc);
        pthread_exit(EXIT_SUCCESS);
    }

    //Checkea si una fila es valida
    void *filValida(void* fc) {
        filacolumna *filcol = (filacolumna*) fc;
        int fil = filcol->fila;
        int visto[9] = {0};
        int i;
        for (i = 0; i < 9; i++) {
            int num = sudoku[fil][i];
            if (visto[num - 1] == 1) {
                pthread_exit(EXIT_SUCCESS);
            } else {
                visto[num - 1] = 1;
            }
        }
        //La columna es valida, guardo en el arreglo y salgo
        valid[9 + fil] = 1;
        free(fc);
        pthread_exit(EXIT_SUCCESS);
    }

    //Checkea si una subseccion de 3x3 es valida
    void *subsValida(void* fc) {
        filacolumna *filcol = (filacolumna*) fc;
        int fil = filcol->fila;
        int col = filcol->columna;
        int visto[9] = {0};
        int i,j;
        for (i = fil; i < fil + 3; i++) {
            for (j = col; j < col + 3; j++) {
                int num = sudoku[i][j];
                if (visto[num - 1] == 1) {
                    pthread_exit(EXIT_SUCCESS);
                } else {
                    visto[num - 1] = 1;
                }
            }
        }
        //La subseccion es valida, guardo en el arreglo y salgo
        valid[fil + col/3] = 1;
        free(fc);
        pthread_exit(EXIT_SUCCESS);
    }
    void crearThreads(){
        int threadNum = 0;
        int i;
        int j;
        //Creo los 27 threads, 9 para las filas, 9 para las columnas y 9 para las subsecciones
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                if (i%3 == 0 && j%3 == 0) {
                    filacolumna *subData = (filacolumna *) malloc(sizeof(filacolumna));
                    if(subData == NULL){
                        printf("Error malloc");
                        exit(EXIT_FAILURE);
                    }
                    subData->fila = i;
                    subData->columna = j;
                    if(pthread_create(&threads[threadNum++], NULL, subsValida, subData)){
                        perror("Error creando thread");
                    }
                }
                if (i == 0) {
                    filacolumna *colData = (filacolumna *) malloc(sizeof(filacolumna));
                    if(colData == NULL){
                        printf("Error malloc");
                        exit(EXIT_FAILURE);
                    }
                    colData->fila = i;
                    colData->columna = j;
                    if(pthread_create(&threads[threadNum++], NULL, colValida, colData)){
                        perror("Error creando thread");
                    }
                }
                if (j == 0) {
                    filacolumna *filData = (filacolumna *) malloc(sizeof(filacolumna));
                    if(filData == NULL){
                        printf("Error malloc");
                        exit(EXIT_FAILURE);
                    }
                    filData->fila = i;
                    filData->columna = j;
                    if(pthread_create(&threads[threadNum++], NULL, filValida, filData)){
                        perror("Error creando thread");
                    }
                }
            }
        }
    }

    void cargarSudoku(){
        FILE *fp;
        fp=fopen("sudoku.txt","r");
        if(fp==NULL){
            exit(EXIT_FAILURE);
        }
        int i,j;
        for(i=0;i<9;i++){
            for(j=0;j<18;j++){
                char c;
                c=fgetc(fp);
                if(j%2 == 0){
                    if(c >= '0' && c <= '9'){
                        int n=atoi(&c);
                        sudoku[i][j/2] = n;
                    }
                }
            }
        }
    }

    int main() {
    cargarSudoku();
    crearThreads();
    int i;
    for (i = 0; i < 27; i++) {//Espero a que finalizen los threads
		pthread_join(threads[i], NULL);
    }
	for (i = 0; i < 27; i++) {
		if (valid[i] == 0) {
			printf("El sudoku es invalido!\n");
			return 0;
		}
	}
	printf("El sudoku es valido!\n");
	return 0;
}
