#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "text.h"

#define CREATES(ptr) sentence *ptr = (sentence *) malloc (sizeof(sentence))
#define CREATEW(ptr) word *ptr = (word *) malloc (sizeof(word))
#define CREATED(ptr) text *ptr = (text *) malloc (sizeof(text))
#define CREATEFB(ptr) frequencyBlock *ptr = (frequencyBlock *) malloc (sizeof(frequencyBlock)) 
#define CREATECSTDOC(ptr) cstName *ptr = (cstName *) malloc (sizeof(cstName)) 
#define CREATECR(ptr) cstRelation *ptr = (cstRelation *) malloc (sizeof(cstRelation)) 
#define CHECK(ptr) if (ptr == NULL) { return OUT_OF_MEMORY; }

void chomp(char str[]) {
    int i;

    for(i = 0 ; str[i] != '\n'; i++);
    str[i] = '\0';
}

int createFile(File *F, int nDocs) {

	F = (File *) malloc (sizeof(File));
	if (F == NULL) {
		return OUT_OF_MEMORY;
	}

	F->nDocs = nDocs;
	F->firstTxt = NULL;
	F->FTfirstword = NULL;
	F->ranking = NULL;
//	F->firstDoc = NULL;
	return SUCCESS;
}

int createDoc(File *F, int nDocs) {
	int i = 0;
	char DocName[100];
	text *newTxt, *auxTxt;

	F->firstTxt = NULL;
	F->FTfirstword = NULL;

	//puts("Nome do arquivo: ");
	fgets(DocName, 100, stdin);		// para limpar o buffer do stdin
	chomp(DocName);
	//puts(CSTName);

	for (i = 0; i < nDocs; i++) {
		CREATED(newTxt);
		if (newTxt == NULL) {
			return OUT_OF_MEMORY;
		}

		newTxt->nextTxt = NULL;
		newTxt->nDoc = i;
//		newTxt->firstCstDoc = NULL; //POINT to cst doc is here
		puts("Digite o nome do documento: ");
		fgets(DocName, 100, stdin);	// a fgets nao esta lendo o primeiro nome Segmentation fault (core dumped)
		chomp(DocName);
		strcpy(newTxt->DocName, DocName);
		
		if (F->firstTxt == NULL) {
			F->firstTxt = newTxt;
		}
		else {
			auxTxt = F->firstTxt;
			while (auxTxt != NULL && auxTxt->nextTxt != NULL) {
				auxTxt = auxTxt->nextTxt;
			}
			auxTxt->nextTxt = newTxt;
		}
		newTxt = NULL;
	}
	return SUCCESS;
}

void callPL(File *F, int nDocs) {
	int lenght, i, j, w, find;
	char docName[100], docNamepl[100];
	text *auxt;

	auxt = F->firstTxt;

	for (j = 0; j < nDocs; j++) {
		find = 0;
		char cmd[1000] = "cat ";

		strcpy(docName, auxt->DocName);
	    strcat(cmd, docName);
	    strcat(cmd, " | /opt/palavras/por.pl 2>/dev/null > ");

	    strcpy(docNamepl, docName);

	    lenght = strlen(docNamepl);
	    for (i = 0; find == 0; i++) {
	        if (docNamepl[i] == '.') {
	    	    find = 1;
		    }   
	    }
		i--;
        w = i;
        docNamepl[i] = 'p';
        i++;
        docNamepl[i] = 'l';
        i++;
        for (w; w < lenght; w++) {
        	docNamepl[i] = docName[w];
        	i++;
        }
        docNamepl[i] = '\0';

		strcpy(auxt->DocNamepl, docNamepl);

	    strcat(cmd, docNamepl);
        puts(cmd);

		system(cmd);
		auxt = auxt->nextTxt;

		memset(cmd, 0, strlen(cmd));
	}
	printf("end of cp\n");
}

