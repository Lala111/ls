#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

struct stat fileStat;
int options[255];
char *filename = NULL;

int sortdesc(const struct dirent **d1, const struct dirent **d2) {
	return alphasort(d2, d1);
}

int sorttime(const struct dirent **d1, const struct dirent **d2) {
	time_t t1, t2;
	struct stat buff1, buff2;
	stat((*d1)->d_name, &buff1);
	stat((*d2)->d_name, &buff2);

	if (options['c'] == 0 && options['u'] == 0)
		t1 = buff1.st_mtime, t2 = buff2.st_mtime;
	if (options['c'] == 1)
		t1 = buff1.st_ctime, t2 = buff2.st_ctime;
	if (options['u'] == 1)
		t1 = buff1.st_atime, t2 = buff2.st_atime;

	if (t1 == t2) {
		return strcasecmp((*d1)->d_name, (*d2)->d_name);
	}
	if (t1 != t2) {
		if (options['r'] == 0 && t1 < t2)
			return 1;
		if (options['r'] == 1 && t1 > t2)
			return 1;
		if (options['r'] == 0 && t1 > t2)
			return -1;
		if (options['r'] == 1 && t1 < t2)
			return -1;
	}
}

int sortsize(const struct dirent **d1, const struct dirent **d2) {
	struct stat buff1, buff2;
	lstat((*d1)->d_name, &buff1);
	lstat((*d2)->d_name, &buff2);

	if (buff1.st_size == buff2.st_size) {
		return (strcasecmp((*d1)->d_name, (*d2)->d_name));
	}
	if (buff1.st_size < buff2.st_size) {
		if (options['r'] == 0 && buff1.st_size < buff2.st_size)
			return 1;
		if (options['r'] == 1 && buff1.st_size > buff2.st_size)
			return 1;
		if (options['r'] == 0 && buff1.st_size > buff2.st_size)
			return -1;
		if (options['r'] == 1 && buff1.st_size < buff2.st_size)
			return -1;
	}
}
void getColor(int mode, char* dir_name) {
	if (options['f'] != 1 && options['F'] == 1) {
		if ((S_ISDIR(mode)))
			printf("\033[1m\033[34m" "%s" "\033[0m" "/", dir_name);
		else if ((access(dir_name, X_OK) != -1))
			printf("\033[1m\033[32m" "%s" "\033[0m" "*", dir_name);
		else if (S_ISSOCK(mode))
			printf("\033[1m\033[35m" "%s" "\033[0m" "=", dir_name);
		else if (S_ISLNK(mode))
			printf("\033[1m\033[36m" "%s" "\033[0m" "@", dir_name);
		else if (S_ISFIFO(mode))
			printf("%s" "|", dir_name);
		else if (S_ISREG(mode))
			printf("%s", dir_name);
	}
	if (options['f'] == 1)
		printf("%s", dir_name);

	if (options['f'] != 1 && options['F'] != 1) {
		if ((S_ISDIR(mode)))
			printf("\033[1m\033[34m" "%s" "\033[0m", dir_name);
		else if ((access(dir_name, X_OK) != -1))
			printf("\033[1m\033[32m" "%s" "\033[0m", dir_name);
		else if (S_ISSOCK(mode))
			printf("\033[1m\033[35m" "%s" "\033[0m", dir_name);
		else if (S_ISLNK(mode))
			printf("\033[1m\033[36m" "%s" "\033[0m", dir_name);
		else if (S_ISFIFO(mode))
			printf("%s", dir_name);
		else if (S_ISREG(mode))
			printf("%s", dir_name);
	}
}
void myls(char* file[], int n) {
	int i;
	long int total = 0;
	char f[255];
	struct passwd *pw;
	struct group *gp;
	if (options['R'] == 1)
		printf("%s;\n", filename);
	for (i = 0; i < n; i++) {
		lstat(file[i], &fileStat);

		if (((options['A'] == 1 || options['t'] || options['S'] || options['r']
				|| options['c'] || options['h'] || options['i'] || options['l']
				|| options['n'] || options['u'] || options['R'])
				&& options['a'] == 0)
				&& (strcmp(file[i], ".") == 0 || strcmp(file[i], "..") == 0
						|| strstr(file[i], ".") == file[i])) {
		} else {
			pw = getpwuid(fileStat.st_uid);
			gp = getgrgid(fileStat.st_gid);
			char *c;
			if ((options['l'] == 0 && options['n'] == 0 && options['i'] == 0)) {
				getColor(fileStat.st_mode, file[i]);
				printf(" \n");
			}
			if (options['i'] == 1 && options['l'] == 0) {
				printf("%ld ", fileStat.st_ino);
				getColor(fileStat.st_mode, file[i]);
				printf("\n");
			}

			if (options['f'] == 1 && options['a'] == 0) {
				getColor(fileStat.st_mode, file[i]);
				printf("\n");
			}
			if (options['R']) {
				if ((S_ISDIR(fileStat.st_mode)))
					strcat(f, file[i]);
			}

			if ((options['l'] == 1 || options['n'] == 1) && options['f'] == 0) {
				if (options['i'] == 1) {
					printf("%ld ", fileStat.st_ino);
				}

				printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
				printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
				printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
				printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
				printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
				printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
				printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
				printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
				printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
				printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
				printf("%2lx", fileStat.st_nlink);
				if (options['l'] == 1) {
					printf(" %4s %4s ", pw->pw_name, gp->gr_name);
				} else if (options['n'] == 1) {
					printf(" %d %d ", fileStat.st_uid, fileStat.st_gid);
				}
				if (options['h'] != 1)
					printf("%5ld ", fileStat.st_size);
				if (options['h'] == 1) {
					if ((fileStat.st_size >= 1000)
							&& (fileStat.st_size < 10000)) {
						int a = fileStat.st_size / 100;
						int b = a % 10;
						int c = fileStat.st_size / 1000;
						printf("%2d,%dK ", c, b);
					}
					if (fileStat.st_size >= 10000) {
						int a = fileStat.st_size / 1000;
						int b = fileStat.st_size / 100;
						int c = b % 10;
						if (c >= 5) {
							a++;
						}
						printf("%4.dK ", a);
					}
					if (fileStat.st_size < 1000) {
						printf("%5ld ", fileStat.st_size);

					}

				}

				c = ctime(&fileStat.st_mtime);
				printf("%c%c%c%c%c%c%c%c%c%c%c%c ", c[4], c[5], c[6], c[7],
						c[8], c[9], c[10], c[11], c[12], c[13], c[14], c[15]);

				getColor(fileStat.st_mode, file[i]);
				printf("\n");
				total += fileStat.st_blocks;
			}
		}
	}
	if ((options['l'] == 1 || options['n'] == 1) && (options['f'] == 0))
		printf("Total: %ld\n", total / 2);
	printf("\n");
	if (options['R']) {
		printf("./%s:\n", f);
		int totalR = 0;
		DIR *mydir = opendir(f);
		struct dirent *myfile;
		struct stat fileStat;
		char* c;
		while ((myfile = readdir(mydir)) != NULL) {
			stat(myfile->d_name, &fileStat);
			if (strcmp(myfile->d_name, ".") == 0
					|| strcmp(myfile->d_name, "..") == 0) {
			} else {
				if (options['l'] || options['n']) {
					printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
					printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
					printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
					printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
					printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
					printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
					printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
					printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
					printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
					printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
					printf(" ");
					printf("%lx ", fileStat.st_nlink);
					pw = getpwuid(fileStat.st_uid);
					printf("%s ", pw->pw_name);
					gp = getgrgid(fileStat.st_gid);
					printf("%s ", gp->gr_name);
					if (options['h'] != 1)
						printf("%5ld ", fileStat.st_size);
					if (options['h'] == 1) {
						if ((fileStat.st_size >= 1000)
								&& (fileStat.st_size < 10000)) {
							int a = fileStat.st_size / 100;
							int b = a % 10;
							int c = fileStat.st_size / 1000;
							printf("%2d,%dK ", c, b);
						}
						if (fileStat.st_size >= 10000) {
							int a = fileStat.st_size / 1000;
							int b = fileStat.st_size / 100;
							int c = b % 10;
							if (c >= 5) {
								a++;
							}
							printf("%4.dK ", a);
						}
						if (fileStat.st_size < 1000) {
							printf("%5ld ", fileStat.st_size);

						}
					}
					c = ctime(&fileStat.st_mtime);
					for (i = 4; i <= 15; i++)
						printf("%c", c[i]);
					printf(" ");
				}
				printf("%s\n", myfile->d_name);
			}
		}
		totalR += fileStat.st_blocks;
		printf("Total: %d\n", totalR / 2);
	}
}

