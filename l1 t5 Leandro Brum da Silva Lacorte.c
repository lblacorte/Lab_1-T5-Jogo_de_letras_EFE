#include "tela.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> //Necessário para srand

//Registro contendo todas as variáveis do jogo:
typedef struct
{
    char tabuleiro[5][5]; //Matriz que contém as letras do tabuleiro
    int pontuacaoAtual; //Guarda a pontuação referente ao jogo atual
    int ranking[5]; //Guarda as 5 melhores pontuações
    char nomes[5][20]; //Guarda as identificações das 5 melhores pontuações
    char nomeAtual[21]; //Guarda a identificação do usuário atual
    bool reiniciar; //Controla o estado do jogo
} JOGO;

//Função para inicializar o tabuleiro e o ranking como nulos:
void preencherNulo (JOGO *novoJogo)
{
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            novoJogo->tabuleiro[i][j] = 0; //Todas as casas da matriz viram zero
        }
        novoJogo->ranking[i] = 0; //Inicializa as maiores pontuações como zero
        novoJogo->nomes[i][0] = '\0'; //E os nomes também
    }
}

//Função para gerar letra A ou B em posição aleatória:
void preencherAleatorio (char tabuleiro[5][5], int quant) //quant = quantidade de letras aleatórias a serem geradas
{
    int linAnt = -1, colAnt = 0, linha = 0, coluna = 0;
    srand(time(NULL)); //Inicializando a 'semente'
    for(int i = 0; i < quant; i++){ //Loop para gerar o número de letras passadas pelo parâmetro 'quant'
        while((linAnt == -1) || (linAnt == linha && colAnt == coluna && tabuleiro[linha][coluna] != 0)){ //Tratando para não poder ser a mesma casa duas vezes, nem sobrescrever uma casa já preenchida
            linha = rand() % 5; //Sorteando um número de 0 a 4 para linha da matriz
            coluna = rand() % 5; //Sorteando um número de 0 a 4 para coluna da matriz
            if(i == 0 && tabuleiro[linha][coluna] == 0) break; //Caindo fora do laço se for a primeira vez, para não ficar infinito
        }
        int letra = rand() % 4; //Sorteando um número de 0 a 3 para probabilidade de letra A ou B
        if(letra < 3) tabuleiro[linha][coluna] = 'A'; //75% de chance de ser A
        else tabuleiro[linha][coluna] = 'B'; //25% de chance de ser B
        linAnt = linha; //Setando uma variável para comparar com a próxima linha aleatória (caso exista)
        colAnt = coluna; //Setando uma variável para comparar com a próxima coluna aleatória (caso exista)
    }
}

//Definindo identificadores para facilitar a chamada posterior das funções de movimentação:
enum{
    linhas,
    colunas,
    crescente,
    decrescente
};

//Função para posicionar os elementos de um vetor no começo/fim dele:
void reordenarVetorPontuando (char faixa[5], int ordem, int *pontuacaoAtual)
{
    int ultCasaVazia = -1;
    if (ordem == crescente){ //Nessa condição os elementos são posicionados no início
        for (int i = 1; i < 5; i++){ //Laço que percorre o vetor a partir da segunda casa (a primeira não muda de lugar)
            if (faixa[i] != 0){ //Se a casa não estiver vazia
                for (int j = i - 1; j >= 0; j--){ //Laço que percorre todas as casas anteriores à i, a partir de i-1 até 0
                    if (faixa[j] == 0) ultCasaVazia = j; //Se a casa for vazia guarda a posição do vetor, muda sempre para ter a última
                }
                if(ultCasaVazia != -1){ //Se não houver casa vazia será -1 e não acontecerá nada
                    faixa[ultCasaVazia] = faixa[i]; //Passa o valor em i para a última casa vazia
                    faixa[i] = 0; //Esvazia a casa que mudou de lugar
                    (*pontuacaoAtual)++; //Aumenta um ponto para cada casa movimentada
                    ultCasaVazia = -1; //Atribui -1 para zerar a última casa novamente, para o caso de não haver nenhuma
                }
            }
        }
    }
    else if (ordem == decrescente){ //Nessa condição os elementos são posicionados no fim
        for (int i = 3; i >= 0; i--){ //O mesmo processo, mas ao contrário
            if (faixa[i] != 0){ 
                for (int j = i + 1; j < 5; j++){ 
                    if (faixa[j] == 0) ultCasaVazia = j;
                }
                if(ultCasaVazia != -1){ 
                    faixa[ultCasaVazia] = faixa[i]; 
                    faixa[i] = 0; 
                    (*pontuacaoAtual)++;
                    ultCasaVazia = -1;
                }
            }
        }
    }
}

