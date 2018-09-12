#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


    typedef struct {
        int fila;
        int columna;
    } filacolumna;

    int valid[27] = {0};
    int sudoku[9][9];


    //Checkea si una columna es valida
    void *colValida(void* fc) {
        //Veo si la fila y columna corresponden a una columna
        filacolumna *filcol = (filacolumna*) fc;
        int fil = filcol->fila;
        int col = filcol->columna;
        if (fil != 0 || col > 8) {
            fprintf(stderr, "Fila o columna invalida! fila=%d, columna=%d\n", fil, col);
            pthread_exit(NULL);
        }

        //Veo si en la columna hay solo una aparicion del 1-9
        int visto[9] = {0};
        int i;
        for (i = 0; i < 9; i++) {
            int num = sudoku[i][col];
            if (num < 1 || num > 9 || visto[num - 1] == 1) {
                pthread_exit(NULL);
            } else {
                visto[num - 1] = 1;
            }
        }
        //La columna es valida, guardo en el arreglo y salgo
        valid[18 + col] = 1;
        pthread_exit(NULL);
    }

    //Checkea si una fila es valida
    void *filValida(void* fc) {
        //Veo si la fila y columna corresponden a una fila
        filacolumna *filcol = (filacolumna*) fc;
        int fil = filcol->fila;
        int col = filcol->columna;
        if (col != 0 || fil > 8) {
            fprintf(stderr, "Fila o columna invalida! fila=%d, columna=%d\n", fil, col);
            pthread_exit(NULL);
        }

        //Veo si en la fila hay solo una aparicion del 1-9
        int visto[9] = {0};
        int i;
        for (i = 0; i < 9; i++) {
            // If the corresponding index for the number is set to 1, and the number is encountered again,
            // the valid array will not be updated and the thread will exit.
            int num = sudoku[fil][i];
            if (num < 1 || num > 9 || visto[num - 1] == 1) {
                pthread_exit(NULL);
            } else {
                visto[num - 1] = 1;
            }
        }
        //La columna es valida, guardo en el arreglo y salgo
        valid[9 + fil] = 1;
        pthread_exit(NULL);
    }

    //Checkea si una subseccion de 3x3 es valida
    void *subsValida(void* fc) {
        //Veo si la fila y columna corresponden a una subseccion 3x3
        filacolumna *filcol = (filacolumna*) fc;
        int fil = filcol->fila;
        int col = filcol->columna;
        if (fil > 6 || fil % 3 != 0 || col > 6 || col % 3 != 0) {
            fprintf(stderr, "Fila o columna invalida! fila=%d, columna=%d\n", fil, col);
            pthread_exit(NULL);
        }
        int visto[9] = {0};
        int i, j;
        for (i = fil; i < fil + 3; i++) {
            for (j = col; j < col + 3; j++) {
                int num = sudoku[i][j];
                if (num < 1 || num > 9 || visto[num - 1] == 1) {
                    pthread_exit(NULL);
                } else {
                    visto[num - 1] = 1;
                }
            }
        }
        //La subseccion es valida, guardo en el arreglo y salgo
        valid[fil + col/3] = 1;
        pthread_exit(NULL);
    }
    int main() {
	pthread_t threads[27];
    FILE *fp;
    fp=fopen("sudoku.txt","r");
    int i,j;
    for(i=0;i<9;i++){
        int j;
        for(j=0;j<18;j++){
            char c;
            c=fgetc(fp);
            if(c!=EOF){
                if(c >= '0' && c <= '9'){
                    int n=atoi(&c);
                    sudoku[i][j/2] = n;
                }
            }
        }
    }
	int threadNum = 0;
	// Create 9 threads for 9 3x3 subsections, 9 threads for 9 columns and 9 threads for 9 rows.
	// This will end up with a total of 27 threads.
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (i%3 == 0 && j%3 == 0) {
				filacolumna *subData = (filacolumna *) malloc(sizeof(filacolumna));
				subData->fila = i;
				subData->columna = j;
				pthread_create(&threads[threadNum++], NULL, subsValida, subData); // 3x3 subsection threads
			}
			if (i == 0) {
				filacolumna *colData = (filacolumna *) malloc(sizeof(filacolumna));
				colData->fila = i;
				colData->columna = j;
				pthread_create(&threads[threadNum++], NULL, colValida, colData);	// column threads
			}
			if (j == 0) {
				filacolumna *filData = (filacolumna *) malloc(sizeof(filacolumna));
				filData->fila = i;
				filData->columna = j;
				pthread_create(&threads[threadNum++], NULL, filValida, filData); // row threads
			}
		}
	}

	for (i = 0; i < 27; i++) {
		pthread_join(threads[i], NULL);			// Wait for all threads to finish
	}

	// If any of the entries in the valid array are 0, then the sudoku solution is invalid
	for (i = 0; i < 27; i++) {
		if (valid[i] == 0) {
			printf("Sudoku solution is invalid!\n");
			return EXIT_SUCCESS;
		}
	}
	printf("Sudoku solution is valid!\n");
	return EXIT_SUCCESS;
}
