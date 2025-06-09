#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: fileman <ls|cat> [path]\n");
        return 1;
    }
    if (strcmp(argv[1], "ls") == 0) {
        const char *path = argc > 2 ? argv[2] : ".";
        DIR *d = opendir(path);
        if (!d) { perror("opendir"); return 1; }
        struct dirent *de;
        while ((de = readdir(d)))
            printf("%s\n", de->d_name);
        closedir(d);
    } else if (strcmp(argv[1], "cat") == 0) {
        if (argc < 3) { printf("usage: fileman cat <file>\n"); return 1; }
        FILE *f = fopen(argv[2], "r");
        if (!f) { perror("fopen"); return 1; }
        char buf[128];
        while (fgets(buf, sizeof(buf), f))
            fputs(buf, stdout);
        fclose(f);
    } else {
        printf("unknown command %s\n", argv[1]);
        return 1;
    }
    return 0;
}
