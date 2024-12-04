#include <iostream>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <clocale>

using namespace std;

int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "ru_RU.UTF-8");

    // Получаем имя файла из командной строки
    const char *filename = argv[1];

    // Структура для хранения информации о файле
    struct stat fileStat;

    // Вызываем stat, чтобы получить информацию о файле
    if (stat(filename, &fileStat) < 0) {
        perror("Ошибка при вызове stat");
        return 1;
    }

    // Определяем, является ли это обычным файлом или
    // каталогом через макросы из справочной информации
    if (S_ISREG(fileStat.st_mode)) {
        cout << "Файл '" << filename << "' является обычным файлом\n";
    } else if (S_ISDIR(fileStat.st_mode)) {
        cout << "Файл '" << filename << "' является папкой\n";
    } else {
        cout << "Файл '" << filename << "' имеет неизвестный тип\n";
    }

    // Выводим права доступа в виде 8-битного числа и в виде строки
    // через статусную информацию из справочной информации
    cout << "Права доступа = " << oct << fileStat.st_mode << " ("
         << (fileStat.st_mode & S_IRUSR ? "r" : "-")
         << (fileStat.st_mode & S_IWUSR ? "w" : "-")
         << (fileStat.st_mode & S_IXUSR ? "x" : "-")
         << (fileStat.st_mode & S_IRGRP ? "r" : "-")
         << (fileStat.st_mode & S_IWGRP ? "w" : "-")
         << (fileStat.st_mode & S_IXGRP ? "x" : "-")
         << (fileStat.st_mode & S_IROTH ? "r" : "-")
         << (fileStat.st_mode & S_IWOTH ? "w" : "-")
         << (fileStat.st_mode & S_IXOTH ? "x" : "-")
         << ")\n";

    // Получаем информацию о пользователе и группе
    struct passwd *pw = getpwuid(fileStat.st_uid);
    struct group *gr = getgrgid(fileStat.st_gid);

    // Если пользователь и группа найдены, выводим их id
    if (pw) {
        cout << "UID = " << fileStat.st_uid << " (" << pw->pw_name << ")\n";
    } else {
        cout << "UID = " << fileStat.st_uid << " (неизвестный)\n";
    }

    if (gr) {
        cout << "GID = " << fileStat.st_gid << " (" << gr->gr_name << ")\n";
    } else {
        cout << "GID = " << fileStat.st_gid << " (неизвестный)\n";
    }

    // Размер файла
    cout << "Размер файла = " << fileStat.st_size << " байт\n";

    // Преобразуем время последней модификации в удобочитаемый формат
    cout << "Время последнего открытия файла (atime) = " << ctime(&fileStat.st_atime);
    cout << "Время последнего изменения файла (mtime) = " << ctime(&fileStat.st_mtime);

    return 0;
}