int clear(File *F, int nDocs) {
	int i;
	FILE *fp, *fd;
	char empty[] = "\n", oldname[100], line[1000];
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		fp = fopen(auxt->DocNamepl, "r+");
		fd = fopen("newfile.txt", "w");

		strcpy(oldname, auxt->DocNamepl);

		while (!feof(fp) || strcmp(line, empty)) {
			fgets(line, 1000, fp);

			if (strcmp(line, empty)) {
				//chomp(line);
				fputs(line, fd);
			}
		}
		remove(auxt->DocNamepl);
		rename("newfile.txt", oldname);
		strcpy(auxt->DocNamepl, oldname);

		fclose(fp);
		fclose(fd);

		printf("final clear\n");

		separate(auxt);

		auxt = auxt->nextTxt;	
	}
	return SUCCESS;
}

int separate(text *txt) {
	FILE *fp, *fd;
	char line[1000], *pch, palavra[50], auxpal[50], lema[50], classe_gramat[50]; 
	int get, j, w, k, i = 0, find, plenght;
	sentence *auxS, *sent;
	word *auxW, *wrd;
	char endSent[] = "</s>";

	fp = fopen(txt->DocNamepl, "r");

	while ((line != NULL) && (!feof(fp))) {
		fgets(line, 1000, fp);
		chomp(line);
		if (strcmp(line,endSent)) {
			CREATES(sent);
			// create a new sentence's block
			if (sent == NULL) {
				return OUT_OF_MEMORY;
			}
			sent->nro_sent = i;
			sent->firstWord = NULL;
			sent->nextSent = NULL;
			sent->location = 0;
			sent->frequency = 0;
			sent->standardFrequency = 0;
			sent->size = 0;
			sent->standardSize = 0;
			sent->redundancy = 0;
			sent->standardRedundancy = 0;
			sent->firstcr == NULL; // first cstRelation
			strcpy(sent->docName, txt->DocName);
			puts(sent->docName);

			// chains the new sentence
			if (txt->firstSent == NULL) {
				txt->firstSent = sent;
			}
			else {
				auxS = txt->firstSent;
				while ((auxS != NULL) && (auxS->nextSent != NULL)) {
					auxS = auxS->nextSent;
				}
				auxS->nextSent = sent;
			}

			// reading a sentence, separating and chains the words
			while ((line != NULL) && (strcmp(line, endSent))) {
				// get the word
				pch = strtok (line, " "); 
				if (pch != NULL) { 
					if (pch[0] == '$') { // for accentuation
		                pch[0] = pch[1];
		                pch[1] = '\0';
					}
					strcpy(palavra, pch);
				//	puts(palavra);		// puts
				}
				get = 0; //get the lema
				while ((pch != NULL) && (get == 0)) {;
					if (pch[0] == '[') {
						w = 0;
						for (j = 1; j < strlen(pch) - 1; j++) {
							lema[w] = pch[j]; 
							w++;
						}
						lema[w] = '\0';
	                	get = 1;
					}
					pch = strtok (NULL, " \t");
				}

				// reading the classe gramatical
				get = 0;
				while ((pch != NULL) && (get == 0)) {
					if (pch[0] == '<') {
					}
					else {
						strcpy(classe_gramat, pch);
						get = 1;
						break;
					}
					pch = strtok (NULL, " ");
				}

				find = 1;
				for (j = 0; j < strlen(palavra); j++) {
					if (palavra[j] == '=') {
						find = 0;
						break;
					}
				}
				//printf("f %d\n", find);		// puts

				if (find == 1) {
					// creating the new word's block
					CREATEW(wrd);
					if (wrd == NULL) {
						return OUT_OF_MEMORY;
					}
					wrd->nextWord = NULL;
					strcpy(wrd->palavra, palavra);
					strcpy(wrd->lema, lema);
					strcpy(wrd->classe_gramat, classe_gramat);

					// chains the new word
					if (sent->firstWord == NULL) {
						sent->firstWord = wrd;
					}
					else {
						auxW = sent->firstWord;
						while ((auxW != NULL) && (auxW->nextWord != NULL)) {
							auxW = auxW->nextWord;
						}
						auxW->nextWord = wrd;
					}
					wrd = NULL;
				}
				else {
//puts("find = 0.");
					plenght = strlen(palavra);
					j = 0;
					while (j < plenght) {
						w = 0;
						for (j; j < plenght && palavra[j] != '='; j++) {
							auxpal[w] = palavra[j];
							w++;
						}
						auxpal[w] = '\0';
						j++;		// para sair do palavra[j] == '='
//puts(auxpal);

						// creating the new word's block
						CREATEW(wrd);
						if (wrd == NULL) {
							return OUT_OF_MEMORY;
						}
						wrd->nextWord = NULL;
						strcpy(wrd->palavra, auxpal);
						strcpy(wrd->lema, lema);
						strcpy(wrd->classe_gramat, classe_gramat);

						// chains the new word
						if (sent->firstWord == NULL) {
							sent->firstWord = wrd;
						}
						else {
							auxW = sent->firstWord;
							while ((auxW != NULL) && (auxW->nextWord != NULL)) {
								auxW = auxW->nextWord;
							}
							auxW->nextWord = wrd;
						}
						memset(auxpal, 0, strlen(auxpal));
						wrd = NULL;
					}
				}
				fgets(line, 1000, fp);
				chomp(line);
			}
		}
		i++;
		sent = NULL;
	}

	fclose(fp);

	//print(txt->firstSent);
	return SUCCESS;
}