//Função que atribui a pontuação respectiva à letra que foi formada na junção
void atribuirPontuacaoJuncao(char letra, int *pontuacaoAtual)
{
    switch (letra){
        case 'B':
            *pontuacaoAtual += 30;
            break;
        case 'C':
            *pontuacaoAtual += 90;
            break;
        case 'D':
            *pontuacaoAtual += 270;
            break;
        case 'E':
            *pontuacaoAtual += 810;
            break;
        case 'F':
            *pontuacaoAtual += 2430;
            break;
    }
}

//Função para juntar 3 letras iguais:
void juntarLetras (char faixa[5], int ordem, int *pontuacaoAtual)
{
    int comparacao1, comparacao2;
    if (ordem == crescente){ //Se a direção da junção for para o início do vetor
        for (int i = 0; i < 5; i++){
            comparacao1 = -1; comparacao2 = -1;
            if (faixa[i] != 0){
                for (int j = i + 1; j < 5; j++){
                    if (faixa[j] != 0 && comparacao1 == -1) comparacao1 = j; //Isso serve pra pegar a casa imediata à i que seja uma letra, pulando os zeros
                    else if (faixa[j] != 0){
                        comparacao2 = j; //E aqui pega a próxima
                        break;
                    }
                }
                if (comparacao1 != -1 && comparacao2 != -1 && faixa[i] == faixa[comparacao1] && faixa[i] == faixa[comparacao2]){ //Agora, compara i com as duas próximas letras, caso elas existam
                    faixa[i] = faixa[i] + 1; //i vira a próxima letra
                    atribuirPontuacaoJuncao(faixa[i], pontuacaoAtual);
                    faixa[comparacao1] = 0; //Seta como zero as duas outras letras que se juntaram a i
                    faixa[comparacao2] = 0;
                    break;
                }
            }
        }
    }
    else if (ordem == decrescente){ //Se a direção da junção for para o fim do vetor
        for (int i = 4; i >= 0; i--){ //De novo, o mesmo processo, porém com o laço invertido
            comparacao1 = -1; comparacao2 = -1;
            if (faixa[i] != 0){
                for (int j = i - 1; j >= 0; j--){
                    if (faixa[j] != 0 && comparacao1 == -1) comparacao1 = j;
                    else if (faixa[j] != 0){
                        comparacao2 = j;
                        break;
                    }
                }
                if (comparacao1 != -1 && comparacao2 != -1 && faixa[i] == faixa[comparacao1] && faixa[i] == faixa[comparacao2]){
                    faixa[i] = faixa[i] + 1;
                    atribuirPontuacaoJuncao(faixa[i], pontuacaoAtual);
                    faixa[comparacao1] = 0;
                    faixa[comparacao2] = 0;
                    break;
                }
            }
        }
    }
}

//Função para percorrer e movimentar matriz:
void movimentarMatriz (int eixo, int ordem, JOGO *novoJogo)
{
    char faixa[5];
    for (int j = 0; j < 5; j++){
        for (int i = 0; i < 5; i++){
            if (eixo == colunas)
            faixa[i] = novoJogo->tabuleiro[i][j]; //Assim, os valores em cada coluna são armazenados no vetor faixa
            else if (eixo == linhas)
            faixa[i] = novoJogo->tabuleiro[j][i]; //Assim, os valores em cada linha são armazenados no vetor faixa
        }
        juntarLetras(faixa, ordem, &novoJogo->pontuacaoAtual);
        reordenarVetorPontuando(faixa, ordem, &novoJogo->pontuacaoAtual); //E depois esse vetor é passado para a função de reordenar
        for (int i = 0; i < 5; i++){
            if (eixo == colunas)
            novoJogo->tabuleiro[i][j] = faixa[i]; //Agora é feito o processo inverso, após serem reposicionados os elementos no vetor
            else if (eixo == linhas)
            novoJogo->tabuleiro[j][i] = faixa[i]; //Agora é feito o processo inverso, após serem reposicionados os elementos no vetor 
        }
    } //Como o segundo laço termina depois, as colunas/linhas são movimentadas uma por vez
}

