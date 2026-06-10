#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>

#include <time.h>
#include "util.h" 

// prototipos das funcoes
void inicia_tabuleiro(Celula tabuleiro[][COLUNAS], Nanograma *jogo);
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button);
void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button, bool *menu_show);
void escreve_numeros(Tela *t, int numeros[], int qtd, Ponto inicio, bool horizontal);
void cicla_estado_celula(Celula *c);
void cria_gabarito(Nanograma *jogo);
void limpa_tabuleiro(Celula tabuleiro[][COLUNAS], Nanograma *nanograma);
void novo_jogo(Celula tabuleiro[][COLUNAS], Nanograma *jogo);
bool verifica_vitoria(Nanograma *nanograma, Celula tabuleiro[][COLUNAS]);
Retangulo cria_botao(int x1, int y1, int x2, int y2);
void desenha_botao(Retangulo btn, char *txt, Tela *t);
void verifica_clique_botao(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button, Ponto mouse, bool *menu_show);
void desenha_menu(Tela *t, Lista_Botao *button);
void verifica_clique_menu(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button, bool *menu_show);
void define_botoes(Lista_Botao *btn);

int main(int argc, char **argv) {
    int tecla_pressionada;
    bool menu_show = true;
    Tela t;
    Celula tabuleiro_nonogram[LINHAS][COLUNAS];
    Nanograma jogo;
    Lista_Botao btn;

    jogo.usuario_ganhou = false;

    memset(&jogo, 0, sizeof(Nanograma));
    srand(time(NULL));

    inicia_tela(&t, LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);

    define_botoes(&btn);

    while ((tecla_pressionada = codigo_tecla(&t)) != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        if(menu_show){
            limpa_tela(&t);
            desenha_menu(&t, &btn);
            verifica_clique_menu(&t, tabuleiro_nonogram, &jogo, &btn, &menu_show);
            mostra_tela();
            
            // espera 30 ms antes de atualizar a tela
            espera(30);
        }else{
            limpa_tela(&t);
            
            desenha_tabuleiro(&t, tabuleiro_nonogram, &jogo, &btn);
            verifica_clique(&t, tabuleiro_nonogram, &jogo, &btn, &menu_show);

            mostra_tela();
            
            // espera 30 ms antes de atualizar a tela
            espera(30);
        }
    }
    
    finaliza_tela(&t);

    return 0;
}

void define_botoes(Lista_Botao *btn){
    btn->lista_btn[LIMPAR] = cria_botao(150, 650, 100, 50);
    btn->lista_btn[NOVO_NANOGRAMA] = cria_botao(300, 650, 200, 50);
    btn->lista_btn[MODO_NORMAL] = cria_botao((LARGURA_JANELA/2 - 200), (ALTURA_JANELA/2), 150, 50);
    btn->lista_btn[MODO_CLASSICO] = cria_botao((LARGURA_JANELA/2 + 10), (ALTURA_JANELA/2), 150, 50);
    btn->lista_btn[MENU] = cria_botao(550, 650, 100, 50);
}