int printWords(word *wrd) {

	if (wrd == NULL) {
		return 0;
	}
	printf("%s ", wrd->palavra);
	printWords(wrd->nextWord);
}

int print(sentence *sent) {

	if (sent == NULL) {
		return 0;
	}
	printf("Sentenca %d: \n", sent->nro_sent);
	switch (sent->location) {
		case BEGIN:
			puts("Localizacao = comeco.");
			break;
		case MIDDLE:
			puts("Localicao = meio.");
			break;
		case END:
			puts("Localizacao = fim");
			break;
	}

	printf("Tamanho = %d .\n", sent->size);
	printf("Tamanho normalizado = %f .\n", sent->standardSize);

	printf("Frequencia = %d .\n", sent->frequency);
	printf("Frequencia normalizada = %f .\n", sent->standardFrequency);

    printf("Redundancy = %d .\n", sent->redundancy);
	printf("Redundancia normalizada = %f .\n", sent->standardRedundancy);

	printf("\t");
	printf("%s\n", sent->sentenca);
	//printWords(sent->firstWord);
	printf("\n\n");
	print(sent->nextSent);
}

int printText(text *txt) {

	if (txt == NULL) {
		return 0;
	}
	puts(txt->DocName);
	print(txt->firstSent);
	printText(txt->nextTxt);

}

void location(File *F, int nDocs) {
	int i;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			if (auxs->nro_sent == 0) {
				auxs->location = BEGIN;
			}
			else if (auxs->nextSent == NULL) {
				auxs->location = END;
			}
			else {
				auxs->location = MIDDLE;
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}
}

void settolower(char *source, char *dest) {
	int i;

	for (i = 0; i < strlen(source); i++) {
		dest[i] = tolower(source[i]);
	}
	dest[i] = '\0';
}

void size(File *F, int nDocs) {
	int i, j, find;
	FILE *fp;
	char stopword[100], palavra[100];
	word *auxw;
	sentence *auxs;
	text *auxt;

	fp = fopen("stoplist_pt.txt", "r");

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		auxs->size = 0; // para assegurar que inicialmente estarah setada com zero
		while (auxs != NULL) {
			auxw = auxs->firstWord;
			while (auxw != NULL) {
				if (isalnum(auxw->palavra[0]) != 0) {
					find = 0;
					settolower(auxw->palavra, palavra);
					for (j = 0; j < 244; j++) { // 244 eh o numero de stopword do doc stoplist
						fgets(stopword, 100, fp);
						chomp(stopword);
						if (strcmp(palavra, stopword) == 0) {
							find = 1;
							break;
						}
					}
					rewind(fp);
					if (find == 0) {
						auxs->size++;

					}
					memset(palavra, 0, strlen(palavra));
				}
				auxw = auxw->nextWord;
			}
			printf("sn %d size %d\n", auxs->nro_sent, auxs->size); // printf
			auxs = auxs->nextSent;
		}
		puts(""); // puts \n
		auxt = auxt->nextTxt;
	}
	fclose(fp);
}