//Função para verificar se tem algum zero na matriz:
bool verificarSeTaCheio (char tabuleiro[5][5])
{
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            if (tabuleiro[i][j] == 0) return false; //Se tiver algum zero retorna false
        }
    }
    return true; //Senão retorna true
}

//Função para verificar se há jogada possível:
bool verificarJogadaPossivel (char tabuleiro[5][5])
{
    bool possivel = false;
    if (verificarSeTaCheio(tabuleiro)){ //Se a matriz estiver completa, sem zeros
        for (int i = 0; i < 5; i++){
            for (int j = 0; j < 3; j++){
                if (tabuleiro[i][j] == tabuleiro[i][j+1] && tabuleiro[i][j] == tabuleiro[i][j+2])
                possivel = true; //Testa se há alguma linha com três letras iguais juntas
            }
        }
        for (int j = 0; j < 5; j++){
            for (int i = 0; i < 3; i++){
                if (tabuleiro[i][j] == tabuleiro[i+1][j] && tabuleiro[i][j] == tabuleiro[i+2][j])
                possivel = true; //Testa se há alguma coluna com três letras iguais juntas
            }
        }
    }
    else possivel = true; //Se tiver algum zero, é possível jogar
    return possivel; //Caso passe em todos os testes e não haja jogada, retorna falso
}

//Verifica se o jogador ganhou (se formou um F):
bool verificarVitoria (char tabuleiro[5][5])
{
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            if (tabuleiro[i][j] == 'F') return true; //Retorna true se houver algum F na matriz
        }
    }
    return false;
}

//Função para atualizar as variáveis que guardam as melhores pontuações e nomes:
bool atualizarRank (JOGO *novoJogo)
{
    int auxiliar1 = 0, auxiliar2 = 0;
    char palavra1[20], palavra2[20];
    palavra1[0] = '\0'; palavra2[0] = '\0';
    for (int i = 0; i < 5; i++){
        if (novoJogo->pontuacaoAtual > novoJogo->ranking[i]){ //Se a pontuação atual é maior que alguma das maiores
            auxiliar1 = novoJogo->ranking[i];
            strcpy(palavra1, novoJogo->nomes[i]);
            novoJogo->ranking[i] = novoJogo->pontuacaoAtual;
            strcpy(novoJogo->nomes[i], "*"); //Passa a pontuação e nome atuais para o lugar da outra
            for (int j = i + 1; j < 5; j++){ //Após, reorganiza todas as pontuações posteriores uma casa para trás, caso tenha
                auxiliar2 = novoJogo->ranking[j];
                strcpy(palavra2, novoJogo->nomes[j]);
                novoJogo->ranking[j] = auxiliar1;
                strcpy(novoJogo->nomes[j], palavra1);
                auxiliar1 = auxiliar2;
                strcpy(palavra1, palavra2);
            }
            return true; //Retorna true se houver feito alguma alteração (se a pontuação atual é suficiente)
        }
    }
    return false;
}

//Função para ler os caracteres do nome digitado pelo usuário:
bool lerCaractereNome (char nome[21], int *i)
{
    int tecla = tela_tecla();
    if (tecla == c_enter && *i > 2) return false; //Se a tecla for enter e tiver mais de 2 letras cai fora retornando false
    else if (((tecla >= 'a' && tecla <= 'z') || (tecla >= 'A' && tecla <= 'Z') || tecla == '_') && *i != 20){
        nome[*i] = tecla; //Guarda o caractere na devida posição
        nome[*i+1] = '\0'; //Seta o próximo caractere como nulo
        (*i)++; //Pula para a próxima casa
    }
    else if (tecla == c_back && *i != 0){ //Se for apertado backspace e não estiver na primeira casa
        (*i)--; //Volta uma casa
        nome[*i] = '\0'; //Seta a atual como nula
    }
    return true; //Retorna true pra qualquer caractere que não seja enter
}

