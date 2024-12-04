#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cctype>

using namespace std;

int main(int argc, char *argv[]) {

    // Проверка наличия имени файла
    if (argc < 2) {
        cout << "Пожалуйста, укажите имя файла!" << endl;
        return 1;
    }

    const char *inputFile = argv[1]; // Имя входного файла
    char outputFile[256]; // Для имени выходного файла

    // Формируем имя выходного файла (добавляем "_stat.txt")
    // массив символов, в который будет записан отформатированный результат,
    // Это размер массива outputFile в байтах, который передается
    // как ограничение для записи в строку,
    // форматная строка
    // Это строка, значение которой будет подставлено
    // в место %s в форматной строке
    snprintf(outputFile, sizeof(outputFile), "%s_stat.txt", inputFile);

    // Открываем файл только для чтения
    int inputFd = open(inputFile, O_RDONLY);
    if (inputFd == -1) {
        perror("Не удалось открыть файл для чтения");
        return 1;
    }

    // Буфер для чтения данных из файла
    char buffer[1024];

    // Переменная для хранения числа прочитанных байтов
    // где ssize_t - тип для хранения байтов,
    // bytesRead — это переменная, в которой
    // будет храниться результат выполнения операции чтения
    ssize_t bytesRead;

    // Массив для подсчета символов (26 букв латинского алфавита)
    // где каждому элементу соответвует кол-во соответсвенной буквы
    int letterCount[26] = {0};

    // Чтение файла и подсчет символов
    // условие выхода из while пока функция read() не вернет значение,
    // меньшее или равное нулю
    while ((bytesRead = read(inputFd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            // Приводим к нижнему регистру для удобства подсчета
            char c = tolower(buffer[i]);
            // Проверка, что буква латинская
            if (c >= 'a' && c <= 'z') {
                // Увеличиваем счетчик для соответствующей буквы
                letterCount[c - 'a']++;
            }
        }
    }

    // Закрываем исходный файл
    close(inputFd);

    // Открываем файл для записи результатов
    // с флагами на открытие для записи, созданию, если его нет,
    // изменения при существующем файле
    int outputFd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Записываем и выводим результаты подсчета
    for (int i = 0; i < 26; i++) {
        if (letterCount[i] > 0) {
            // Формируем строку вида "a - 122\n"
            char result[50];
            snprintf(result, sizeof(result), "%c - %d\n", 'a' + i, letterCount[i]);
            // где "%c - %d\n" форматная строка, 'a' + i вычисление символа,
            // letterCount[i] кол-во элемента

            // Записываем строку в файл
            write(outputFd, result, strlen(result));

            // Выводим в терминал
            cout << result;
        }
    }

    // Закрываем выходной файл
    close(outputFd);

    cout << "Результаты подсчета записаны в файл: " << outputFile << endl;

    return 0;
}
