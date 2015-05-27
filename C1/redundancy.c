#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

//#include "text.h"

void setStandardRedundancy(File *F, int nDocs, int higherRedundancy) {
	int i;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
	        while (auxs != NULL) {
	            auxs->standardRedundancy = (float) auxs->redundancy / (float) higherRedundancy;
				printf("sr %f\n", auxs->standardRedundancy); // printf
	        	auxs = auxs->nextSent;
			}
		puts(""); // puts \n
		//print(auxt->firstSent);
		auxt = auxt->nextTxt;
	}

}

void findMaxRedundancy(File *F, int nDocs) {
	int i, higherRedundancy = INT_MIN;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			if (auxs->redundancy > higherRedundancy) {
				higherRedundancy = auxs->redundancy;
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}
	printf("higherRedundancy %d\n", higherRedundancy); // printf
	setStandardRedundancy(F, nDocs, higherRedundancy);

}

void setCST(File *F, int nDocs, char *SDID, char *TDID, int SSENT, int TSENT, char *TYPE) {
	int i, j;
	word *auxw;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt; 

	for (i = 0; i < nDocs ; i++) {  
		if (strcmp(auxt->DocName, SDID) == 0) {
			auxs = auxt->firstSent;
			for (j = 0; j != SSENT - 1; j++) { // - 1 por comecar em zero
				auxs = auxs->nextSent;
			}
			auxs->redundancy++;
			puts(SDID);
			printf("for0 j %d SSENT %d r %d .\n", j, SSENT, auxs->redundancy);
		}
		auxt = auxt->nextTxt;
	}

	auxt = F->firstTxt; 

	for (i = 0; i < nDocs; i++) {
		if (strcmp(auxt->DocName, TDID) == 0) {
			auxs = auxt->firstSent;
			for (j = 0; j != TSENT - 1; j++) { // - 1 por comecar em zero
				auxs = auxs->nextSent;
			}
			auxs->redundancy++;
			puts(TDID);
			printf("for1 j %d TSENT %d r %d .\n", j, TSENT, auxs->redundancy);
		}
		auxt = auxt->nextTxt;
	}
}

void readCST(File *F, int nDocs) {
    int SSENT, TSENT, begin, i, j, w, k, count, countLine, c, position, nCst;
    char *pch, SDID[100], TDID[100], TYPE[100], line[1000], str[100], CSTName[100];
    FILE *fp;

	puts("Numero de arquivos CST: ");
	scanf("%d", &nCst);
	//printf("nCst %d\n", nCst);

	//puts("Nome do arquivo: ");
	fgets(CSTName, 100, stdin);		// para limpar o buffer do stdin
	chomp(CSTName);
	//puts(CSTName);

	for (k = 0; k < nCst; k++) {
		puts("Nome do arquivo: ");
		fgets(CSTName, 100, stdin);
		chomp(CSTName);
		//puts(CSTName);
		puts("");

		fp = fopen(CSTName, "r");

	    countLine = 0;
	    fgets(line, 1000, fp);
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

	        if (strcmp(line, "</R>") != 0) {
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
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Equivalence") == 0) {
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Summary") == 0) {
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Subsumption") == 0) {
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Overlap") == 0) {
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
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
	        }
	        c = fgetc(fp);
	    }

		fclose(fp);
		printf("\n");
	}
	printf("\n");
	findMaxRedundancy(F, nDocs);		// call to set CST attribute function
	printText(F->firstTxt);
}