//Função que exibe o tabuleiro atualizado:
void exibirTabuleiro (char tabuleiro[5][5]) 
{
    int corBase = tela_cria_cor(0.161, 0.243, 0.388); //Criação de nova cor Azul Marinho Suave
    tela_retangulo(175, 100, 675, 600, 1, corBase, corBase); //Cria um quadrado para base do tabuleiro
    for (int i = 0; i < 5; i++){ //Laço que percorre linhas
        for (int j = 0; j < 5; j++){ //Laço que percorre colunas
            //Variáveis para calcular posição da casa do tabuleiro no eixo X (depende da linha) e 
            // Y (depende da coluna), posição do quadrado base + borda + espaços de 100px:
            float calcPosicaoX = 175 + 10 + 100 * j;
            float calcPosicaoY = 100 + 10 + 100 * i;
            int corQuadrado;
            switch (tabuleiro[i][j]){ //Teste com o elemento alocado em determinada posição da matriz
                case 'A':
                    //Criação da paleta de cores para as casas do tabuleiro:
                    corQuadrado = tela_cria_cor(1.000, 0.537, 0.671); //Rosa Intenso
                    break;
                case 'B':
                    corQuadrado = tela_cria_cor(1.000, 0.969, 0.506); //Amarelo Vivo
                    break;
                case 'C':
                    corQuadrado = tela_cria_cor(0.435, 0.780, 1.000); //Azul Celeste Intenso
                    break;
                case 'D':
                    corQuadrado = tela_cria_cor(0.604, 0.749, 0.416); //Verde Oliva
                    break;
                case 'E':
                    corQuadrado = tela_cria_cor(0.753, 0.506, 1.000); //Lilás Vibrante
                    break;
                case 'F':
                    corQuadrado = tela_cria_cor(0.694, 0.694, 0.694); //Cinza Forte
                    break;
                default:
                    corQuadrado = branco;
                    break;
            }
            //Desenha um quadrado 80x80 na posição definida, com determinada cor para cada caso:
            tela_retangulo(calcPosicaoX, calcPosicaoY, calcPosicaoX + 80, calcPosicaoY + 80, 1, corQuadrado, corQuadrado);
            //Escreve a letra (ou nulo) no meio do quadrado de 80x80:
            char letra[2] = {tabuleiro[i][j], '\0'}; //Passando o char para um vetor de char (necessário em tela_texto)
            tela_texto(calcPosicaoX + 40, calcPosicaoY + 40, 50, preto, letra);
        }
    }
}

//Função para exibir a pontuação atual, a maior e alguns textos de formatação
void exibirPontuacaoETextos (int pontuacaoAtual, int melhorPontuacao)
{
    char pontosAtual[6];
    sprintf(pontosAtual, "%d", pontuacaoAtual); //Converte a pontuação em string
    char pontosMelhor[6];
    sprintf(pontosMelhor, "%d", melhorPontuacao);
    int corBase = tela_cria_cor(0.161, 0.243, 0.388);
    tela_retangulo(500, 5, 640, 80, 1, corBase, corBase);
    tela_texto(570, 15, 20, laranja, "SCORE");
    tela_texto(570, 50, 45, branco, pontosAtual);
    tela_retangulo(650, 5, 790, 80, 1, corBase, corBase);
    tela_texto(720, 15, 20, laranja, "BEST");
    if (melhorPontuacao != 0) tela_texto(720, 50, 45, branco, pontosMelhor); //Só imprime a melhor pontuação se ela existir (0 = não existe)
    tela_texto_dir(60, -5, 65, branco, "EFE");
    tela_texto_dir(65, 60, 18, branco, "Junte as letras e chegue ao bloco F!");
    tela_texto_dir(65, 603, 18, branco, "COMO JOGAR: Use as setas do teclado para mover as peças. Quando três peças");
    tela_texto_dir(65, 623, 18, branco, "com a mesma letra se tocam, elas se fundem em uma só!");
}

