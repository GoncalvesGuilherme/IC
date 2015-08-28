#include <stdio.h>
#include "text.h"

int main (void) {
   	int regress, nDocs, i;
	File F;

	printf("Digite o numero de documentos: ");
	scanf("%d", &nDocs);

	createFile(&F, nDocs);

	createDoc(&F, nDocs);
//printf("h2\n");

	callPL(&F, nDocs);

	clear(&F, nDocs);

	location(&F, nDocs);

	size(&F, nDocs);

	standard_size(&F, nDocs);

	create_frequency_table(&F, nDocs);

	calculates_frequency_table(&F, nDocs);

	calculates_frequency_sentence(&F, nDocs);

	standard_frequency(&F, nDocs);

	readCST(&F, nDocs);

	readFile(&F, nDocs);

	selectSentences(&F, nDocs);

	doRanking(&F, nDocs);

	printf("end\n");

	return 0;
}
