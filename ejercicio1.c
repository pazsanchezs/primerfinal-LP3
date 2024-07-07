#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//estructura para pasar argumentos a los hilos
typedef struct {
    int numero; //numero a pasar como argumento
    int fact; //resultado
    int order; //orden en que se debe imprimir el resultado
} thread_arg;

//para el control de impresion ordenada
int next_order = 0; //proximo orden esperado
pthread_mutex_t lock; //para proteger la seccion critica
pthread_cond_t cond; //para la sincronizacion de la impresion

void* cFactorial(void* arg) {
    thread_arg* t_arg = (thread_arg*)arg;
    int numero = t_arg->numero;
    int fact = 1;

    //calculamos el factorial
    for (int i = 1; i <= numero; ++i) {
        fact *= i;
    }

    //guardamos el resultado
    t_arg->fact = fact;

    //para asegurar la impresion ordenada
    pthread_mutex_lock(&lock);
    while (t_arg->order != next_order) {
        pthread_cond_wait(&cond, &lock);
    }
    printf("Factorial de %d: %d\n", numero, fact);
    next_order++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) { //verificamos los argumentos
        fprintf(stderr, "Uso: %s numero1 numero2 ... numeroN\n", argv[0]);
        return 1;
    }

    int num_args = argc - 1; //numero de argumentos que pasamos
    pthread_t threads[num_args];
    thread_arg t_args[num_args];

    //inicializar mutex y condicional
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    //para crear los hilos y pasar argumentos
    for (int i = 0; i < num_args; ++i) {
        t_args[i].numero = atoi(argv[i + 1]);
        t_args[i].order = i;
        pthread_create(&threads[i], NULL, cFactorial, &t_args[i]);
    }

    //esperar a que todos los hilos terminen
    for (int i = 0; i < num_args; ++i) {
        pthread_join(threads[i], NULL);
    }

    //destruir mutex y condicional
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