//Função para guardar o nome do jogador atual na sua posição do ranking marcada com * anteriormente
void guardarNome (JOGO *novoJogo)
{
    for (int i = 0; i < 5; i++){
        if (novoJogo->nomes[i][0] == '*'){ //Procura a string que começa com *
            strcpy(novoJogo->nomes[i], novoJogo->nomeAtual);
        }
    }
}

//Função para salvar as variáveis das melhores pontuações e nomes no arquivo txt:
void salvarNoArquivo (JOGO *novoJogo)
{
    FILE * arquivo;
    arquivo = fopen("ranking.txt", "w"); //Abre o arquivo pra escrita, caso não exista ele cria
    if (arquivo == NULL) { //Se houver algum erro para abrir
        printf("\nNão foi possível abrir o arquivo do ranking para escrita\n");
        exit(1);
    }
    for (int i=0; i<5; i++) { //Percorre cada uma das posições do ranking
        if (novoJogo->ranking[i] > 0){ //Se a pontuação em determinada posição existe
            fprintf(arquivo, "%s %d\n", novoJogo->nomes[i], novoJogo->ranking[i]); //Escreve no arquivo o nome e a pontuação
        }
        else break; //Se não existe pontuação cai fora, não existe próximas, pois não há como ser zero
    }
    fclose(arquivo); //Fecha o arquivo
}

//Função para ler o arquivo de ranking e inicializar as variáveis:
void lerArquivoDeRanking (JOGO *novoJogo)
{
    FILE * arquivo;
    arquivo = fopen("ranking.txt", "r"); //Abre o arquivo para leitura
    if (arquivo){ //Se o arquivo existe
        int i = 0;
        while (!feof(arquivo)){ //Até o fim do arquivo
            fscanf(arquivo, "%s %d\n", novoJogo->nomes[i], &novoJogo->ranking[i]); //Lê o nome e pontuação de cada linha
            i++;
        } 
        fclose(arquivo);
    }
}

//Função para exibir os botões da tela de ranking:
void exibirBotoesRanking (JOGO *novoJogo, int *i)
{
    tela_retangulo(250, 540, 380, 580, 1, amarelo, amarelo);
    tela_retangulo(410, 540, 540, 580, 1, amarelo, amarelo);
    int cor[2] = {verde, verde}; //Todas as posições com texto verde
    cor[*i] = vermelho; //Marca a posição atual com texto vermelho
    tela_texto(315, 560, 23, cor[0], "Rejogar");
    tela_texto(475, 560, 23, cor[1], "Sair");
    tela_atualiza();
    switch (tela_tecla()){
        case c_right:
            if (*i == 0) (*i)++; //Seta para direita, só tem duas casas, só se for a primeira pula pra segunda
            break;
        case c_left:
            if (*i == 1) (*i)--; //Seta para esquerda, se for a segunda volta pra primeira
            break;
        case c_enter:
            if (*i == 0) novoJogo->reiniciar = true; //O primeiro botão faz iniciar um novo jogo
            if (*i == 1) exit(0); //O segundo botão fecha o programa
    }
}

//Função para exibir a tela de ranking:
void exibirRanking (JOGO *novoJogo)
{
    tela_atualiza();
    int var = 0;
    while(novoJogo->reiniciar == false){ //Necessário testar se não foi apertado o botão de novo jogo, para não ficar em loop infinito
        exibirPontuacaoETextos(novoJogo->pontuacaoAtual, novoJogo->ranking[0]);
        tela_texto(425, 150, 50, vermelho, "RANKING");
        for (int i = 0; i < 5; i++){
            if (novoJogo->ranking[i] > 0){ //Se a pontuação na posição existe
                int calculoY = 250 + i*20 + i*40; //Cálculo para posição na tela
                char linha[40];
                sprintf(linha, "%dº - %s com %d pontos", i+1, novoJogo->nomes[i], novoJogo->ranking[i]); //Junta todas as informações em uma só string
                tela_retangulo(80, calculoY - 50, 600, calculoY, 1, azul, azul);
                tela_texto_dir(90, calculoY - 37, 20, branco, linha);
            }
        }
        exibirBotoesRanking(novoJogo, &var);
    }
}

