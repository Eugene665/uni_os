#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstdlib>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "Rus");

    // Вводим параметр от пользователя
    int N = atoi(argv[1]);

    cout << "запущен главный процесс PID = " << getpid() << "\n";

    // Массив для хранения PID дочерних процессов
    pid_t pids[N];

    for (int i = 0; i < N; i++) {
        // Создаём дочернюю ветку главного процесса
        pid_t pid = fork();

        // Проверка, что процесс дочерний, тк
        // в дочернем процессе fork() возвращает 0,
        // в родительском возвращает PID дочернего процесса
        if (pid == 0) {
            cout << "новый дочерний процесс PID = " << getpid()
                 << ", PPID = " << getppid()
                 << " (i = " << i << ")\n";

            cout << "дочерний процесс " << getpid() << " засыпает\n";
            sleep(4);
            cout << "дочерний процесс " << getpid() << " проснулся\n";

            cout << i << " дочерний процесс завершён PID = " << getpid()
                 << ", PPID = " << getppid() << "\n";

            // Дочерний процесс завершает работу
            // для выхода из цикла, иначе
            // каждый дочерний процесс будет образовать
            // свои ветки дочерних процессов
            return 0;
        }
        else {
            // Родитель сохраняет PID дочернего процесса в массив
            pids[i] = pid;
        }
    }

    // Цикл ожидания завершения дочерних процессов
    // Цикл ждёт N завершений для каждого дочернего процесса,
    // после родительский процесс заканчивает свою работу
    //for (int i = 0; i < N; i++) {
    //    wait(nullptr);
    //}

    // Цикл ожидания завершения конкретных дочерних процессов
    // waitpid для ожидания завершения каждого дочернего процесса
    for (int i = 0; i < N; i++) {
        int status;
        // Ожидаем завершение дочернего процесса
        waitpid(pids[i], &status, 0);
    }

    cout << "главный процесс завершён PID = " << getpid() << endl;
    return 0;
}


