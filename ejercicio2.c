#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define M 20

//estructura que almacena la información para pasar a los hilos 
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

//función que va a buscar el num. en el subvector
void *buscar_en_subvector(void *arg){
    e *ejer2 = (e *) arg;
    for(int i=ejer2->inicio; i< ejer2->fin; i++){ //recorremos el subvector de inicio a fin
        if(ejer2->MAX[i] == ejer2->num_buscar){ //si el num. en la posición i es el num. buscado 
            pthread_mutex_lock(ejer2->lock); //se bloquea el mutex para asegurar el acceso exclusivo a la act. del resultado
            if(*ejer2->result == -1){ // si el resultado no se encuentra 
                *ejer2->result = i;  // se act. el resultado con el indice que encontró el num 
                printf("El hilo nro %d encontró el núm. en el índice %d \n", ejer2->id,i); //imprime el identificador del hilo y el indice que encontró el num.
            }
            pthread_mutex_unlock(ejer2->lock); //desbloqueamos el mutex
            return NULL;
        }
    }
    return NULL;
}
//función que busca el num. en un vector con distintos hilos
void buscarnum_en_vector(int num_buscar, int num_hilos){
    int MAX[M]; //array que almacena los num. enteros aleatorios
    srand(time(NULL)); //inicializacion para generar los num. aleatorios 
    for(int i=0; i<M; i++){ // generemos el array con num. aleatorios del 0 al 9
        MAX[i] = rand() % 10;
    }
    printf("\nVector generado: ");
    printf("|");
    for (int i = 0; i < M; i++){ //imprimimos el vector con los num aleatorios
       printf("%d|", MAX[i]);
    }
    printf("\n");
    pthread_t hilos[num_hilos]; //declaramos un identif. de hilos 
    e ejer2[num_hilos]; //declaramos un array de estruct. para cada hilo
    pthread_mutex_t lock; //declaramos el mutex
    pthread_mutex_init(&lock,NULL); //lo inicializamos
    int resultado = -1; //inicializamos el resultado en -1 para indicar que no se ha encontrado el um
    int tam_subvector = (M + num_hilos - 1)/ num_hilos; //calculamos el tamaño de cada hilo de acuerdo al tamaño del vector generado
    //se crea y config. los hilos
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
        pthread_join(hilos[i], NULL); //se espera que todos los hilos terminen su ejecución
    }
    pthread_mutex_destroy(&lock); //se destruye el mutex
    if(resultado != -1){ //imprimimos el resultado
        printf("\nEl num. %d fue encontrado en el ind. %d n", num_buscar, resultado);
    }else{
        printf("\nEl num. %d no está en el vector. ",num_buscar);
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){ //verificamos que el num. de argumentos sea correcto
        printf("Uso: %s <numero_a_buscar> <num_hilos>\n", argv[0]); //sino emitimos mensaje
        return 1;
    }
    //convertimos los argumentos de cadena a enteros
    int num_buscar = atoi(argv[1]);
    int num_hilos = atoi(argv[2]);
    //llamamos a la funcion para buscar el numero en el vector
    buscarnum_en_vector(num_buscar, num_hilos);
    return 0;
}