//Função para exibir a tela para digitar o nome:
void exibirTelaNome (JOGO *novoJogo)
{
    int i = 0;
    novoJogo->nomeAtual[0] = '\0';
    while (i <= 20 && i >= 0 && novoJogo->reiniciar == false){ //Até 20 caracteres (o último é \0), enquanto não for apertado para reiniciar
        exibirPontuacaoETextos(novoJogo->pontuacaoAtual, novoJogo->ranking[0]);
        tela_retangulo(175, 370, 675, 450, 10, amarelo, preto);
        tela_texto_dir(50, 200, 24, amarelo, "Digite seu nickname (usando de 3 a 20 letras sem acento e _)");
        tela_texto_dir(50, 230, 24, amarelo, "e aperte enter, para entrar no ranking:");
        if (lerCaractereNome(novoJogo->nomeAtual, &i)){ //Lê e verifica se o caractere não for enter
            exibirPontuacaoETextos(novoJogo->pontuacaoAtual, novoJogo->ranking[0]);
            tela_texto_dir(220, 390, 35, branco, novoJogo->nomeAtual); //Exibe o nome atualizado
            tela_atualiza();
        }
        else{ //Se for enter
            guardarNome(novoJogo);
            salvarNoArquivo(novoJogo);
            exibirRanking(novoJogo);
        }
    }
}

//Função para exibir os botões da tela de fim de jogo:
void exibirBotoesFimDeJogo (JOGO *novoJogo, bool gameOver)
{
    int i = 0, cor;
    char comentario[15];
    if (gameOver == true){ //Se for a tela de game over
        cor = vermelho;
        strcpy(comentario, "GAME OVER!");
    }
    else{ //Se for a tela de vitória
        cor = verde;
        strcpy(comentario, "VOCÊ VENCEU!");
    }
    while(novoJogo->reiniciar == false){ //Enquanto não for apertado para reiniciar
        exibirTabuleiro(novoJogo->tabuleiro);
        exibirPontuacaoETextos(novoJogo->pontuacaoAtual, novoJogo->ranking[0]);
        tela_retangulo(175, 250, 675, 450, 10, cor, preto);
        tela_texto(425, 320, 50, branco, comentario);
        tela_retangulo(200, 390, 330, 430, 1, amarelo, amarelo);
        tela_retangulo(360, 390, 490, 430, 1, amarelo, amarelo);
        tela_retangulo(520, 390, 650, 430, 1, amarelo, amarelo);
        int cor[3] = {verde, verde, verde}; //Todas as posições inicializadas com texto verde
        cor[i] = vermelho; //Marca a posição atual com texto vermelho
        tela_texto(265, 410, 23, cor[0], "Ranking");
        tela_texto(425, 410, 23, cor[1], "Rejogar");
        tela_texto(585, 410, 23, cor[2], "Sair");
        tela_atualiza();
        switch (tela_tecla()){
            case c_right:
                if (i < 2) i++; //Seta para direita, se for antes da última casa pula pra próxima
                break;
            case c_left:
                if (i > 0) i--; //Seta para direita, se for depois da primeira casa volta uma
                break;
            case c_enter:
                if (i == 0) exibirRanking(novoJogo); //Primeiro botão vai pra tela de ranking
                if (i == 1) novoJogo->reiniciar = true; //Segundo botão inicia uma nova partida
                if (i == 2) exit(0); //Terceiro botão sai do programa
        }
    }
}

//Função para mostrar tela de game over:
void exibirGameOver (JOGO *novoJogo){
    double tempo = relogio();
    while(true){ //Espera um segundo pra mostrar a tela
        if (relogio() > tempo + 1) break;
    }
    exibirTabuleiro(novoJogo->tabuleiro);
    exibirPontuacaoETextos(novoJogo->pontuacaoAtual, novoJogo->ranking[0]);
    tela_retangulo(175, 250, 675, 450, 10, vermelho, preto);
    tela_texto(425, 320, 50, branco, "GAME OVER!");
    if (atualizarRank(novoJogo)){ //Caso a pontuação seja suficiente
        tela_retangulo(335, 370, 515, 410, 1, amarelo, amarelo);
        tela_texto(425, 390, 28, vermelho, "Aperte enter"); //Pede para apertar enter para ir pra próxima tela
        tela_atualiza();
        while (true){
            if(tela_tecla() == c_enter) break;
        }
        exibirTelaNome(novoJogo); //Se apertar enter vai para tela de pedir nome
    }
    else exibirBotoesFimDeJogo(novoJogo, true); //Se não for suficiente, exibe botões com opções
}