Retangulo cria_botao(int x1, int y1, int width, int height){
    Retangulo botao = {{x1, y1}, {width, height}, VAZIO};
    return botao;
}
void desenha_botao(Retangulo btn, char *txt, Tela *t){
    desenha_retangulo(btn, t, false);
    Ponto pos_texto = btn.pos;
    pos_texto.x += 5;
    pos_texto.y += 10;
    escreve_texto(t, pos_texto, txt);
}
int verifica_gabarito(Nanograma *nanograma){
    int total = 0;
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
           if (nanograma->gabarito[i][j] == 1) total++;
        }
    }
    return total;
}
void cria_gabarito(Nanograma *nanograma) {
    do{
        nanograma->quant_celulas_pintar = 0;
        for(int i = 0; i < LINHAS; i++){
            for(int j = 0; j < COLUNAS; j++){
                if(rand() % 2 == 1){
                    nanograma->gabarito[i][j] = COLORIDO;
                    nanograma->quant_celulas_pintar++;
                }else{
                    nanograma->gabarito[i][j] = VAZIO;
                }
            }
        }
    }while(verifica_gabarito(nanograma) < 25);
    
}
void gera_numeros_array(Nanograma *nanograma){
    int count, pos_array;
    int i, j;
    for(i = 0; i < LINHAS; i++){
        count = 0, pos_array = 0;
        for(j = 0; j < COLUNAS; j++){
            if(nanograma->gabarito[i][j] == COLORIDO){
                count++;
            }else if(count > 0){
                nanograma->numeros_linha[i][pos_array] = count;
                pos_array++, count = 0;
            }
        }
        if(count > 0){
            nanograma->numeros_linha[i][pos_array] = count;
            pos_array++;
        }
        nanograma->quant_numeros_linha[i] = pos_array;
    }

    for(j = 0; j < COLUNAS; j++){
        count = 0; pos_array = 0;
        for(i = 0; i < LINHAS; i++){
            if(nanograma->gabarito[i][j] == 1){
                count++;
            }else if(count > 0){
                nanograma->numeros_coluna[j][pos_array] = count;
                pos_array++, count = 0;
            }
        }
        if(count > 0){
            nanograma->numeros_coluna[j][pos_array] = count;
            pos_array++;
        }
        nanograma->quant_numeros_coluna[j] = pos_array;
    }
}

bool verifica_vitoria(Nanograma *nanograma, Celula tabuleiro[][COLUNAS]){
    int celulas_clicadas[COLUNAS], pos_array, count;
    int i, j, contador;
    for(i = 0; i < LINHAS; i++){
        pos_array = 0, count = 0;
        memset(celulas_clicadas, 0, sizeof(celulas_clicadas));
        for(j = 0; j < COLUNAS; j++){
            if(tabuleiro[i][j].estado == COLORIDO){
                count++;
            }else if(count > 0){
                celulas_clicadas[pos_array] = count;
                count = 0, pos_array++;
            }            
        }
        if(count > 0){
            celulas_clicadas[pos_array] = count;
            pos_array++, count = 0;
        }
        if (pos_array != nanograma->quant_numeros_linha[i]) return false;
        for(contador = 0; contador < pos_array; contador++){
            if(nanograma->numeros_linha[i][contador] != celulas_clicadas[contador]) return false;
        }
    }

    for(j = 0; j < COLUNAS; j++){
        pos_array = 0, count = 0;
        memset(celulas_clicadas, 0, sizeof(celulas_clicadas));
        for(i = 0; i < LINHAS; i++){
            if(tabuleiro[i][j].estado == COLORIDO){
                count++;
            }else if(count>0){
                celulas_clicadas[pos_array] = count;
                count = 0, pos_array++;
            }
        }
        if(count > 0){
            celulas_clicadas[pos_array] = count;
            pos_array++, count = 0;
        }
        if (pos_array != nanograma->quant_numeros_coluna[j]) return false;
        for(contador = 0; contador < pos_array; contador++){
            if(celulas_clicadas[contador] != nanograma->numeros_coluna[j][contador]) return false;
        }
    }
    return true;
}

void limpa_tabuleiro(Celula tabuleiro[][COLUNAS], Nanograma *nanograma){
    if(nanograma->usuario_ganhou == true) nanograma->usuario_ganhou = false;
    for(int i = 0; i < LINHAS; i++){
        for(int j = 0; j < COLUNAS; j++){
            tabuleiro[i][j].estado = VAZIO;
        }
    }
}
void novo_jogo(Celula tabuleiro[][COLUNAS], Nanograma *nanograma){
    if(nanograma->usuario_ganhou == true) nanograma->usuario_ganhou = false;
    memset(nanograma, 0, sizeof(Nanograma));
    inicia_tabuleiro(tabuleiro, nanograma);
}


