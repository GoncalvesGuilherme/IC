#include <stdio.h>

void chomp(char *str);

void readFile(char *docName);

int main (void) {
	char docName[50];

	fgets(docName, 50, stdin);
	chomp(docName);

	readFile(docName);

	return 0;
}

void chomp(char str[]) {
    int i;

    for(i = 0; str[i] != '\n'; i++);
    str[i] = '\0';
}

void readFile(char *docName) {
	int countLine, position, c;
	char *pch, line[1000];
	FILE *fp;

	fp = fopen(docName, "r");

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
		puts(line);

		//pch = strtok (str,".");
		//while (pch != NULL) {
		//	printf ("%s\n",pch);
		//	pch = strtok (NULL, ".");
		//}
		countLine++;
		c = fgetc(fp);
	}
}