void set_standard_size(File *F, int nDocs, int largerSize) {
	int i;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
	        while (auxs != NULL) {
	                auxs->standardSize = (float) auxs->size / (float) largerSize;
			printf("D %d ss %f\n", i, auxs->standardSize); // printf
	                auxs = auxs->nextSent;
		}
		puts(""); // puts \n
		auxt = auxt->nextTxt;
	}
}

void standard_size(File *F, int nDocs) {
	int i, largerSize = INT_MIN;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			if (auxs->size > largerSize) {
				largerSize = auxs->size;
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}
	printf("D %d largerSize %d\n", i, largerSize); // printf
	set_standard_size(F, nDocs, largerSize);
}

int printfrequencytable(frequencyBlock *auxfb) {

	if (auxfb == NULL) {
		return 0;
	}
	printf("w %s f %d\n", auxfb->lema, auxfb->frequency);
	printfrequencytable(auxfb->nextword);
}

int IsWordContent(char *word) {
	int j, find = 0;
	char stopword[50];
	FILE *fp;

	fp = fopen("stoplist_pt.txt", "r");
	for (j = 0; j < 244; j++) { // 244 eh o numero de stopword do doc stoplist
		fgets(stopword, 100, fp);
		chomp(stopword);
		if (strcmp(word, stopword) == 0) {
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int create_frequency_table(File *F, int nDocs) {
	int i, find;
	char palavra[50];
	word *auxw;
	sentence *auxs;
	text *auxt;
	frequencyBlock *auxfb, *auxfb01, *auxfbprev;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			auxw = auxs->firstWord;
			while (auxw != NULL) {
				if ((IsWordContent(auxw->lema) == 0) && (isalnum(auxw->palavra[0]) != 0)) {
					if (F->FTfirstword == NULL) {
						CREATEFB(auxfb);
						if (auxfb == NULL) {
							return OUT_OF_MEMORY;
						}
						auxfb->nextword = NULL;
						auxfb->frequency = 0;
						strcpy(auxfb->lema, auxw->lema);
						F->FTfirstword = auxfb;
					}
					else {
						auxfb01 = F->FTfirstword;
						find = 1;
						while (auxfb01 != NULL) {
							if (strcmp(auxfb01->lema, auxw->lema) == 0) {
								find = 0;
							}
							auxfbprev = auxfb01;
							auxfb01 = auxfb01->nextword;
						}
						if (find != 0) {
							CREATEFB(auxfb);
							if (auxfb == NULL) {
								return OUT_OF_MEMORY;
							}
							auxfb->nextword = NULL;
							auxfb->frequency = 0;
							strcpy(auxfb->lema, auxw->lema);
							auxfbprev->nextword = auxfb;
						}
					}
				}
				auxw = auxw->nextWord;
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}
}

void calculates_frequency_table(File *F, int nDocs) {
	int i;
	frequencyBlock *auxfb;
	word *auxw;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxfb = F->FTfirstword;
		while (auxfb != NULL) {
			auxs = auxt->firstSent;
			while (auxs != NULL) {
				auxw = auxs->firstWord;
				while (auxw != NULL) {
					if (strcmp(auxfb->lema, auxw->lema) == 0) {
						auxfb->frequency++;
					}
					auxw = auxw->nextWord;
				}
				auxs = auxs->nextSent;
			}
			auxfb = auxfb->nextword;
		}
		auxt = auxt->nextTxt;
	}
//	printfrequencytable(F->FTfirstword);
//	puts("");
}

void set_standard_frequency(File *F, int nDocs, int higherFrequency) {
	int i;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
	        while (auxs != NULL) {
	                auxs->standardFrequency = (float) auxs->frequency / (float) higherFrequency;
			printf("D %d sf %f\n", i, auxs->standardFrequency); // printf
	                auxs = auxs->nextSent;
		}
		puts(""); // puts \n
		//print(auxt->firstSent);
		auxt = auxt->nextTxt;
	}
}

void standard_frequency(File *F, int nDocs) {
	int i, higherFrequency = INT_MIN;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			if (auxs->frequency > higherFrequency) {
				higherFrequency = auxs->frequency;
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}
	printf("D %d higherFrequency %d\n", i, higherFrequency); // printf
	set_standard_frequency(F, nDocs, higherFrequency);
}

void calculates_frequency_sentence(File *F, int nDocs) {
	int i, higherFrequency = INT_MIN;
	frequencyBlock *auxfb;
	word *auxw;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) { 
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			auxw = auxs->firstWord;
			while (auxw != NULL) {
				auxfb = F->FTfirstword;
				while (auxfb != NULL) {
					if (strcmp(auxfb->lema, auxw->lema) == 0) {
						auxs->frequency += auxfb->frequency;
						break;
					}
					auxfb = auxfb->nextword;
				}
				auxw = auxw->nextWord;
			}
			printf("sn %d frequency %d\n", auxs->nro_sent, auxs->frequency); // printf
			auxs = auxs->nextSent;
		}
		puts(""); // puts \n
		auxt = auxt->nextTxt;
	}
}

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

