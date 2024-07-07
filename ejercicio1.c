#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estructura para pasar argumentos a los hilos
typedef struct {
    int num;
    int result;
    int order;
} thread_arg;

// Variable global para el control de impresión ordenada
int next_order = 0;
pthread_mutex_t lock;
pthread_cond_t cond;

void* calculate_factorial(void* arg) {
    thread_arg* t_arg = (thread_arg*)arg;
    int num = t_arg->num;
    int result = 1;

    // Calculo del factorial
    for (int i = 1; i <= num; ++i) {
        result *= i;
    }

    // Guardar el resultado en la estructura
    t_arg->result = result;

    // Sincronización para la impresión ordenada
    pthread_mutex_lock(&lock);
    while (t_arg->order != next_order) {
        pthread_cond_wait(&cond, &lock);
    }
    printf("Factorial de %d: %d\n", num, result);
    next_order++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s num1 num2 ... numN\n", argv[0]);
        return 1;
    }

    int num_args = argc - 1;
    pthread_t threads[num_args];
    thread_arg t_args[num_args];

    // Inicializar mutex y condicional
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    // Crear hilos y pasar argumentos
    for (int i = 0; i < num_args; ++i) {
        t_args[i].num = atoi(argv[i + 1]);
        t_args[i].order = i;
        pthread_create(&threads[i], NULL, calculate_factorial, &t_args[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_args; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Destruir mutex y condicional
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);

    return 0;
}