// inicia tabuleiro do jogo, definindo as posicoes de cada celula
void inicia_tabuleiro(Celula tabuleiro[][COLUNAS], Nanograma *nanograma) {
    cria_gabarito(nanograma);
    gera_numeros_array(nanograma);
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            // coordenadas de cada celula (retangulo)
            int x = MARGIN_HORIZONTAL + (LARGURA_CELULA * j);
            int y = MARGIN_VERTICAL + (ALTURA_CELULA * i);
            // inicializacao da celula com coordenadas (ponto), tamanho e o estado (por padrao = vazio)
            Celula c = { { x, y }, { ALTURA_CELULA, LARGURA_CELULA }, VAZIO };
            tabuleiro[i][j] = c;
            
            /*if(nanograma->gabarito[i][j] == COLORIDO){
                printf("%d, ", COLORIDO);
            }else{
                printf("%d, ", VAZIO);
            }*/
        }
        //printf("\n");
    }

}

void desenha_menu(Tela *t, Lista_Botao *button){
    desenha_botao(button->lista_btn[MODO_NORMAL], "NORMAL", t);
    desenha_botao(button->lista_btn[MODO_CLASSICO], "CLASSICO", t);
}

// desenha tabuleiro (celulas) e os numeros nas linhas e colunas
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button) {
    int i, j;
    Cor preto = {0, 0, 0};
    Cor vermelho = {1, 0, 0};
    define_cor(t, preto); // usado para as bordas (quando celula nao estiver colorida) e para o preenchimento (quando estiver colorida)

    for (i = 0; i < LINHAS; i++)  {
        for (j = 0; j < COLUNAS; j++) {
            // desenha a celula. se estado da celula for colorido, ira pintar o retangulo, senão desenha apenas as bordas
            desenha_retangulo(tabuleiro[i][j], t, tabuleiro[i][j].estado == COLORIDO);
            // se estado da celula for alerta, desenha um x vermelho dentro das bordas do retangulo
            if (tabuleiro[i][j].estado == ALERTA) {
                define_cor(t, vermelho);
                desenha_x_dentro_ret(tabuleiro[i][j], t);
                define_cor(t, preto); // restaura cor
            }
        }
    }

    // escrita dos numeros horizontalmente em cada linha
    for (i = 0; i < LINHAS; i++) {
        int x = MARGIN_HORIZONTAL - 10;
        int y = MARGIN_VERTICAL + (i * ALTURA_CELULA) + 15;
        Ponto p = { x, y };
        
        // escreve os numeros (vetor), a partir de uma coordenada. escrita horizontal = true
        escreve_numeros(t, nanograma->numeros_linha[i], nanograma->quant_numeros_linha[i], p, true);
        
    }

    // escrita dos numeros verticalmente em cada coluna
    for (j = 0; j < COLUNAS; j++) {
        int x = MARGIN_HORIZONTAL + (j * LARGURA_CELULA) + 15;
        int y = MARGIN_VERTICAL - 10;
        Ponto p = { x, y };

        // escreve os numeros (vetor), a partir de uma coordenada. escrita horizontal = false
        escreve_numeros(t, nanograma->numeros_coluna[j], nanograma->quant_numeros_coluna[j], p, false);
    }
    desenha_botao(button->lista_btn[LIMPAR], "LIMPAR", t);
    desenha_botao(button->lista_btn[NOVO_NANOGRAMA], "NOVO NANOGRAMA", t);
    desenha_botao(button->lista_btn[MENU], "MENU", t);
    if(nanograma->usuario_ganhou == true){
        Ponto txt_ganhou = {(LARGURA_JANELA/2) - 40, 40};
        escreve_texto(t, txt_ganhou, "GANHOU");
    }
    Ponto txt_ganhou = {LARGURA_JANELA - 250, 40};
    escreve_texto(t, txt_ganhou, "Células a pintar :");

    char txt_qnt_celul_pint[LINHAS*COLUNAS];

    sprintf(txt_qnt_celul_pint, "%d", nanograma->quant_celulas_pintar);
    Ponto pt_qnt_celulas_pintar = {LARGURA_JANELA - 100, 40};
    escreve_texto(t, pt_qnt_celulas_pintar, txt_qnt_celul_pint);

}

