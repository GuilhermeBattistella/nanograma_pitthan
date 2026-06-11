#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>

// definicoes janela
#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define TITULO_JANELA "Nonogram"

// definicoes tabuleiro
#define LINHAS 8
#define COLUNAS 8
#define ALTURA_CELULA 50
#define LARGURA_CELULA 50
#define LARGURA_TABULEIRO LARGURA_CELULA * COLUNAS
#define ALTURA_TABULEIRO ALTURA_CELULA * LINHAS
#define MARGIN_HORIZONTAL (int) ((LARGURA_JANELA / 2) - (LARGURA_TABULEIRO / 2))
#define MARGIN_VERTICAL  (int) ((ALTURA_JANELA / 2) - (ALTURA_TABULEIRO / 2))

#define MAX_DICAS 3
#define MIN_CEL_PINTAR 30
#define MAX_VIDA 3

typedef struct jogada{
    int jogada_passada_i, jogada_passada_j;
    int nova_jogada_i, nova_jogada_j;
} Jogada;

typedef enum modo{
    MD_CLASSICO,
    MD_NORMAL
} Modo;

typedef struct nanograma{
    int gabarito[LINHAS][COLUNAS];

    int numeros_linha[LINHAS][COLUNAS];
    int quant_numeros_linha[LINHAS];

    int numeros_coluna[LINHAS][COLUNAS];
    int quant_numeros_coluna[COLUNAS];

    bool usuario_ganhou;
    bool usuario_perdeu;

    int quant_celulas_pintar;
    int quant_celulas_pintadas;
    
    Jogada jogada;
    Modo modo;

    int dicas_restante;
    int vidas_restante;
} Nanograma;

typedef struct menu{
    bool show_menu;
} Menu;
// estrutura que representa uma cor, com RGB podendo variar entre 0 e 1

typedef struct cor {
    float r;
    float g;
    float b;
} Cor;

// estrutura que representa um ponto, com as coordenadas x e y, do tipo float
typedef struct ponto {
    float x;
    float y;
} Ponto;

// estrutura que representa um tamanho, constituído por altura e largura, do tipo float
typedef struct tamanho {
    float larg;
    float alt;
} Tamanho;

/* Retangulo/Celula, constituído por:
    a) um ponto (que contém as coordenadas do canto inferior esquerdo do retangulo)
    b) um tamanho
    c) um estado, podendo ser vazio, colorido ou alerta (usado quando for Celula)
*/ 
typedef struct retangulo {
    Ponto pos;
    Tamanho tam;
    enum { VAZIO, COLORIDO, ALERTA } estado;
} Retangulo, Celula;

// estrutura que representa um botao, contendo o retangulo, o estado e o texto do botao
typedef struct botao {
    Retangulo ret;
    enum { NORMAL, HOVER, APERTADO } estado;
    char const *texto;
} Botao;

// estrutura que representa uma tela
typedef struct tela {
    ALLEGRO_DISPLAY *display;   // display X
    ALLEGRO_COLOR ac_fundo;     // cor de fundo
    ALLEGRO_COLOR ac_cor;       // cor padrao
    ALLEGRO_EVENT_QUEUE *queue; // fila de eventos
    ALLEGRO_FONT *fonte;        // fonte padrao
    Retangulo janela;           // retangulo que contem a janela
    Tamanho tam;                // tamanho da janela
    Ponto _mouse;               // onde esta o mouse
    bool _botao;                // estado do botao do mouse
    int _tecla;                 // ultima tecla apertada
} Tela;

//essas 2 structs o chatgpt me ajudou a criar essa ideia pra que eu não precise ficar passando os botoens por parametro, e sim uma struct com a lista de todos os botoes
typedef enum index_botao{
    LIMPAR,
    NOVO_NANOGRAMA,
    MODO_NORMAL,
    MODO_CLASSICO,
    MENU,
    DESFAZER,
    DICA,
    QNT_BOTOES
} Index_Botao;

typedef struct lista_botao{
    Retangulo lista_btn[QNT_BOTOES];
} Lista_Botao;

// inicializa a tela; deve ser chamada no inicio da execucao do programa
void inicia_tela(Tela *t, int larg, int alt, char *nome);

// limpa a tela toda
void limpa_tela(Tela *t);

// faz aparecer na janela o que foi desenhado
void mostra_tela();

// tempo de espera da tela, em microsegundos
void espera(double ms);

// finaliza a tela; deve ser chamado no final do programa
void finaliza_tela(Tela *t);

// retorna o codigo da proxima tecla apertada (ou 0, se nenhuma tecla tiver sido apertada)
int codigo_tecla(Tela *t);

// processa eventos da tela
void processa_eventos(Tela *t);

// desenha um retangulo (preenchido ou nao) com a cor padrao
void desenha_retangulo(Retangulo r, Tela *t, bool filled);

// desenha uma linha do ponto p1 ao ponto p2 com a cor padrao
void desenha_linha(Ponto p1, Ponto p2, Tela *t);

// desenha um x dentro do retangulo na cor padrao
void desenha_x_dentro_ret(Retangulo r, Tela *t);

// mantem rgb entre 0 e 1
int ajeita_rgb(int x);

// altera a cor padrao (muda a cor dos proximos desenhos de linha/retangulo/caracteres/etc)
void define_cor(Tela *t, Cor c);

// escreve o texto s a partir da posicao p da tela
void escreve_texto(Tela *t, Ponto p, char *s);

// retorna a posicao do cursor do mouse
Ponto posicao_mouse(Tela *t);

// retorna se o botao do mouse estiver apertado
bool botao_clicado(Tela *t);

// retorna tamanho necessario para se escrever o texto s
Tamanho tamanho_texto(Tela *t, char *s);

// retorna se o ponto esta dentro do retangulo
bool ret_contains_pt(Retangulo r, Ponto p);
