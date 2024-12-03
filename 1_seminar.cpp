#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

int main () {
  setlocale(LC_ALL,"Rus");
  for (int i = 0; i < 5; i++){
    if (fork()){
      int status = 0;
      wait(&status);
      if (i == 0)
        wait(NULL);
        cout << "Папа 0 " << getpid() << " " << getppid() << "\n";
      }
    else{
      sleep(1 + i);
      cout << "Дочь " << i+1 << " " << getpid() << " " << getppid() << "\n";
      return 0;
      }
    }
  
return 0;
}

/* 
 * вывод дочерний
 * дочерний
 * дочерний
 * папа
 *
 *
 * при запуске последовательно всемя будет возрастать по i + 1
 * задача запустить одновременно
 * */