//Função para mostrar tela de vitória:
void exibirVitoria(JOGO *novoJogo){
    double tempo = relogio();
    while(true){
        if (relogio() > tempo + 1) break;
    }
    exibirTabuleiro(novoJogo->tabuleiro);
    exibirPontuacaoETextos(novoJogo->pontuacaoAtual, novoJogo->ranking[0]);
    tela_retangulo(175, 250, 675, 450, 10, verde, preto);
    tela_texto(425, 320, 50, branco, "VOCÊ VENCEU!");
    if (atualizarRank(novoJogo)){
        tela_retangulo(335, 370, 515, 410, 1, amarelo, amarelo);
        tela_texto(425, 390, 28, vermelho, "Aperte enter");
        tela_atualiza();
        while (true){
            if(tela_tecla() == c_enter) break;
        }
        exibirTelaNome(novoJogo);
    }
    else exibirBotoesFimDeJogo(novoJogo, false);
}

//Função para fazer o controle do tabuleiro conforme as teclas pressionadas:
void controlarTabuleiro (JOGO *novoJogo)
{
    if (verificarVitoria(novoJogo->tabuleiro)) exibirVitoria(novoJogo); //Se ganhar, mostrar a tela da vitória
    if (verificarJogadaPossivel(novoJogo->tabuleiro)){ //Se houver jogada possível
    bool tecla_valida = true;
    //Para cada caso, há um loop diferente, movimentando linha (i) ou coluna (j), de trás pra frente ou vice-versa:
    switch(tela_tecla()){ //Faz o teste para verificar qual tecla foi pressionada
        case c_up: //Se for pressionado seta pra cima,
            movimentarMatriz(colunas, crescente, novoJogo); //as colunas são movimentadas pra cima
            break;
        case c_down: //Se for pressionado seta pra baixo,
            movimentarMatriz(colunas, decrescente, novoJogo); //as colunas são movimentadas pra baixo
            break;
        case c_left: //Se for pressionado seta pra esquerda,
            movimentarMatriz(linhas, crescente, novoJogo); //as linhas são movimentadas pra esquerda
            break;
        case c_right: //Se for pressionado seta pra direita,
            movimentarMatriz(linhas, decrescente, novoJogo); //as linhas são movimentadas pra direita
            break;
        default:
            tecla_valida = false; //Se for qualquer outra tecla não é válida
            break;
    }
    if (tecla_valida == true && verificarSeTaCheio(novoJogo->tabuleiro) == false) //Se for pressionado uma tecla válida e a matriz não estiver cheia
    preencherAleatorio(novoJogo->tabuleiro, 1); //Preenche uma casa aleatória com A ou B
    }
    else exibirGameOver(novoJogo); //Se não houver jogada possível, game over
}

//Função principal:
int main(void)
{
    JOGO novoJogo; //Cria um registro contendo todos os dados do jogo
    novoJogo.reiniciar = true; //Seta reiniciar como true para primeira vez
    tela_inicio(850, 650, "L1T5 | Leandro Lacorte"); //Inicializando tela gráfica
    while (true){ //Esse laço só é quebrado pelos botões de "sair"
        if (novoJogo.reiniciar){ //Se for pra começar um novo jogo
            novoJogo.pontuacaoAtual = 0; //Inicializa a pontuação atual com zero
            preencherNulo(&novoJogo);
            preencherAleatorio(novoJogo.tabuleiro, 2); //Preenche duas casas aleatórias com A ou B
            lerArquivoDeRanking(&novoJogo);
            novoJogo.reiniciar = false; //Não começa novo jogo enquanto não for apertado o respectivo botão
        }
        exibirTabuleiro(novoJogo.tabuleiro);
        exibirPontuacaoETextos(novoJogo.pontuacaoAtual, novoJogo.ranking[0]);
        tela_atualiza();
        controlarTabuleiro(&novoJogo);
    }
}