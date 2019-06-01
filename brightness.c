/*
 * Simple utility to control brightness on linux systems.
 *
 * Copyright (c) 2019 Gaurav Kumar Yadav <gaurav712@protonmail.com>
 * for license and copyright information, see the LICENSE file distributed with this source
 *
 * Specially for wayland window managers like sway, velox, bspwc .....
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define BCKLTDIR "/sys/class/backlight/" /* Backlight directory */

/* Explains itself */
static void showHelp(void);
/* Detect brightness controller(simply said, folder name in BCKLTDIR) */
static void findController(DIR *dirp, struct dirent *dir, char *controller);
/* Get brightness value(what to write in /sys/..../brightness) */
static int getBrtValue(char *path, int level);
/* Set the brightness to "level" */
static void setBrt(char *path, int value, char mode);
/* Get current brightness */
static int getCurBrt(char *path);

static FILE *fp;

int main(int argc, char *argv[]) {

    DIR *dirp;
    struct dirent *dir = NULL;
    char controller[30], path[PATH_MAX];
    int value;

    if(argc != 3) {
        showHelp();
        exit(1);
    }

    if((dirp = opendir(BCKLTDIR)) == NULL) {
        perror("opendir() failed!");
        exit(1);
    }

    findController(dirp, dir, controller);

    printf("found controller %s\n", controller);

    /* Setup path */
    strcpy(path, BCKLTDIR);
    strcat(path, controller);

    /* Parse options */
    if(!(strcmp(argv[1], "set"))) {
        value = getBrtValue(path, atoi(argv[2]));
        setBrt(path, value, ' ');
    } else if(!(strcmp(argv[1], "inc"))) {
        value = getBrtValue(path, atoi(argv[2]));
        setBrt(path, value, '+');
    } else if(!(strcmp(argv[1], "dec"))) {
        value = getBrtValue(path, atoi(argv[2]));
        printf("value is %d\n", value);
        setBrt(path, value, '-');
    } else {
        printf("Unknown option %s\n", argv[1]);
        showHelp();
        exit(1);
    }

    return 0;
}

void
showHelp(void) {

    printf("\nUSAGE :\nbrightness [set|inc|dec] [value]\nNOTE : value is \"percentage\" brightness but don't append %%\n\n");
}

void
findController(DIR *dirp, struct dirent *dir, char *controller) {

    while(1) {
        if((dir = readdir(dirp)) == NULL) {
            perror("couldn't detect the backlight controller!");
            exit(1);
        }

        if(dir->d_name[0] == '.')
            continue;
        else {
            strcpy(controller, dir->d_name);
            break;
        }
    }
}

int
getBrtValue(char *path, int level) {

    unsigned brtMax;
    char pathBak[PATH_MAX];
    strcpy(pathBak, path); /* Just to keep "path" safe */

    strcat(pathBak, "/max_brightness");
    printf("path is %s\n", pathBak);
    if((fp = fopen(pathBak, "r")) == NULL) {
        perror("couldn't access /sys/../max_brightness");
        exit(1);
    }

    fscanf(fp, "%u", &brtMax);

    fclose(fp);
    return ((int)((float)brtMax * level/100));
}

void
setBrt(char *path, int value, char mode) {

    unsigned prevValue;

    strcat(path, "/brightness");

    prevValue = getCurBrt(path);

    if((fp = fopen(path, "w")) == NULL) {
        perror("couldn't access /sys/../brightness");
        exit(1);
    }

    if(mode == '+')
        fprintf(fp, "%d", (value + prevValue));
    else if(mode == '-')
        fprintf(fp, "%d", (prevValue - value));
    else
        fprintf(fp, "%d", value);

    fclose(fp);
}

int
getCurBrt(char *path) {

    int curBrt;

    if((fp = fopen(path, "r")) == NULL) {
        perror("couldn't access /sys/../brightness");
        exit(1);
    }

    fscanf(fp, "%d", &curBrt);

    return curBrt;
}

