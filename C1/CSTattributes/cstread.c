#include<stdio.h>
#include<string.h>

void chomp(char str[]);

void clearCST(void);

void readCST(void);

void printAll(char *SDID, char *TDID, int SSENT, int TSENT, char *TYPE);

int main (void) {

	//clearCST();
	readCST();

	return 0;
}

void chomp(char str[]) {
	int i;

	for(i = 0 ; str[i] != '\n'; i++);
	str[i] = '\0';
}

void clearCST(void) {
    int i, position, c, countLine;
    FILE *fp, *fd;
    char empty[] = "\n", oldname[100], line[1000];

    fp = fopen("D2_D3_C1L.cst", "r+");
    fd = fopen("newfile.txt", "w");

    strcpy(oldname, "D1_D2_C1.cst");

	countLine = 0;
	c = fgetc(fp);
	fseek(fp, 0, SEEK_SET);
	while (c != EOF) {  // (line != NULL) && (!feof(fp)))
		if (countLine != 0) {	
			position = ftell(fp);
			position--;
			fseek(fp, position, SEEK_SET);
		}
		fgets(line, 1000, fp);
		chomp(line);

		if (strcmp(line, empty) != 0) {
			//puts(line);
			fputs(line, fd);
		}
		countLine++;
		c = fgetc(fp);
	}

    remove("D1_D2_C1.cst");
    rename("newfile.txt", oldname);
    //strcpy("D1_D2_C1.cst", oldname);

    fclose(fp);
    fclose(fd);

    printf("final clear\n");
}

void readCST(void) {
	int SSENT, TSENT, begin, i, j, count, countLine, c, position;
	char SDID[100], TDID[100], TYPE[100], line[1000], str[100];
	char empty[] = "</R>";
	FILE *fp;

	fp = fopen("D2_D3_C1L.cst", "r");

	countLine = 0;
	c = fgetc(fp);
	fseek(fp, 0, SEEK_SET);
	while (c != EOF) {  // (line != NULL) && (!feof(fp)))
		if (countLine != 0) {	
			position = ftell(fp);
			position--;
			fseek(fp, position, SEEK_SET);
		}
		fgets(line, 1000, fp);
		chomp(line);
		//puts(line);

		if (strcmp(line, empty) != 0) {
			//puts(line);

			begin = 0; count = 0;
			for (i = 0; i < strlen(line); i++) {
//printf("b %d\n", begin);
				if (line[i] == '"') {
					if (begin == 0) {
						begin = 1;
						i++;
						j = 0;
					}
					else if (begin == 1) {
						//printf("count %d\n", count);
						begin = 0;
						str[j] = '\0';
						//puts(str);
						if (countLine % 2 == 0) {
							if (count == 0) {
								strcpy(SDID, str);
								//puts(SDID);
								count++;
							}
							else if (count == 1) {
								SSENT = atoi(str);
								//printf("SSENT %d\n", SSENT);
								count++;
							}
							else if (count == 2) {
								strcpy(TDID, str);
								//puts(TDID);
								count++;
							}
							else if (count == 3){
								TSENT = atoi(str);
								//printf("TSENT %d\n", TSENT);
								count++;
							}
						}
						else {
							if (count == 0) {
								strcpy(TYPE, str);
								//puts(TYPE);
								count++;
								if (strcmp(TYPE, "Identity") == 0) {
									printAll(SDID, TDID, SSENT, TSENT, TYPE);
								}
								else if (strcmp(TYPE, "Equivalence") == 0) {
									printAll(SDID, TDID, SSENT, TSENT, TYPE);
								}
								else if (strcmp(TYPE, "Summary") == 0) {
									printAll(SDID, TDID, SSENT, TSENT, TYPE);
								}
								else if (strcmp(TYPE, "Subsumption") == 0) {
									printAll(SDID, TDID, SSENT, TSENT, TYPE);
								}
								else if (strcmp(TYPE, "Overlap") == 0) {
									printAll(SDID, TDID, SSENT, TSENT, TYPE);
								}
							}
						}
					}
				}
				if (begin == 1) {
					str[j] = line[i];
					j++;
				}
			}
			countLine++;

			/*pch = strtok (line,""");
			while (pch != NULL) {
				printf ("%s\n",pch);
				pch = strtok (NULL, """);
			}*/
		}
		c = fgetc(fp);
	}

	fclose(fp);
}

void printAll(char *SDID, char *TDID, int SSENT, int TSENT, char *TYPE) {

	printf("SDID = %s\n", SDID);
	printf("TDID = %s\n", TDID);
	printf("SSENT = %d\n", SSENT);
	printf("TSENT = %d\n", TSENT);
	printf("TYPE = %s\n", TYPE);

}
