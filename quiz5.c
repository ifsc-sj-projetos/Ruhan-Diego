#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUESTOES 10
#define MAX_NOME 50
#define ARQUIVO_PERGUNTAS "perguntas.txt"
#define ARQUIVO_RANKING "ranking.txt"

// Estrutura para armazenar perguntas
typedef struct {
    char pergunta[256];
    char opcoes[4][100];
    int respostaCorreta;
    char explicacao[256];
} Questao;

// Estrutura para armazenar ranking
typedef struct {
    char nome[MAX_NOME];
    int pontuacao;
} Jogador;

// Função para carregar perguntas do arquivo
int carregarPerguntas(Questao quiz[], int maxQuestoes, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
        return 0;
    }

    int i = 0;
    while (i < maxQuestoes) {
        if (fgets(quiz[i].pergunta, sizeof(quiz[i].pergunta), arquivo) == NULL) break;
        quiz[i].pergunta[strcspn(quiz[i].pergunta, "\n")] = '\0';

        for (int j = 0; j < 4; j++) {
            if (fgets(quiz[i].opcoes[j], sizeof(quiz[i].opcoes[j]), arquivo) == NULL) break;
            quiz[i].opcoes[j][strcspn(quiz[i].opcoes[j], "\n")] = '\0';
        }

        char resposta[5];
        if (fgets(resposta, sizeof(resposta), arquivo) == NULL) break;
        quiz[i].respostaCorreta = atoi(resposta);

        if (fgets(quiz[i].explicacao, sizeof(quiz[i].explicacao), arquivo) == NULL) break;
        quiz[i].explicacao[strcspn(quiz[i].explicacao, "\n")] = '\0';

        i++;

        // Ignora linha vazia opcional entre as perguntas
        char linhaVazia[5];
        fgets(linhaVazia, sizeof(linhaVazia), arquivo);
    }

    fclose(arquivo);
    return i;
}

// Função para embaralhar perguntas
void embaralharPerguntas(Questao quiz[], int numQuestoes) {
    srand(time(NULL));
    for (int i = numQuestoes - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Questao temp = quiz[i];
        quiz[i] = quiz[j];
        quiz[j] = temp;
    }
}

// Função para perguntar e validar resposta do usuário
int perguntar(Questao q) {
    int escolha;

    printf("\n%s\n", q.pergunta);
    for (int i = 0; i < 4; i++) {
        printf("%d. %s\n", i + 1, q.opcoes[i]);
    }

    // Validação da entrada do usuário
    do {
        printf("Escolha a resposta (1-4): ");
        if (scanf("%d", &escolha) != 1) {
            while (getchar() != '\n');
            escolha = 0;
        }
    } while (escolha < 1 || escolha > 4);

    if (escolha == q.respostaCorreta) {
        printf("\033[0;32mResposta correta!\033[0m 🎉\n");
        return 1;
    } else {
        printf("\033[0;31mResposta incorreta.\033[0m ❌\n");
        printf("A resposta correta era: %d. %s\n", q.respostaCorreta, q.opcoes[q.respostaCorreta - 1]);
    }

    printf("Explicação: %s\n\n", q.explicacao);
    return 0;
}

// Função para salvar pontuação no ranking
void salvarPontuacao(const char *nome, int pontuacao) {
    FILE *arquivo = fopen(ARQUIVO_RANKING, "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de ranking.\n");
        return;
    }
    fprintf(arquivo, "%s %d\n", nome, pontuacao);
    fclose(arquivo);
}

// Função para exibir o ranking
void mostrarRanking() {
    Jogador ranking[100];
    int count = 0;

    FILE *arquivo = fopen(ARQUIVO_RANKING, "r");
    if (arquivo == NULL) {
        printf("Ainda não há ranking disponível.\n");
        return;
    }

    while (fscanf(arquivo, "%s %d", ranking[count].nome, &ranking[count].pontuacao) == 2) {
        count++;
    }
    fclose(arquivo);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (ranking[j].pontuacao > ranking[i].pontuacao) {
                Jogador temp = ranking[i];
                ranking[i] = ranking[j];
                ranking[j] = temp;
            }
        }
    }

    printf("\n\033[1;33m🏆 RANKING DOS MELHORES JOGADORES 🏆\033[0m\n");
    for (int i = 0; i < count && i < 5; i++) {
        printf("%d. %s - %d pontos\n", i + 1, ranking[i].nome, ranking[i].pontuacao);
    }
}

int main() {
    Questao quiz[MAX_QUESTOES];
    int totalQuestoes = carregarPerguntas(quiz, MAX_QUESTOES, ARQUIVO_PERGUNTAS);

    if (totalQuestoes == 0) {
        printf("Nenhuma pergunta foi carregada. Verifique o arquivo!\n");
        return 1;
    }

    embaralharPerguntas(quiz, totalQuestoes);

    int pontuacao = 0;
    for (int i = 0; i < totalQuestoes; i++) {
        pontuacao += perguntar(quiz[i]);
    }

    printf("\n\033[1;34m=== Resultado Final ===\033[0m\n");
    printf("Você acertou \033[1;32m%d\033[0m de %d perguntas!\n", pontuacao, totalQuestoes);

    char nomeJogador[MAX_NOME];
    printf("Digite seu nome para salvar no ranking: ");
    scanf("%s", nomeJogador);
    salvarPontuacao(nomeJogador, pontuacao);

    mostrarRanking();
    return 0;
}