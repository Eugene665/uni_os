#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024 // Размер буфера для ввода строки
#define EXIT_COMMAND "exit" // Команда для завершения программы

int main() {
    char input[BUFFER_SIZE]; // Буфер для ввода
    char *args1[BUFFER_SIZE / 2 + 1]; // Массив для хранения первой команды и её аргументов
    char *args2[BUFFER_SIZE / 2 + 1]; // Массив для хранения второй команды и её аргументов
    char *outputFile = NULL; // Имя файла для перенаправления вывода
    pid_t pid; // Идентификатор процесса

    while (1) {
        // Вывод приглашения к вводу
        printf("Введите команду >>> ");
        fflush(stdout);

        // Считывание строки
        if (!fgets(input, sizeof(input), stdin)) {
            perror("Ошибка ввода");
            continue;
        }

        // Удаление символа новой строки
        input[strcspn(input, "\n")] = '\0';

        // Проверка на команду выхода
        if (strcmp(input, EXIT_COMMAND) == 0) {
            printf("Выход из программы.\n");
            break;
        }

        // Проверка на наличие символа конвейера '|'
        char *pipeSymbol = strchr(input, '|');
        if (pipeSymbol) {
            *pipeSymbol = '\0';
            char *cmd1 = strtok(input, " ");
            char *cmd2 = strtok(pipeSymbol + 1, " ");

            // Разбор первой команды
            int i = 0;
            while (cmd1 != NULL) {
                args1[i++] = cmd1;
                cmd1 = strtok(NULL, " ");
            }
            args1[i] = NULL;

            // Разбор второй команды
            i = 0;
            while (cmd2 != NULL) {
                args2[i++] = cmd2;
                cmd2 = strtok(NULL, " ");
            }
            args2[i] = NULL;

            // Создание канала
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("Ошибка создания канала");
                continue;
            }

            // Создание первого дочернего процесса
            pid = fork();
            if (pid == -1) {
                perror("Ошибка создания первого процесса");
                continue;
            } else if (pid == 0) {
                dup2(pipefd[1], STDOUT_FILENO); // Перенаправляем вывод в канал
                close(pipefd[0]); // Закрываем неиспользуемый конец канала
                close(pipefd[1]);
                execvp(args1[0], args1);
                perror("Ошибка выполнения первой команды");
                exit(EXIT_FAILURE);
            }

            // Создание второго дочернего процесса
            pid = fork();
            if (pid == -1) {
                perror("Ошибка создания второго процесса");
                continue;
            } else if (pid == 0) {
                dup2(pipefd[0], STDIN_FILENO); // Перенаправляем ввод из канала
                close(pipefd[1]); // Закрываем неиспользуемый конец канала
                close(pipefd[0]);
                execvp(args2[0], args2);
                perror("Ошибка выполнения второй команды");
                exit(EXIT_FAILURE);
            }

            // Родительский процесс
            close(pipefd[0]);
            close(pipefd[1]);
            wait(NULL); // Ожидание первого дочернего процесса
            wait(NULL); // Ожидание второго дочернего процесса
            continue;
        }

        // Проверка на наличие перенаправления `>`
        char *redirect = strchr(input, '>');
        if (redirect) {
            *redirect = '\0'; // Разделяем строку на команду и имя файла
            outputFile = strtok(redirect + 1, " ");
        } else {
            outputFile = NULL;
        }

        // Разбор строки на отдельные слова
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL) {
            args1[i++] = token;
            token = strtok(NULL, " ");
        }
        args1[i] = NULL; // Последний элемент массива равен NULL

        // Проверка на команду `cd`
        if (strcmp(args1[0], "cd") == 0) {
            if (args1[1] == NULL) {
                fprintf(stderr, "Ошибка: отсутствует аргумент для cd\n");
            } else if (chdir(args1[1]) == -1) {
                perror("Ошибка смены каталога");
            }
            continue; // Переход к следующей итерации цикла
        }

        // Проверка на команду `echo`
        if (strcmp(args1[0], "echo") == 0) {
            for (int j = 1; args1[j] != NULL; j++) {
                printf("%s ", args1[j]);
            }
            printf("\n");
            continue; // Переход к следующей итерации цикла
        }

        // Создание дочернего процесса
        pid = fork();

        if (pid < 0) {
            perror("Ошибка создания процесса");
            continue;
        } else if (pid == 0) {
            // Если есть перенаправление, открываем файл и перенаправляем вывод
            if (outputFile) {
                int fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("Ошибка открытия файла");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO); // Перенаправляем стандартный вывод в файл
                close(fd);
            }

            // Выполнение команды
            if (execvp(args1[0], args1) == -1) {
                perror("Ошибка выполнения команды");
                exit(EXIT_FAILURE);
            }
        } else {
            // Родительский процесс: ожидание завершения дочернего процесса
            wait(NULL);
        }
    }

    return 0;
}