// escreve os numeros (vetor), a partir de uma coordenada, horizontalmente ou verticalmente
void escreve_numeros(Tela *t, int numeros[], int qtd, Ponto inicio, bool horizontal) {
    int x = inicio.x, y = inicio.y, offset = 20;
    for (int i = qtd - 1; i >= 0; i--) {
        char buffer[10];
        sprintf(buffer, "%d", numeros[i]);
        if (horizontal) {
            if (i > 0) strcat(buffer, ","); // se horizontal separa os numeros com , (menos no numero mais a direita)
            x -= offset;
        } else y -= offset;

        Ponto p = { x, y };
        // escreve o numero na posicao/coordenada definada
        escreve_texto(t, p, buffer);
        
    }
}
void verifica_clique_botao(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button, Ponto mouse, bool *menu_show){
    
    for(int i = 0; i < QNT_BOTOES; i++){
        if(mouse.x >= button->lista_btn[i].pos.x && mouse.x <= button->lista_btn[i].tam.larg + button->lista_btn[i].pos.x &&
            mouse.y >= button->lista_btn[i].pos.y && mouse.y <= button->lista_btn[i].tam.alt + button->lista_btn[i].pos.y ){
                switch(i){
                    case LIMPAR:
                        limpa_tabuleiro(tabuleiro, nanograma);
                        break;
                    case NOVO_NANOGRAMA:
                        novo_jogo(tabuleiro, nanograma);
                        break;
                    case MENU:
                        memset(nanograma, 0, sizeof(Nanograma));
                        *menu_show = true;
                        break;
                    default:
                        break;
                }
            }
    }
    t->_botao = false;
}

void verifica_clique_menu(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button, bool *menu_show){
    Ponto mouse = posicao_mouse(t);
    if(botao_clicado(t)){
        for(int i = 0; i < QNT_BOTOES; i++){
        if(mouse.x >= button->lista_btn[i].pos.x && mouse.x <= button->lista_btn[i].tam.larg + button->lista_btn[i].pos.x &&
            mouse.y >= button->lista_btn[i].pos.y && mouse.y <= button->lista_btn[i].tam.alt + button->lista_btn[i].pos.y ){
                switch(i){
                    case MODO_NORMAL:
                        *menu_show = false;
                        novo_jogo(tabuleiro, nanograma);
                        break;
                    case MODO_CLASSICO:
                        *menu_show = false;
                        novo_jogo(tabuleiro, nanograma);
                        break;
                    default:
                        break;
                }
            }
    }
    }    
    t->_botao = false;

}

// verifica se houve um clique em alguma celula no tabuleiro
void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS], Nanograma *nanograma, Lista_Botao *button, bool *menu_show) {
    Ponto mouse = posicao_mouse(t);

    if (botao_clicado(t)) { // mouse foi clicado
        verifica_clique_botao(t, tabuleiro, nanograma, button, mouse, menu_show);
        int x_clique_tabuleiro = (mouse.x - MARGIN_HORIZONTAL);
        int y_clique_tabuleiro = (mouse.y - MARGIN_VERTICAL);

        // verifica se o clique foi dentro do tabuleiro
        if (x_clique_tabuleiro >= 0 && y_clique_tabuleiro >= 0) {
            // descobre qual celula do tabuleiro foi clicada
            int i = (int) (y_clique_tabuleiro/ ALTURA_CELULA);
            int j = (int) (x_clique_tabuleiro/ LARGURA_CELULA);

            // verifica se eh uma celula valida
            if (i >= 0 && i < LINHAS && j >=0 && j < COLUNAS) {
                cicla_estado_celula(&tabuleiro[i][j]);

                /* dica: aqui eh um bom lugar para verificar se o usuario venceu :) */
                if(verifica_vitoria(nanograma, tabuleiro)){                    
                    nanograma->usuario_ganhou = true;
                }
            }
        }
        t->_botao = false; // clique ja foi tratado!
    }
}

// cicla o estado da celula
void cicla_estado_celula(Celula *c) {
    // alternativa: c.estado = (c.estado + 1) % 3;
    // cicla os estados da celula
    switch (c->estado) {
        // se celula esta vazia, fica colorida
        case VAZIO: c->estado = COLORIDO;
            break;
        // se celula esta colorida, fica alerta (x)
        case COLORIDO: c->estado = ALERTA;
            break;
        // se celula esta alerta, volta a ficar vazia
        case ALERTA: c->estado = VAZIO;
            break;
    }
}
