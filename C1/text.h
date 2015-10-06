#ifndef TEXT_H
#define TEXT_H

#define OUT_OF_MEMORY -1
#define SUCCESS 0

#define BEGIN 10
#define MIDDLE 11
#define END 12
#define YES 13
#define NO 14

typedef struct cstName {
	char docName[100];
	struct cstName *next;
} cstName;

typedef struct frequency_table {
	char lema[50];
	int frequency;
	struct frequency_table *nextword;
} frequencyBlock;

typedef struct word {
	char palavra[50];
	char lema[50];
	char classe_gramat[50];
	struct word *nextWord;
} word;

typedef struct sentence {
	char sentenca[1000];
	int nro_sent;
	int nro_doc;
	int location;			//
	int frequency;			// valores dos atributos
	float standardFrequency;	// normalizado
	int size;			//
	float standardSize;		// normalizado
	int redundancy;		//
	float standardRedundancy;	// normalizado
	int summary;		// indica se a sentenca vai ou nao para o sumario
	float rulePrecision;
	word *firstWord;
	struct sentence *nextSent;
} sentence;

typedef struct text {
	char DocName[100];
	char DocNamepl[100];
	int nDoc;
	sentence *firstSent;
	struct text *nextTxt;
} text;

typedef struct file {
	int nDocs;
	frequencyBlock *FTfirstword;
	cstName *firstCstDoc;
	text *firstTxt;
	sentence *ranking;
} File;					// colecao

/**
 * @brief Retira o \n do final da string.
 *
 * @param char string.
 *
 * @return void.
 */
void chomp(char *);

/**
 * @brief Cria um ponteiro para uma colecao de textos.
 *
 * @param ponteiro para registro do tipo File.
 * @param numero de documentos (como houve problemas com o fgets, eh necessario que seja n + 1 para que possa trabalhar com n textos)
 *
 * @return SUCCESS, se sucesso;
 *         OUT_OF_MEMORY, se ha memoria disponivel.
 */
int createFile(File *F, int nDocs);

/**
 * @brief Cria a lista que representa a colecao de textos.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return SUCCESS, se sucesso;
 *         OUT_OF_MEMORY, se ha memoria disponivel.
 */
int createDoc(File *F, int nDocs);

/**
 * @brief Realiza a chamada do programa externo, Palavras, que realiza a lematizacao e analise sintatica.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void callPL(File *F, int nDocs);

/**
 * @brief Remove as linhas vazias geradas pelo Palavras.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return SUCCESS, se sucesso;				lembrar de trocar para void.
 */
int clear(File *F, int nDocs);

/**
 * @brief Realiza a separacao dos elementos gerados pelo Palavras, colocando em uma lista.
 *
 * @param ponteiro para um texto.
 *
 * @return SUCCESS, se sucesso;
 *         OUT_OF_MEMORY, se nao ha memoria disponivel.
 */
int separate(text *);

/**
 * @brief Imprime as sentencas e seus respectivos atributos.
 *
 * @param ponteiro para uma sentenca.
 *
 * @return zero, se imressao acabou.
 */
int print(sentence *);

/**
 * @brief Define o atributo localizacao para as sentencas.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void location(File *F, int nDocs);

/**
 * @brief Define o atributo tamanho para as sentencas.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void size(File *F, int nDocs);

/**
 * @brief Define o atributo tamanho normalizado (valor do atributo da sentenca / maior valor encontrado para o atributo na colecao).
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void standard_size(File *F, int nDocs);

/**
 * @brief Cria a lista que representa a tabela de palavras encontradas nos textos da colecao.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return SUCCESS, se sucesso;
 *         OUT_OF_MEMORY, se ha memoria disponivel.
 */
int create_frequency_table(File *F, int nDocs);

/**
 * @brief Realiza a contagem das palavras encontradas no colecao contabilizando na tabela de frequencias.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void calculates_frequency_table(File *F, int nDocs);

/**
 * @brief Define o atributo tamanho normalizado.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void standard_frequency(File *F, int nDocs);

/**
 * @brief Define o atributo frequencia para as sentencas com base na tabela de frequencias.
 *
 * @param ponteiro para a colecao de textos.
 * @param numero de textos.
 *
 * @return void.
 */
void calculates_frequency_sentence(File *F, int nDocs);

void readCST(File *F, int nDocs);

void readFile(File *F, int nDocs);

void selectSentences(File *F, int nDocs);

int doRanking(File *F, int nDocs);

//int removeRedundancyFromRank(File *F, int nDocs);

int getCstDocName(File *F, int nDocs);

#endif