int setCstRelationList(sentence *sent, char *TYPE, int sentNum, char *docName, int SSENT) {
	cstRelation *auxcr, *newcr;

//	CREATECR(newcr);
	newcr = (cstRelation *) malloc (sizeof(cstRelation));
	CHECK(newcr);

	strcpy(newcr->type, TYPE);
	strcpy(newcr->docName, docName);
	newcr->sentNum = sentNum;
	newcr->SSENT = SSENT;
	newcr->next = NULL;

	if (sent->firstcr == NULL) {
		sent->firstcr = newcr;
	}
	else {
		auxcr = sent->firstcr;
		while (auxcr->next != NULL) {
			auxcr = auxcr->next;
		}
		auxcr->next = newcr;
	}

	return SUCCESS;
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
			auxs->redundancy++; // setar a lista de relacoes cst aqui
			setCstRelationList(auxs, TYPE, TSENT - 1, TDID, YES); // coloca direto na sentenca a qual pertence a relacao
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
			setCstRelationList(auxs, TYPE, SSENT - 1, SDID, NO);
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
	    fgets(line, 1000, fp);  // ignora o lixo do buffer
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
									if (strcmp(TYPE, "Identity") == 0) { // qualquer uma
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Equivalence") == 0) { // pega a menor sentenca
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Summary") == 0) { // pega a menor sentenca
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Subsumption") == 0) { // a que engloba a outra
	                                    setCST(F, nDocs, SDID, TDID, SSENT, TSENT, TYPE);
	                                }
	                                else if (strcmp(TYPE, "Overlap") == 0) { // pega a menor sentenca
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
	//printText(F->firstTxt);
}

// pega as sentencas originais dos textos-fonte
void readFile(File *F, int nDocs) {
    int countLine, position, c, i;
    char *pch, line[1000];
    FILE *fp;
	sentence *auxs;
	text *auxt;

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
    	fp = fopen(auxt->DocName, "r");
		auxs = auxt->firstSent;
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
			strcpy(auxs->sentenca, line);
			//puts(auxs->sentenca);
			auxs = auxs->nextSent;

	        countLine++;
	        c = fgetc(fp);
	    }
		auxt = auxt->nextTxt;
	}
	printText(F->firstTxt);
}

/******************** content selection ********************/