int main(int argc, char **argv) {
	struct dirent **namelist;
	int i, n;
	char* file[255];
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (int j = 0; j < strlen(argv[i]); j++)
				options[argv[i][j]] = 1;
		} else
			filename = argv[i];
	}

	if (filename == NULL)
		filename = ".";
	lstat(filename, &fileStat);
	if (options['a'] || options['A'] || options['n'] || options['i']
			|| options['i'] || options['r'] || options['t'] || options['c']
			|| options['u'] || options['S'] || options['f'] || options['l']
			|| options['F'] || options['h'] || options['R']) {
		if (options['A'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['a'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['c'])
			n = scandir(filename, &namelist, NULL, sorttime);
		if (options['F'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['f'])
			n = scandir(filename, &namelist, NULL, NULL);
		if (options['h'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['i'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['l'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['n'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['R'])
			n = scandir(filename, &namelist, NULL, alphasort);
		if (options['r'])
			n = scandir(filename, &namelist, NULL, sortdesc);
		if (options['S'])
			n = scandir(filename, &namelist, NULL, sortsize);
		if (options['t'])
			n = scandir(filename, &namelist, NULL, sorttime);
		if (options['u'])
			n = scandir(filename, &namelist, NULL, sorttime);

		for (i = 0; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	}
	if (options['d'] == 1) {
		file[0] = malloc(sizeof(filename));
		strcpy(file[0], filename);
		n = 1;
	} else if (options['A'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['S'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['t'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['r'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['c'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['h'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['l'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['i'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['n'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['u'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	} else if (options['R'] == 1) {
		for (i = 2; i < n; i++) {
			file[i] = malloc(sizeof(namelist[i]->d_name));
			strcpy(file[i], namelist[i]->d_name);
		}
	}
	myls(file, n);

	return 0;
}

