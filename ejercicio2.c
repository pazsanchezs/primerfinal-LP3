#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define M 20

typedef struct ejercicio2
{
    int *MAX;  // puntero al vector de max numeros enteros aleatorios
    int num_buscar; // número que se va a buscar
    int inicio; // inicio del subvector 
    int fin; // fin del subvector 
    int id; //identificador del hilo
    int *result; // puntero al resultado donde se almacenará lo encontrado
    pthread_mutex_t *lock; //puntero al mutex para asegurar la actualización del resultado
}e;


void *buscar_en_subvector(void *arg){
    e *ejer2 = (e *) arg;
    for(int i=ejer2->inicio; i< ejer2->fin; i++){
        if(ejer2->MAX[i] == ejer2->num_buscar){
            pthread_mutex_lock(ejer2->lock);
            if(*ejer2->result == -1){
                *ejer2->result = i;
                printf("El hilo nro %d encontró el núm. en el índice %d \n", ejer2->id,i);
            }
            pthread_mutex_unlock(ejer2->lock);
            return NULL;
        }
    }
    return NULL;
}

void buscarnum_en_vector(int num_buscar, int num_hilos){
    int MAX[M];
    srand(time(NULL));
    for(int i=0; i<M; i++){
        MAX[i] = rand() % 10;
    }
    printf("\nVector generado: ");
    printf("|");
    for (int i = 0; i < M; i++){
       printf("%d|", MAX[i]);
    }
    printf("\n");
    pthread_t hilos[num_hilos];
    e ejer2[num_hilos];
    pthread_mutex_t lock;
    pthread_mutex_init(&lock,NULL);
    int resultado = -1;
    int tam_subvector = (M + num_hilos - 1)/ num_hilos;
    for(int i = 0; i < num_hilos; i++){
        ejer2[i].MAX = MAX;
        ejer2[i].num_buscar = num_buscar;
        ejer2[i].inicio = i * tam_subvector;
        ejer2[i].fin = (i+1) * tam_subvector > M? M:(i + 1) * tam_subvector;
        ejer2[i].id = i;
        ejer2[i].result = &resultado;
        ejer2[i].lock = &lock;
        pthread_create(&hilos[i],NULL, buscar_en_subvector, &ejer2[i]);
    }
    for(int i = 0; i < num_hilos; i++){
        pthread_join(hilos[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    if(resultado != -1){
        printf("\nEl núm. %d fue encontrado en el ind. %d n", num_buscar, resultado);
    }else{
        printf("\nEl núm. %d no está en el vector. ",num_buscar);
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Uso: %s <numero_a_buscar> <num_hilos>\n", argv[0]);
        return 1;
    }
    int num_buscar = atoi(argv[1]);
    int num_hilos = atoi(argv[2]);
    buscarnum_en_vector(num_buscar, num_hilos);
    return 0;
}