void selectSentences(File *F, int nDocs) {
	int i;
	float R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11;
	sentence *auxs;
	text *auxt;

	R1 = 140.0 / (156.0);
	R2 = 81.0 / (92.0);
	R3 = 68.0 / (80.0);
	R4 = 172.0 / (248.0);
	R5 = 46.0 / (53.0);
	R6 = 197.0 / (285.0);
	R7 = 35.0 / (44.0);
	R8 = 10.0 / (12.0);
	R9 = 12.0 / (14.0);
	R10 = 14.0 / (17.0);
	R11 = 1305.0 / (1651.0);

	auxt = F->firstTxt;

	for (i = 0; i < nDocs; i++) {
		puts(auxt->DocName);
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			// rule 1
			if (auxs->location == BEGIN) {
				auxs->summary = YES;
				auxs->rulePrecision = R1;
				printf("Sentence %d sumario sim, R1 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 2
			else if (auxs->standardRedundancy >= 0.9 && auxs->standardRedundancy <= 1) {
				auxs->summary = YES;
				auxs->rulePrecision = R2;
				printf("Sentence %d sumario sim, R2 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 3
			else if (auxs->standardRedundancy >= 0.6 && auxs->standardRedundancy <= 0.7) {
				auxs->summary = YES;
				auxs->rulePrecision = R3;
				printf("Sentence %d sumario sim, R3 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 4
			else if (auxs->standardRedundancy >= 0.3 && auxs->standardRedundancy <= 0.4) {
				auxs->summary = YES;
				auxs->rulePrecision = R4;
				printf("Sentence %d sumario sim, R4 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 5
			else if (auxs->standardRedundancy >= 0.7 && auxs->standardRedundancy <= 0.8) {
				auxs->summary = YES;
				auxs->rulePrecision = R5;
				printf("Sentence %d sumario sim, R5 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 6
			else if (auxs->standardRedundancy >= 0.4 && auxs->standardRedundancy <= 0.5) {
				auxs->summary = YES;
				auxs->rulePrecision = R6;
				printf("Sentence %d sumario sim, R6 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 7
			else if ((auxs->standardRedundancy >= 0.2 && auxs->standardRedundancy <= 0.3) &&
						(auxs->standardFrequency >= 0.5 && auxs->standardFrequency >= 0.6)) {
				auxs->summary = YES;
				auxs->rulePrecision = R7;
				printf("Sentence %d sumario sim, R7 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 8
			else if ((auxs->standardRedundancy >= 0.1 && auxs->standardRedundancy <= 0.2) &&
						(auxs->standardFrequency >= 0.4 && auxs->standardFrequency >= 0.5)) {
				auxs->summary = YES;
				auxs->rulePrecision = R8;
				printf("Sentence %d sumario sim, R8 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 9
			else if ((auxs->standardRedundancy >= 0.1 && auxs->standardRedundancy <= 0.2) &&
						(auxs->standardSize >= 0.2 && auxs->standardSize >= 0.3)) {
				auxs->summary = YES;
				auxs->rulePrecision = R9;
				printf("Sentence %d sumario sim, R9 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 10
			else if ((auxs->standardSize >= 0.1 && auxs->standardSize <= 0.2) &&
						(auxs->standardFrequency >= 0.3 && auxs->standardFrequency >= 0.4)) {
				auxs->summary = YES;
				auxs->rulePrecision = R10;
				printf("Sentence %d sumario sim, R10 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			} // rule 11
			else {
				auxs->summary = NO;
				auxs->rulePrecision = R11;
				printf("Sentence %d sumario nao, R11 rulePrecison %f\n", auxs->nro_sent, auxs->rulePrecision);
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}
	printf("end selectSentences\n\n");
}

/***************************** ranking ****************************/

/* doRanking praticamente coloca em uma lista ordenada as sentencas por precisao das regras que as selecionaram */

int doRanking(File *F, int nDocs) {
	int i;
	sentence *auxs, *auxs1, *news, *prev, *ranking;
	text *auxt;

	F->ranking = NULL;
	auxt = F->firstTxt;
//	ranking = F->ranking;

	printf("numero de documentos %d\n", nDocs);

	for (i = 0; i < nDocs; i++) {
		auxs = auxt->firstSent;
		while (auxs != NULL) {
			if (auxs->summary == YES) {
				puts(auxs->sentenca);
				CREATES(news);	
				if (news == NULL) {
					return OUT_OF_MEMORY;
				}
				news->nro_sent = auxs->nro_sent;
				news->firstWord = NULL;
				news->nextSent = NULL;
				news->standardSize = auxs->standardSize;
				news->rulePrecision = auxs->rulePrecision;
				news->nro_doc = i;
				strcpy(news->sentenca, auxs->sentenca);
				news->firstcr = auxs->firstcr; // gambiarra pra nao perder as relacoes do tipo cst, ficaram dois ponteiros para uma mesma regioa de memoria
				strcpy(news->docName, auxs->docName);

				if ((F->ranking == NULL) || (news->rulePrecision > F->ranking->rulePrecision)) {
					news->nextSent = F->ranking;
					F->ranking = news;
				}
				else if ((F->ranking != NULL) && (news->rulePrecision == F->ranking->rulePrecision)) {
					if (news->rulePrecision == F->ranking->nextSent->rulePrecision) { // se varias sentencas possuirem a mesma rulePrecision o ranking manterah a ordem de texto-fonte
						auxs1 = F->ranking->nextSent;
						while (news->rulePrecision == auxs1->nextSent->rulePrecision) {
							auxs1 = auxs1->nextSent;
						}
						news->nextSent = auxs1->nextSent;
						auxs1->nextSent = news;
					}
					else {
						news->nextSent = F->ranking->nextSent;
						F->ranking->nextSent = news;
					}
				}
				else {
					prev = NULL;
					auxs1 = F->ranking;
					while ((auxs1 != NULL) && ((auxs1->rulePrecision >= news->rulePrecision))) {
						prev = auxs1;
						auxs1 = auxs1->nextSent;
					}
					news->nextSent = auxs1;
					prev->nextSent = news;
				}
				news = NULL;
			}
			auxs = auxs->nextSent;
		}
		auxt = auxt->nextTxt;
	}

	auxs = F->ranking;
	puts("final ranking");
	while (auxs != NULL) {
		printf("%f %s set %d doc %d\n", auxs->rulePrecision, auxs->sentenca, auxs->nro_sent, auxs->nro_doc);
		auxs = auxs->nextSent;
	}

	puts("");
	return SUCCESS;
}

/* pega os nomes dos textos que contem as relacoes do tipo cst e coloca em uma lista encadeada para evitar de ficar pedindo varias vezes */

int getCstDocName(File *F, int nDocs) {
	int i, nCst;
	char CSTName[100];
	cstName *auxCst, *newCst;

	F->firstCstDoc = NULL;

	puts("Numero de arquivos CST: ");
	scanf("%d", &nCst);

	fgets(CSTName, 100, stdin);		// para limpar o buffer do stdin
	chomp(CSTName);
	//puts(CSTName);


	for (i = 0; i < nCst; i++) {
		puts("Nome do arquivo: ");
		fgets(CSTName, 100, stdin);
		chomp(CSTName);

		CREATECSTDOC(newCst);
		if (newCst == NULL) {
			return OUT_OF_MEMORY;
		}
		newCst->next = NULL;
		strcpy(newCst->docName, CSTName);

		if (F->firstCstDoc == NULL) {
			F->firstCstDoc = newCst;
		}
		else {
			auxCst = F->firstCstDoc;
			while (auxCst->next != NULL) {
				auxCst = auxCst->next;
			}
			auxCst->next = newCst;
		}
	}

	auxCst = F->firstCstDoc;
	while (auxCst != NULL) {
		puts(auxCst->docName);
		auxCst = auxCst->next;
	}

	return SUCCESS;
}

int printRelation2(cstRelation *cr) {

	if (cr == NULL) {
		return 0;
	}
	puts(cr->docName);
	printf("sentNum %d\n", cr->sentNum);
	puts(cr->type);
	if (cr->SSENT == YES) {
		puts("is source.");
	}
	else {
		puts("not source.");
	}
	printRelation2(cr->next);
}

int printRelation1(sentence *sent) {

	if (sent == NULL) {
		return 0;
	}	
	puts(sent->docName);
	printRelation2(sent->firstcr);
	puts("");
	printRelation1(sent->nextSent);
}

int printCstRelationList(File *F, int nDocs) {
	int i;
	text *auxTxt;
/*
	auxTxt = F->firstTxt;
	for (i = 0; i < nDocs; i++) {
		printf("nDoc %d\n", i);
		puts(auxTxt->DocName);
		printRelation1(auxTxt->firstSent);
		auxTxt = auxTxt->nextTxt;
	}*/

	printRelation1(F->ranking);

	return SUCCESS;
}

int printRank(File *F) {
	sentence *auxs;

	auxs = F->ranking;
	puts("final ranking");
	while (auxs != NULL) {
		printf("%f %s set %d doc %d\n", auxs->rulePrecision, auxs->sentenca, auxs->nro_sent, auxs->nro_doc);
		auxs = auxs->nextSent;
	}
	puts("");

	return SUCCESS;
}

int removeAuxs1(sentence *prev, sentence *auxs, sentence *prev1, sentence *auxs1, cstRelation *auxcr) {

	if (prev1 == NULL) { // caso o auxs1 nao tenha sido movimentado ainda
		auxs->nextSent = auxs1->nextSent;
		auxs1 = auxs->nextSent;
		auxcr = auxs->firstcr;
	}
	else {
		prev1->nextSent = auxs1->nextSent;
		auxs1 = prev1->nextSent;
		auxcr = auxs->firstcr;
	}

	return SUCCESS;
}

int removeAuxs(sentence *prev, sentence *auxs, sentence *prev1, sentence *auxs1, cstRelation *auxcr) {

	if (prev == NULL) { // caso o auxs nao tenha sido movimentado ainda
		auxs = auxs->nextSent;
		auxcr = auxs->firstcr;
		if (auxs == auxs1) {
			auxs1 = auxs1->nextSent;
		}
	}
	else {
		prev->nextSent = auxs->nextSent;
		if (auxs == prev1) {
			prev1 = prev1->nextSent;
			auxs1 = auxs1->nextSent;
		}
		auxs = prev->nextSent;
		auxcr = auxs->firstcr;
	}

	return SUCCESS;
}

int rmRedundancySimple(File *F, int nDocs) {
	sentence *auxs, *auxs1, *prev, *prev1;
	cstRelation *auxcr;

	auxs = F->ranking;
	prev = NULL;
	prev1 = NULL;

	while (auxs != NULL) {
		auxcr = auxs->firstcr;
		while (auxcr != NULL) {
			auxs1 = auxs->nextSent;
			while (auxs1 != NULL) {
				if (strcmp(auxs1->docName, auxcr->docName) && (auxs1->nro_sent == auxcr->sentNum)) {
					if (strcmp(auxcr->type, "Identity") == 0) { // removo sempre auxs1 por questao de facilidade
						puts("\tIdentity");
						puts(auxs->sentenca);
						puts(auxs1->sentenca);
						//removeAuxs1(prev, auxs, prev1, auxs1, auxcr);
					}
					else if (strcmp(auxcr->type, "Subsumption") == 0) {
						puts("\tSubsumption");
						if (auxcr->SSENT == 1) { // remover auxs, indica que auxs1 eh source, portanto engloba auxs
							puts("remove auxs");
							puts(auxs->sentenca);
							puts(auxs1->sentenca);
							/*if (prev == NULL) {
								removeAuxs(prev, F->ranking, prev1, auxs1, auxcr);
							}
							else {
								removeAuxs(prev, auxs, prev1, auxs1, auxcr);
							}*/
						}
						else { // indica que auxs1 nao eh source, por isso a removo
							puts("remove auxs1");
							puts(auxs->sentenca);
							puts(auxs1->sentenca);
							//removeAuxs1(prev, auxs, prev1, auxs1, auxcr);
						}
					}
					else {
						puts("\tOthers");
						if (auxs->standardSize < auxs1->standardSize) {
							//puts("auxs->stdSize > auxs1->stdSize");
							puts(auxs->sentenca);
							puts(auxs1->sentenca);
							removeAuxs1(prev, auxs, prev1, auxs1, auxcr);
						}
						else {
							//puts("auxs->stdSize > auxs1->stdSize");
							if (prev == NULL) {
								//removeAuxs(prev, F->ranking, prev1, auxs1, auxcr);
								puts(auxs->sentenca);
								puts(auxs1->sentenca);
							}
							else {
								puts(auxs->sentenca);
								puts(auxs1->sentenca);
								//removeAuxs(prev, auxs, prev1, auxs1, auxcr);
							} 
						}
					}
				}
				prev1 = auxs1;
				auxs1 = auxs1->nextSent; // problem is here, not exiting form the auxs1 while
			}
			auxcr = auxcr->next;
		}
		prev = auxs;
		auxs = auxs->nextSent;
	}

	printRank(F);

	return SUCCESS;
}
