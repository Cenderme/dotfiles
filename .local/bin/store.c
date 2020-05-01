#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: store [s(end)/r(ecieve)] files\n");
    exit(EXIT_FAILURE);
  }
  char result[1000] = {"rsync -Pav --delete "};
  char spc[2];
  if (strcmp(argv[1], "s") == 0) {
    for (int i = 2; i < argc; i++) {
      if (access(argv[i], F_OK) == -1) {
        printf("Error: File %s doesn't exist.\n", argv[i]);
        exit(EXIT_FAILURE);
      } else {
        strcat(result, argv[i]);
        strcat(result, " ");
      }
    }
    strcat(result, "soyluoglu16@ssh.itu.edu.tr:~/storage/");
    system(result);
  } else if (strcmp(argv[1], "r") == 0) {
    strcat(result, "soyluoglu16@ssh.itu.edu.tr");
    for (int i = 2; i < argc; i++) {
        strcat(result, ":~/storage/");
        strcat(result, argv[i]);
        strcat(result, " ");
    }
    strcat(result, "~/storage/");
    system(result);
  } else {
    printf("Usage: store [s(end)/r(ecieve)] files\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}
