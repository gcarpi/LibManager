/*************************************************************
* UNIVERSIDADE PAULISTA                                      *
* INSTITUTO DE CIENCIAS EXATAS                               *
*                                                            *
* Graduação em análise e desenvolvimento de sistemas         *
* Projeto multidisciplinar 2015/2 semestre                   *
* Gerenciador de Biblioteca                                  *
*                                                            *
* LibManager.c                                               *
*                                                            *
* Frederico, Guilherme, Matheus, Rafael, Ramon. - jul/15     *
*************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <ncurses.h>

#define KEY_ESC    27
#define KEY_ENVIAR 10

#define MAX_BUFFER_SERIE     15
#define MAX_BUFFER_AUTOR     70
#define MAX_BUFFER_TITULO    70
#define MAX_BUFFER_SUBTITULO 70
#define MAX_BUFFER_EDITORA   30
#define MAX_BUFFER_AREA      20
#define MAX_BUFFER_EDICAO    5
#define MAX_BUFFER_NOME      30
#define MAX_BUFFER_CPF       15
#define MAX_BUFFER_TELEFONE  15
#define MAX_BUFFER_EMAIL     30
#define MAX_BUFFER_DATA      13

#define PADRAO_BR_PT 1
#define PADRAO_PT_BR 2

#define MODO_USUARIO           1
#define MODO_BIBLIOTECA        2
#define MODO_HISTORICO         3
#define MODO_CONSULTAR_LIVRO   0
#define MODO_CADASTRO_LIVRO    1
#define MODO_ALTERAR_LIVRO     2
#define MODO_LISTAR_LIVRO      3
#define MODO_EMPRESITMO_LIVRO  4
#define MODO_DEVOLUCAO_LIVRO   5
#define MODO_IMPORTAR_LIVRO    6
#define MODO_EXCLUIR_LIVRO     7
#define MODO_CONSULTAR_USUARIO 0
#define MODO_CADASTRO_USUARIO  1
#define MODO_ALTERAR_USUARIO   2
#define MODO_LISTAR_USUARIO    3
#define MODO_EXCLUIR_USUARIO   4


#define ERRO                        1
#define ERRO_ALOCAR_MEMORIA         10
#define ERRO_USUARIO_CADASTRADO     11
#define ERRO_USUARIO_NAO_CADASTRADO 12
#define ERRO_ARQUIVO_NAO_ENCONTRADO 13
#define ERRO_ARQUIVO_VAZIO          14
#define ERRO_CRIAR_ARQUIVO          15
#define ERRO_LIVRO_CADASTRADO       16
#define ERRO_LIVRO_NAO_CADASTRADO   17
#define ERRO_MAX_EMPRESTIMO         18
#define ERRO_CPF_INVALIDO           19
#define ERRO_LISTA_VAZIA            20
#define ERRO_LIVRO_NAO_DISPONIVEL   21
#define ERRO_DEVOLCAO_DATA_ERRADA   22

#define SUCESSO                     0
#define SUCESSO_LIVRO_DISPONIVEL    31
#define SUCESSO_LIVRO_CADASTRADO    32
#define SUCESSO_USUARIO_CADASTRADO  33
#define SUCESSO_ALTERAR_LIVRO       34
#define SUCESSO_ALTERAR_USUARIO     35
#define SUCESSO_ALTERAR_HISTORICO   36
#define SUCESSO_EMPRESTIMO          37
#define SUCESSO_EXCLUIR_LIVRO       38
#define SUCESSO_EXCLUIR_USUARIO     39
#define SUCESSO_EXCLUIR_HISTORICO   40
#define SUCESSO_IMPORTAR_LIVROS     41

typedef struct _livro
{
    char serie[MAX_BUFFER_SERIE];
    char autor[MAX_BUFFER_AUTOR];
    char titulo[MAX_BUFFER_TITULO];
    char subtitulo[MAX_BUFFER_SUBTITULO];
    char editora[MAX_BUFFER_EDITORA];
    char area[MAX_BUFFER_AREA];
    char edicao[MAX_BUFFER_EDICAO];
    struct _livro *prox;
    struct _livro *ante;

}LIVRO;

typedef struct _usuario
{
    char nome[MAX_BUFFER_NOME];
    char cpf[MAX_BUFFER_CPF];
    char telefone[MAX_BUFFER_TELEFONE];
    char email[MAX_BUFFER_EMAIL];
    struct _usuario *prox;
    struct _usuario *ante;

}USUARIO;

typedef struct _historico
{
    char cpf[MAX_BUFFER_CPF];
    char serie[MAX_BUFFER_SERIE];
    char data_emp[MAX_BUFFER_DATA];
    char data_dev[MAX_BUFFER_DATA];
    struct _historico *next;

}HISTORICO;

typedef enum _opcoes
{
    NADA,
    MENU_USUARIO,
    MENU_LIVRO,
    MENU_SOBRE

}OPCOES;

static int abrir_criar_fluxo();
static int consulta_fluxo(void **, const int8_t);
static int inserir_fluxo(void **, void *, const int8_t);
static int alterar_fluxo(void **, void *, const int8_t);
static int excluir_fluxo(void **, void *, const int8_t);

static int inserir_lista(void **, void *, const int8_t);
static int liberar_lista(void **, const int8_t);

static void criar_menu(WINDOW *);
static WINDOW ** criar_itens_menu(const int8_t);
static int scroll_menu(WINDOW **, const int8_t);
static void deletar_itens_menu(WINDOW **, const int8_t);
static int tela_user(USUARIO **, const int8_t);
static int tela_livro(LIVRO **, USUARIO **, HISTORICO **, const int8_t);
static int tela_principal(WINDOW *);
static int tela_mensagem(int *);
static int tela_sobre(void);


static int le_cpf(WINDOW *, char *);
static int le_telefone(WINDOW *, char *);
static int le_caract(WINDOW *, char *, const int8_t);
static int select_buttao(WINDOW *, WINDOW *, WINDOW *);
static int limpar_tela(void);
static int verificar_user(USUARIO *, char *);
static int verificar_livro(LIVRO *, char *);
static int consultar_livro(LIVRO *, HISTORICO *, void *);
static int verificar_historico(HISTORICO *, void *);
static int validar_cpf(char *);
static int calcular_data(char*, int , int , int );
static int des_criptografar(char *);
static int import_livros(LIVRO **, const char *);


int main()
{
    auto int sair(void);
    WINDOW *menu = NULL, *exicao = NULL, **itensmenu = NULL;
    MEVENT event;
    register int key = 0;
    int resultado = 0;
    OPCOES opcoes = NADA;
    LIVRO *livro = NULL;
    USUARIO *usuario = NULL;
    HISTORICO *historico = NULL;

    abrir_criar_fluxo();

    consulta_fluxo((void*)&livro,MODO_BIBLIOTECA);
    consulta_fluxo((void*)&usuario,MODO_USUARIO);
    consulta_fluxo((void*)&historico,MODO_HISTORICO);

    initscr();
    start_color();
    cbreak();

    init_pair(PADRAO_BR_PT,COLOR_WHITE,COLOR_BLACK);
    init_pair(PADRAO_PT_BR,COLOR_BLACK,COLOR_WHITE);

    curs_set(FALSE);
    noecho();
    keypad(stdscr,TRUE);
    mousemask(ALL_MOUSE_EVENTS,NULL);
    bkgd(COLOR_PAIR(PADRAO_PT_BR));

    menu = subwin(stdscr,1,80,0,0);
    criar_menu(menu);

    tela_principal(exicao);
    refresh();

    while ((key = getch()) != KEY_ESC)
    {

        if (key == KEY_MOUSE)
        {
            if (getmouse(&event) == OK)
            {
                opcoes = NADA;

                if (event.bstate & BUTTON1_CLICKED)
                    if (event.y < 1)
                    {
                        if (event.x < 15)
                            opcoes = MENU_USUARIO;
                        else if ((event.x > 15) && (event.x < 30))
                            opcoes = MENU_LIVRO;
                        else if ((event.x > 30) && (event.x < 45))
                            opcoes = MENU_SOBRE;
                        else if ((event.x > 75) && (event.x < 80))
                            sair();
                    }
            }
        }
        if ((key == KEY_F(1)) || (opcoes == MENU_USUARIO))
        {
            itensmenu = criar_itens_menu(1);

            switch (scroll_menu(itensmenu,5))
            {
                case 0:
                {
                    resultado = tela_user(&usuario,MODO_CONSULTAR_USUARIO);
                }
                break;
                case 1:
                {
                    resultado = tela_user(&usuario,MODO_CADASTRO_USUARIO);
                }
                break;
                case 2:
                {
                    resultado = tela_user(&usuario,MODO_ALTERAR_USUARIO);
                }
                break;
                case 3:
                {
                    resultado = tela_user(&usuario,MODO_LISTAR_USUARIO);
                }
                break;
                case 4:
                {
                    resultado = tela_user(&usuario,MODO_EXCLUIR_USUARIO);
                }
                break;
            }

            deletar_itens_menu(itensmenu,6);
        }
        else if ((key == KEY_F(2)) || (opcoes == MENU_LIVRO))
        {
            itensmenu = criar_itens_menu(2);

            switch (scroll_menu(itensmenu,8))
            {
                case 0:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_CONSULTAR_LIVRO);
                }
                break;
                case 1:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_CADASTRO_LIVRO);
                }
                break;
                case 2:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_ALTERAR_LIVRO);
                }
                break;
                case 3:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_LISTAR_LIVRO);
                }
                break;
                case 4:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_EMPRESITMO_LIVRO);
                }
                break;
                case 5:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_DEVOLUCAO_LIVRO);
                }
                break;
                case 6:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_IMPORTAR_LIVRO);
                }
                break;
                case 7:
                {
                    resultado = tela_livro(&livro,&usuario,&historico,MODO_EXCLUIR_LIVRO);
                }
                break;
            }
            deletar_itens_menu(itensmenu,9);
        }
        else if ((key == KEY_F(3)) || (opcoes == MENU_SOBRE))
            tela_sobre();

        tela_mensagem(&resultado);

        limpar_tela();
    }

    sair();

	int sair(void)
	{
	    delwin(menu);
        endwin();
        liberar_lista((void*)&livro,MODO_BIBLIOTECA);
        liberar_lista((void*)&usuario,MODO_USUARIO);
        liberar_lista((void*)&historico,MODO_HISTORICO);
        exit(EXIT_SUCCESS);
	}
}


static int abrir_criar_fluxo()
/*
    Está função abre um arquivo se existir,
    caso contrário criar um arquivo no diretório
    corrente.
*/
{
    FILE *arq = NULL;

    if (!(arq = fopen("biblioteca.dat","rb")))
    {
        arq = fopen("biblioteca.dat","wb");
        if(!arq)
            return ERRO_CRIAR_ARQUIVO;
    }else
        fclose(arq);

    if (!(arq = fopen("usuario.dat","rb")))
    {
        arq = fopen("usuario.dat","wb");
        if(!arq)
            return ERRO_CRIAR_ARQUIVO;
    }else
        fclose(arq);

    if (!(arq = fopen("historico.dat","rb")))
    {
        arq = fopen("historico.dat","wb");
        if(!arq)
            return ERRO_CRIAR_ARQUIVO;
    }else
        fclose(arq);

    return SUCESSO;
}

static int consulta_fluxo(void **lista, const int8_t modo)
/*
    Está função percorre o arquivo em busca de dados,
    caso os mesmos estejam disponíveis adiciona eles,
    em uma lista.
*/
{
    switch (modo)
    {
        case MODO_BIBLIOTECA:
        {
            LIVRO *livro = (LIVRO*) malloc(sizeof(LIVRO));
            FILE *bibli = fopen("biblioteca.dat","rb");

            if (!bibli)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            if (!livro)
                return ERRO_ALOCAR_MEMORIA;

            rewind(bibli);

            liberar_lista(lista,MODO_BIBLIOTECA);

            while (fread(livro,sizeof(LIVRO),1,bibli))
                inserir_lista(lista,livro,MODO_BIBLIOTECA);

            free(livro);
            fclose(bibli);
        }
        break;
        case MODO_USUARIO:
        {
            USUARIO *usuario = (USUARIO*) malloc(sizeof(USUARIO));
            FILE *user = fopen("usuario.dat","rb");

            if (!user)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;

            if (!usuario)
                return ERRO_ALOCAR_MEMORIA;

            rewind(user);

            liberar_lista(lista,MODO_USUARIO);

            while (fread(usuario,sizeof(USUARIO),1,user))
                inserir_lista(lista,usuario,MODO_USUARIO);

            free(usuario);
            fclose(user);
        }
        break;
        case MODO_HISTORICO:
        {
            HISTORICO *historico = (HISTORICO*) malloc(sizeof(HISTORICO));
            FILE *hist = fopen("historico.dat","rb");

            if (!hist)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;

            if (!historico)
                return ERRO_ALOCAR_MEMORIA;

            rewind(hist);

            liberar_lista(lista,MODO_HISTORICO);

            while (fread(historico,sizeof(HISTORICO),1,hist))
                inserir_lista(lista,historico,MODO_HISTORICO);

            free(historico);
            fclose(hist);
        }
        break;
    }

    return SUCESSO;
}

static int inserir_fluxo(void **lista, void *dado, const int8_t modo)
/*
    Está função insere valores no arquivo.
*/
{
    switch (modo)
    {
        case MODO_BIBLIOTECA:
        {
            LIVRO *aux = *(LIVRO**)lista;
            FILE *bibli = fopen("biblioteca.dat","r+b");

            if (!bibli)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;

            fseek(bibli,0,SEEK_END);

            while (aux)
            {
                if (!strcmp(aux->serie,(((LIVRO*)dado)->serie)))
                    return ERRO_LIVRO_CADASTRADO;
                aux = aux->prox;
            }

            fwrite(dado,sizeof(LIVRO),1,bibli);
            fflush(bibli);

            fclose(bibli);

            consulta_fluxo(lista,MODO_BIBLIOTECA);

            return SUCESSO_LIVRO_CADASTRADO;
        }
        case MODO_USUARIO:
        {
            USUARIO *aux = *(USUARIO**)lista;
            FILE *user = fopen("usuario.dat","r+b");

            if (!user)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;

            fseek(user,0,SEEK_END);

            while (aux)
            {
                if (!strcmp(aux->cpf,(((USUARIO*)dado)->cpf)))
                    return ERRO_USUARIO_CADASTRADO;
                aux = aux->prox;
            }

            fwrite(dado,sizeof(USUARIO),1,user);
            fflush(user);

            fclose(user);

            liberar_lista(lista,MODO_USUARIO);

            consulta_fluxo(lista,MODO_USUARIO);

            return SUCESSO_USUARIO_CADASTRADO;
        }
        case MODO_HISTORICO:
        {
            FILE *hist = fopen("historico.dat","r+b");

            if (!hist)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;

            fseek(hist,0,SEEK_END);

            fwrite(dado,sizeof(HISTORICO),1,hist);
            fflush(hist);

            fclose(hist);

            consulta_fluxo(lista,MODO_HISTORICO);

            return SUCESSO_EMPRESTIMO;
        }
    }

    return SUCESSO;
}

static int alterar_fluxo(void **lista, void *dado, const int8_t modo)
/*
    Está função alterar o valor atual do dado para um
    novo valor.
*/
{
    switch (modo)
    {
        case MODO_BIBLIOTECA:
        {
            LIVRO *livros = NULL, *aux = NULL;
            FILE *bibli = fopen("biblioteca.dat","r+b");
            long int pos = 0;

            livros = *(LIVRO**)lista;
            aux = (LIVRO*) calloc(1,sizeof(LIVRO));

            if (!bibli)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            else if (!aux)
                return ERRO_ALOCAR_MEMORIA;
            else if (!livros)
                return ERRO_LISTA_VAZIA;
            else if (verificar_livro(livros,((LIVRO*)dado)->serie) != SUCESSO_LIVRO_CADASTRADO)
                return ERRO_LIVRO_NAO_CADASTRADO;

            rewind(bibli);

            while (fread(aux,sizeof(LIVRO),1,bibli))
            {
                if (!strcmp(aux->serie,((LIVRO*)dado)->serie))
                {
                    pos = ftell(bibli) - sizeof(LIVRO);
                    break;
                }
            }

            fseek(bibli,pos,SEEK_SET);
            fwrite(dado,sizeof(LIVRO),1,bibli);

            free(aux);
            fclose(bibli);

            consulta_fluxo(lista,MODO_BIBLIOTECA);

            return SUCESSO_ALTERAR_LIVRO;
        }
        case MODO_USUARIO:
        {
            USUARIO *usuario = NULL, *aux = NULL;
            FILE *user = fopen("usuario.dat","r+b");
            long int pos = 0;

            usuario = *(USUARIO**)lista;
            aux = (USUARIO*) calloc(1,sizeof(USUARIO));

            if (!user)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            else if (!aux)
                return ERRO_ALOCAR_MEMORIA;
            else if (!usuario)
                return ERRO_LISTA_VAZIA;
            else if (verificar_user(usuario,((USUARIO*)dado)->cpf) != SUCESSO_USUARIO_CADASTRADO)
                return ERRO_USUARIO_NAO_CADASTRADO;

            rewind(user);

            while (fread(aux,sizeof(USUARIO),1,user))
            {
                if (!strcmp(aux->cpf,((USUARIO*)dado)->cpf))
                {
                    pos = ftell(user) - sizeof(USUARIO);
                    break;
                }

            }

            fseek(user,pos,SEEK_SET);
            fwrite(dado,sizeof(USUARIO),1,user);

            free(aux);
            fclose(user);

            consulta_fluxo(lista,MODO_USUARIO);

            return SUCESSO_ALTERAR_USUARIO;
        }
        case MODO_HISTORICO:
        {
            HISTORICO *historico = NULL, *aux = NULL;
            FILE *hist = fopen("historico.dat","r+b");
            long int pos = 0;

            historico = *(HISTORICO**)lista;
            aux = (HISTORICO*) calloc(1,sizeof(HISTORICO));

            if (!hist)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            else if (!aux)
                return ERRO_ALOCAR_MEMORIA;
            else if (!historico)
                return ERRO_LISTA_VAZIA;

            rewind(hist);

            while (fread(aux,sizeof(HISTORICO),1,hist))
            {
                if (!strcmp(historico->serie,dado))
                {
                    pos = ftell(hist) - sizeof(HISTORICO);
                }
                historico = historico->next;
            }

            fseek(hist,pos,SEEK_SET);
            fwrite(dado,sizeof(HISTORICO),1,hist);

            free(aux);
            fclose(hist);

            consulta_fluxo(lista,MODO_HISTORICO);

            return SUCESSO_ALTERAR_HISTORICO;
        }
    }

    return SUCESSO;
}

static int excluir_fluxo(void **lista, void *dado, const int8_t modo)
/*
    Está função excluir um determinado valor no arquivo.
*/
{
    switch (modo)
    {
        case MODO_BIBLIOTECA:
        {
            LIVRO *livros = NULL, *aux = NULL, dado_null = {"\0","\0","\0","\0","\0","\0","\0",NULL,NULL};
            FILE *bibli = fopen("biblioteca.dat","r+b");
            long int pos = 0;
            int result = 0;

            if (!((LIVRO*)dado)->serie[0])
                return ERRO_LIVRO_NAO_CADASTRADO;

            livros = *(LIVRO**)lista;
            aux = (LIVRO*) calloc(1,sizeof(LIVRO));

            if (!bibli)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            else if (!aux)
                return ERRO_ALOCAR_MEMORIA;
            else if (!livros)
                return ERRO_LISTA_VAZIA;

            rewind(bibli);

            while (fread(aux,sizeof(LIVRO),1,bibli))
            {
                if (!strcmp(aux->serie,((LIVRO*)dado)->serie))
                {
                    pos = ftell(bibli) - sizeof(LIVRO);
                    result = SUCESSO_EXCLUIR_LIVRO;
                    break;
                }
                result = ERRO_LIVRO_NAO_CADASTRADO;
            }

            if (result != ERRO_LIVRO_NAO_CADASTRADO)
            {
                fseek(bibli,pos,SEEK_SET);
                fwrite(&dado_null,sizeof(LIVRO),1,bibli);
            }

            free(aux);
            fclose(bibli);

            consulta_fluxo(lista,MODO_BIBLIOTECA);

            return result;
        }
        case MODO_USUARIO:
        {
            USUARIO *usuario = NULL, *aux = NULL, dado_null = {"\0","\0","\0","\0",NULL,NULL};
            FILE *user = fopen("usuario.dat","r+b");
            long int pos = 0;
            int result = 0;

            usuario = *(USUARIO**)lista;
            aux = (USUARIO*) calloc(1,sizeof(USUARIO));

            if (!user)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            else if (!aux)
                return ERRO_ALOCAR_MEMORIA;
            else if (!usuario)
                return ERRO_LISTA_VAZIA;

            rewind(user);

            while (fread(aux,sizeof(USUARIO),1,user))
            {
                if (!strcmp(aux->cpf,((USUARIO*)dado)->cpf))
                {
                    pos = ftell(user) - sizeof(USUARIO);
                    result = SUCESSO_EXCLUIR_USUARIO;
                    break;
                }
                result = ERRO_USUARIO_NAO_CADASTRADO;
            }

            if (result != ERRO_USUARIO_NAO_CADASTRADO)
            {
                fseek(user,pos,SEEK_SET);
                fwrite(&dado_null,sizeof(USUARIO),1,user);
            }

            free(aux);
            fclose(user);

            consulta_fluxo(lista,MODO_USUARIO);

            return result;
        }
        case MODO_HISTORICO:
        {
            time_t tempo = 0;
            struct tm *info = NULL;
            long int pos;
            int result = 0, dia = 0, mes = 0, ano = 0;
            HISTORICO *historico = NULL, *aux = NULL, dado_null = {"\0","\0","\0","\0",NULL};
            FILE *hist = fopen("historico.dat","r+b");

            historico = *(HISTORICO**)lista;
            aux = (HISTORICO*) calloc(1,sizeof(HISTORICO));

            if (!hist)
                return ERRO_ARQUIVO_NAO_ENCONTRADO;
            else if (!aux)
                return ERRO_ALOCAR_MEMORIA;
            else if (!historico)
                return ERRO_LISTA_VAZIA;

            time(&tempo);
            info = gmtime(&tempo);

            rewind(hist);

            while (fread(aux,sizeof(HISTORICO),1,hist))
            {
                if (!strcmp(aux->serie,((HISTORICO*)dado)->serie) && !strcmp(aux->cpf,((HISTORICO*)dado)->cpf))
                {
                    pos = ftell(hist) - sizeof(HISTORICO);
                    sscanf(aux->data_dev,"%02d\\%02d\\%d",&dia,&mes,&ano);
                    if ((info->tm_mday > dia) && ((info->tm_mon + 1) >= mes) && ((info->tm_year + 1900) >= ano))
                        result = ERRO_DEVOLCAO_DATA_ERRADA;
                    else
                        result = SUCESSO_EXCLUIR_HISTORICO;
                    break;
                }

                result = ERRO_LISTA_VAZIA;
            }

            if (result != ERRO_LISTA_VAZIA)
            {
                fseek(hist,pos,SEEK_SET);
                fwrite(&dado_null,sizeof(HISTORICO),1,hist);
            }

            free(aux);
            fclose(hist);

            consulta_fluxo(lista,MODO_HISTORICO);

            return result;
        }
    }

    return SUCESSO;
}

static int inserir_lista(void **lista, void *dado, const int8_t modo)
/*
    Está função insere valor na lista encadeada.
*/
{
    switch (modo)
    {
        case MODO_BIBLIOTECA:
        {
            LIVRO *novo, *aux, **livro = (LIVRO**)lista;

            if (!((LIVRO*)dado)->serie[0])
                return ERRO_LISTA_VAZIA;

            if (*livro == NULL)
            {
                *livro = (LIVRO*) malloc(sizeof(LIVRO));
                strcpy((*livro)->serie,((LIVRO*)dado)->serie);
                strcpy((*livro)->autor,((LIVRO*)dado)->autor);
                strcpy((*livro)->titulo,((LIVRO*)dado)->titulo);
                strcpy((*livro)->subtitulo,((LIVRO*)dado)->subtitulo);
                strcpy((*livro)->editora,((LIVRO*)dado)->editora);
                strcpy((*livro)->area,((LIVRO*)dado)->area);
                strcpy((*livro)->edicao,((LIVRO*)dado)->edicao);
                (*livro)->prox = NULL;
                (*livro)->ante= NULL;
            }
            else
            {
                aux = *livro;

                while (aux->prox)
                {
                    if(!strcmp(aux->serie,(((LIVRO*)dado)->serie)))
                        return 1;
                    aux = aux->prox;
                }

                novo = (LIVRO*) malloc(sizeof(LIVRO));
                strcpy(novo->serie,((LIVRO*)dado)->serie);
                strcpy(novo->autor,((LIVRO*)dado)->autor);
                strcpy(novo->titulo,((LIVRO*)dado)->titulo);
                strcpy(novo->subtitulo,((LIVRO*)dado)->subtitulo);
                strcpy(novo->editora,((LIVRO*)dado)->editora);
                strcpy(novo->area,((LIVRO*)dado)->area);
                strcpy(novo->edicao,((LIVRO*)dado)->edicao);
                novo->prox = NULL;
                novo->ante = aux;

                aux->prox = novo;

            }

        }
        break;
        case MODO_USUARIO:
        {
            USUARIO *novo, *aux, **user = (USUARIO**)lista;

            if (!((USUARIO*)dado)->cpf[0])
                return ERRO_LISTA_VAZIA;

            if (*user == NULL)
            {
                *user = (USUARIO*) malloc(sizeof(USUARIO));
                strcpy((*user)->cpf,((USUARIO*)dado)->cpf);
                strcpy((*user)->nome,((USUARIO*)dado)->nome);
                strcpy((*user)->telefone,((USUARIO*)dado)->telefone);
                strcpy((*user)->email,((USUARIO*)dado)->email);
                (*user)->prox = NULL;
                (*user)->ante = NULL;
            }
            else
            {
                aux = *user;

                while (aux->prox != NULL)
                {
                    if (!strcmp(aux->cpf,((USUARIO*)dado)->cpf))
                        return 1;
                    aux = aux->prox;
                }

                novo = (USUARIO*) malloc(sizeof(USUARIO));
                strcpy(novo->cpf,((USUARIO*)dado)->cpf);
                strcpy(novo->nome,((USUARIO*)dado)->nome);
                strcpy(novo->telefone,((USUARIO*)dado)->telefone);
                strcpy(novo->email,((USUARIO*)dado)->email);
                novo->prox = NULL;
                novo->ante = aux;

                aux->prox = novo;
            }
        }
        break;
        case MODO_HISTORICO:
        {
            HISTORICO *novo, *aux, **hist = (HISTORICO**)lista;

            if (!((HISTORICO*)dado)->serie[0])
                return ERRO_LISTA_VAZIA;

            if (*hist == NULL)
            {
                *hist = (HISTORICO*) malloc(sizeof(HISTORICO));
                strcpy((*hist)->serie,((HISTORICO*)dado)->serie);
                strcpy((*hist)->cpf,((HISTORICO*)dado)->cpf);
                strcpy((*hist)->data_emp,((HISTORICO*)dado)->data_emp);
                strcpy((*hist)->data_dev,((HISTORICO*)dado)->data_dev);
                (*hist)->next = NULL;
            }
            else
            {
                aux = *hist;

                while (aux->next != NULL)
                    aux = aux->next;

                novo = (HISTORICO*) malloc(sizeof(HISTORICO));
                strcpy(novo->serie,((HISTORICO*)dado)->serie);
                strcpy(novo->cpf,((HISTORICO*)dado)->cpf);
                strcpy(novo->data_emp,((HISTORICO*)dado)->data_emp);
                strcpy(novo->data_dev,((HISTORICO*)dado)->data_dev);
                novo->next = NULL;

                aux->next = novo;
            }
        }
        break;
    }
    return SUCESSO;
}

static int liberar_lista(void **lista, const int8_t modo)
/*
    Está função liberar a memória ocupada
    pela lista encadeada.
*/
{
    switch (modo)
    {
        case MODO_BIBLIOTECA:
        {
            LIVRO *prox = NULL;

            while (*lista != NULL)
            {
                prox = (*(LIVRO**)lista)->prox;
                free(*lista);
                (*lista) = prox;
            }
        }
        break;
        case MODO_USUARIO:
        {
            USUARIO *prox = NULL;

            while (*lista != NULL)
            {
                prox = (*(USUARIO**)lista)->prox;
                free(*lista);
                (*lista) = prox;
            }
        }
        break;
        case MODO_HISTORICO:
        {
            HISTORICO *prox = NULL;

            while (*lista != NULL)
            {
                prox = (*(HISTORICO**)lista)->next;
                free(*lista);
                (*lista) = prox;
            }
        }
        break;
    }

    return SUCESSO;
}

static void criar_menu(WINDOW * menu)
/*
    Está função cria um menu principal
    na tela inicial.
*/
{
    wbkgd(menu,COLOR_PAIR(PADRAO_BR_PT) | A_BOLD);
    mvwprintw(menu,0,0,"[F1] USUARIO");
    mvwprintw(menu,0,15,"|");
    mvwprintw(menu,0,17,"[F2] LIVROS");
    mvwprintw(menu,0,30,"|");
    mvwprintw(menu,0,33,"[F3] SOBRE");
    mvwprintw(menu,0,45,"|");
    mvwprintw(menu,0,69,"[ESC] SAIR");
    wrefresh(menu);
}

static WINDOW ** criar_itens_menu(const int8_t modo)
/*
    Está função cria itens para o
    menu principal.
*/
{
    int i;
    char *itens_user[] = {"Consultar","Cadastrar","Alterar","Listar","Excluir"};
    char *itens_livros[] = {"Consultar","Cadastrar","Alterar","Listar","Emprestimo","Devolucao","Importar","Excluir"};
    WINDOW **itensmenu;

    switch (modo)
    {
        case 1:
        {
            itensmenu = (WINDOW **) malloc(6 * sizeof(WINDOW *));

            itensmenu[0] = newwin(7,15,1,0);
            wbkgd(itensmenu[0],COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(itensmenu[0],ACS_VLINE,ACS_HLINE);

            for (i = 1; i < 6; i++)
                itensmenu[i] = subwin(itensmenu[0],1,13,i+1,1);

            for (i = 1; i < 6; i++)
                wprintw(itensmenu[i],"%s",itens_user[i-1]);
        }
        break;
        case 2:
        {
            itensmenu = (WINDOW **) malloc(9 * sizeof(WINDOW *));

            itensmenu[0] = newwin(10,15,1,16);
            wbkgd(itensmenu[0],COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(itensmenu[0],ACS_VLINE,ACS_HLINE);

            for (i = 1; i < 9; i++)
                itensmenu[i] = subwin(itensmenu[0],1,13,i+1,17);

            for (i = 1; i < 9; i++)
                wprintw(itensmenu[i],"%s",itens_livros[i-1]);
        }
        break;
    }

    wbkgd(itensmenu[1],COLOR_PAIR(PADRAO_PT_BR)|A_BOLD);
    wrefresh(itensmenu[0]);

    return itensmenu;
}

static int scroll_menu(WINDOW **itensmenu, const int8_t numeroitens)
/*
    Está função permiti realizar o scroll no
    itens do menu.
*/
{
    MEVENT event;
    register int key, selecionado = 0;

    for (;;)
    {
        key = getch();

        if (key == KEY_MOUSE)
        {
            if (getmouse(&event) == OK)
            {
                if (event.bstate & BUTTON1_CLICKED)
                {
                    if (event.x < 15)
                        return (event.y - 2) % numeroitens;
                    else if ((event.x > 15) && (event.x < 30))
                        return (event.y - 2) % numeroitens;
                }

            }
        }

        if (key == KEY_DOWN || key == KEY_UP)
        {
            wbkgd(itensmenu[selecionado + 1],COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            wnoutrefresh(itensmenu[selecionado + 1]);

            if (key == KEY_DOWN)
                selecionado = (selecionado + 1) % numeroitens;
            else
                selecionado = (selecionado + numeroitens - 1) % numeroitens;

            wbkgd(itensmenu[selecionado + 1],COLOR_PAIR(PADRAO_PT_BR)|A_BOLD);
            wnoutrefresh(itensmenu[selecionado + 1]);

            doupdate();
        }
        else if (key == KEY_ENVIAR)
            return selecionado;
        else
            return -1;
    }
}

static void deletar_itens_menu(WINDOW **itensmenu, const int8_t cont_itens)
/*
    Está liberar a memória ocupada pelas
    funções de criação de itens de menu.
*/
{
    if (itensmenu != NULL)
    {
        int i;
        for (i = 0; i < cont_itens; i++)
            delwin(itensmenu[i]);
        free(itensmenu);
    }
}

static int tela_user(USUARIO **user, const int8_t modo)
/*
    Está função criar um tela 8X30
    para gerenciar os usuários.
*/
{
    WINDOW *janela_usuario = NULL, *borda_nome = NULL, *borda_cpf= NULL, *borda_telefone = NULL, *borda_email = NULL,
    *nome = NULL, *cpf = NULL, *telefone = NULL, *email = NULL, *botao_ok = NULL, *botao_cancelar = NULL, *botao_ant = NULL, *botao_prox = NULL;
    USUARIO *usuario;
    int result = 0;

    usuario = (USUARIO*) calloc(1,sizeof(USUARIO));
    if (!usuario)
        return ERRO_ALOCAR_MEMORIA;

    switch(modo)
    {
        case MODO_CONSULTAR_USUARIO:
        {
            janela_usuario = newwin(6,30,2,20);
            wbkgd(janela_usuario,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_usuario,ACS_VLINE,ACS_HLINE);
            wprintw(janela_usuario,"Consultar Usuario");

            borda_cpf = subwin(janela_usuario,3,28,3,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,4,22);

            botao_ok = subwin(janela_usuario,1,4,6,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_usuario,1,10,6,35);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_usuario);

            le_cpf(cpf,usuario->cpf);

            if (validar_cpf(usuario->cpf))
            {
                result =  ERRO_CPF_INVALIDO;
                break;
            }

            des_criptografar(usuario->cpf);

            if (!select_buttao(janela_usuario,botao_ok,botao_cancelar))
                result = verificar_user(*(void**)user,usuario->cpf);
        }
        break;
        case MODO_CADASTRO_USUARIO:
        {
            janela_usuario = newwin(16,30,2,20);
            wbkgd(janela_usuario,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_usuario,ACS_VLINE,ACS_HLINE);
            wprintw(janela_usuario,"Cadastro Usuario");

            borda_nome = subwin(janela_usuario,3,28,3,21);
            box(borda_nome,ACS_VLINE,ACS_HLINE);
            wprintw(borda_nome,"Nome");

            nome = subwin(borda_nome,1,26,4,22);

            borda_cpf = subwin(janela_usuario,3,28,6,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,7,22);

            borda_telefone = subwin(janela_usuario,3,28,9,21);
            box(borda_telefone,ACS_VLINE,ACS_HLINE);
            wprintw(borda_telefone,"Telefone");

            telefone = subwin(borda_telefone,1,26,10,22);

            borda_email = subwin(janela_usuario,3,28,12,21);
            box(borda_email,ACS_VLINE,ACS_HLINE);
            wprintw(borda_email,"Email");

            email = subwin(borda_email,1,26,13,22);

            botao_ok = subwin(janela_usuario,1,4,16,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_usuario,1,10,16,35);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_usuario);

            le_caract(nome,usuario->nome,MAX_BUFFER_NOME);
            le_cpf(cpf,usuario->cpf);
            le_telefone(telefone,usuario->telefone);
            le_caract(email,usuario->email,MAX_BUFFER_EMAIL);

            if (validar_cpf(usuario->cpf))
            {
                result =  ERRO_CPF_INVALIDO;
                break;
            }


            des_criptografar(usuario->nome);
            des_criptografar(usuario->cpf);
            des_criptografar(usuario->telefone);
            des_criptografar(usuario->email);

            if (!select_buttao(janela_usuario,botao_ok,botao_cancelar))
                result = inserir_fluxo((void**)user,usuario,MODO_USUARIO);
        }
        break;
        case MODO_ALTERAR_USUARIO:
        {
            janela_usuario = newwin(16,30,2,20);
            wbkgd(janela_usuario,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_usuario,ACS_VLINE,ACS_HLINE);
            wprintw(janela_usuario,"Alterar Usuario");

            borda_nome = subwin(janela_usuario,3,28,3,21);
            box(borda_nome,ACS_VLINE,ACS_HLINE);
            wprintw(borda_nome,"Nome");

            nome = subwin(borda_nome,1,26,4,22);

            borda_cpf = subwin(janela_usuario,3,28,6,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,7,22);

            borda_telefone = subwin(janela_usuario,3,28,9,21);
            box(borda_telefone,ACS_VLINE,ACS_HLINE);
            wprintw(borda_telefone,"Telefone");

            telefone = subwin(borda_telefone,1,26,10,22);

            borda_email = subwin(janela_usuario,3,28,12,21);
            box(borda_email,ACS_VLINE,ACS_HLINE);
            wprintw(borda_email,"Email");

            email = subwin(borda_email,1,26,13,22);

            botao_ok = subwin(janela_usuario,1,4,16,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_usuario,1,10,16,35);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_usuario);

            le_caract(nome,usuario->nome,MAX_BUFFER_NOME);
            le_cpf(cpf,usuario->cpf);
            le_telefone(telefone,usuario->telefone);
            le_caract(email,usuario->email,MAX_BUFFER_EMAIL);

            if (validar_cpf(usuario->cpf))
            {
                result =  ERRO_CPF_INVALIDO;
                break;
            }

            des_criptografar(usuario->nome);
            des_criptografar(usuario->cpf);
            des_criptografar(usuario->telefone);
            des_criptografar(usuario->email);

            if (!select_buttao(janela_usuario,botao_ok,botao_cancelar))
                result = alterar_fluxo((void**)user,usuario,MODO_USUARIO);
        }
        break;
        case MODO_LISTAR_USUARIO:
        {
            USUARIO *usuar = *user;
            int key = 0;
            MEVENT event;

            janela_usuario = newwin(16,50,2,10);
            wbkgd(janela_usuario,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_usuario,ACS_VLINE,ACS_HLINE);
            wprintw(janela_usuario,"Lista Usuario");

            borda_nome = subwin(janela_usuario,3,28,3,21);
            box(borda_nome,ACS_VLINE,ACS_HLINE);
            wprintw(borda_nome,"Nome");

            nome = subwin(borda_nome,1,26,4,22);

            borda_cpf = subwin(janela_usuario,3,28,6,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,7,22);

            borda_telefone = subwin(janela_usuario,3,28,9,21);
            box(borda_telefone,ACS_VLINE,ACS_HLINE);
            wprintw(borda_telefone,"Telefone");

            telefone = subwin(borda_telefone,1,26,10,22);

            borda_email = subwin(janela_usuario,3,28,12,21);
            box(borda_email,ACS_VLINE,ACS_HLINE);
            wprintw(borda_email,"Email");

            email = subwin(borda_email,1,26,13,22);

            botao_ant = subwin(janela_usuario,1,2,9,15);
            wprintw(botao_ant,"<");

            botao_prox = subwin(janela_usuario,1,2,9,54);
            wprintw(botao_prox,">");

            botao_ok = subwin(janela_usuario,1,4,16,32);
            wprintw(botao_ok,"[OK]");

            limpar_tela();
            keypad(janela_usuario,TRUE);
            mousemask(ALL_MOUSE_EVENTS,NULL);
            wrefresh(janela_usuario);

            if (!usuar)
            {
                result =  ERRO_LISTA_VAZIA;
                break;
            }

            for (;;)
            {
                des_criptografar(usuar->nome);
                des_criptografar(usuar->cpf);
                des_criptografar(usuar->telefone);
                des_criptografar(usuar->email);

                werase(nome);
                werase(cpf);
                werase(telefone);
                werase(email);

                mvwprintw(nome,0,0,"%s",usuar->nome);
                mvwprintw(cpf,0,0,"%s",usuar->cpf);
                mvwprintw(telefone,0,0,"%s",usuar->telefone);
                mvwprintw(email,0,0,"%s",usuar->email);
                touchwin(janela_usuario);

                des_criptografar(usuar->nome);
                des_criptografar(usuar->cpf);
                des_criptografar(usuar->telefone);
                des_criptografar(usuar->email);

                key = wgetch(janela_usuario);

                if (key == KEY_MOUSE)
                {
                    if (getmouse(&event) == OK)
                    {
                        if (event.bstate & BUTTON1_CLICKED)
                        {
                            if ((event.y == 9) && (event.x == 15))
                            {
                                if(usuar->ante)
                                usuar = usuar->ante;
                            }
                            else if ((event.y == 9) && (event.x == 54))
                            {
                                if(usuar->prox)
                                usuar = usuar->prox;
                            }
                            else if ((event.y == 16) && ((event.x > 32) && (event.x < 37)))
                            {
                                break;
                            }
                        }
                    }
                }

                if (key == KEY_LEFT)
                {
                    if(usuar->ante)
                        usuar = usuar->ante;
                }
                else if (key == KEY_RIGHT)
                {
                    if(usuar->prox)
                        usuar = usuar->prox;
                }
                else if (key == KEY_ENVIAR)
                    break;

            }

        }
        break;
        case MODO_EXCLUIR_USUARIO:
        {
            janela_usuario = newwin(6,30,2,20);
            wbkgd(janela_usuario,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_usuario,ACS_VLINE,ACS_HLINE);
            wprintw(janela_usuario,"Excluir Usuario");

            borda_cpf = subwin(janela_usuario,3,28,3,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,4,22);

            botao_ok = subwin(janela_usuario,1,4,6,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_usuario,1,10,6,35);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_usuario);

            le_cpf(cpf,usuario->cpf);

            if (validar_cpf(usuario->cpf))
            {
                result =  ERRO_CPF_INVALIDO;
                break;
            }

            des_criptografar(usuario->cpf);

            if (!select_buttao(janela_usuario,botao_ok,botao_cancelar))
                result = excluir_fluxo((void**)user,(void*)usuario,MODO_USUARIO);
        }
        break;
    }

    free(usuario);

    return result;

}

static int tela_livro(LIVRO **livros, USUARIO **usuario, HISTORICO **hist, const int8_t modo)
/*
    Está função cria uma tela 20X60
    para gerenciar os livros da
    biblioteca.
*/
{
    WINDOW *janela_livro = NULL, *borda_serie = NULL, *borda_editora = NULL, *borda_area = NULL, *borda_autor = NULL, *borda_edicao = NULL, *borda_titulo = NULL, *borda_subtitulo = NULL, *borda_cpf = NULL,
    *serie = NULL, *editora = NULL, *area = NULL, *autor = NULL, *edicao = NULL, *titulo = NULL, *subtitulo = NULL, *cpf = NULL, *botao_ok = NULL, *botao_cancelar = NULL, *botao_ant = NULL, *botao_prox = NULL;
    LIVRO *livro = NULL;
    HISTORICO *historico = NULL;
    int result = 0;

    livro = (LIVRO*) calloc(1,sizeof(LIVRO));
    if (!livro)
        return ERRO_ALOCAR_MEMORIA;

    historico = (HISTORICO*) calloc(1,sizeof(HISTORICO));
    if (!historico)
        return ERRO_ALOCAR_MEMORIA;

    switch(modo)
    {
        case MODO_CONSULTAR_LIVRO:
        {
            janela_livro = newwin(9,70,2,7);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Consultar Livro");

            borda_titulo = subwin(janela_livro,3,68,3,8);
            box(borda_titulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_titulo,"Titulo");

            titulo = subwin(borda_titulo,1,66,4,9);

            borda_subtitulo = subwin(janela_livro,3,68,6,8);
            box(borda_subtitulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_subtitulo,"SubTitulo");

            subtitulo = subwin(borda_subtitulo,1,66,7,9);

            botao_ok = subwin(janela_livro,1,4,9,30);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,9,40);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_livro);

            le_caract(titulo,livro->titulo,MAX_BUFFER_TITULO);
            le_caract(subtitulo,livro->subtitulo,MAX_BUFFER_SUBTITULO);

            if(!select_buttao(janela_livro,botao_ok,botao_cancelar))
                result = consultar_livro(*livros,*hist,livro);
        }
        break;
        case MODO_CADASTRO_LIVRO:
        {
            janela_livro = newwin(17,70,2,7);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Cadastro Livro");

            borda_serie = subwin(janela_livro,3,18,3,8);
            box(borda_serie,ACS_VLINE,ACS_HLINE);
            wprintw(borda_serie,"Serie");

            serie = subwin(borda_serie,1,16,4,9);

            borda_editora = subwin(janela_livro,3,17,3,26);
            box(borda_editora,ACS_VLINE,ACS_HLINE);
            wprintw(borda_editora,"Editora");

            editora = subwin(borda_editora,1,15,4,27);

            borda_area = subwin(janela_livro,3,19,3,43);
            box(borda_area,ACS_VLINE,ACS_HLINE);
            wprintw(borda_area,"Area");

            area = subwin(borda_area,1,14,4,44);

            borda_edicao = subwin(janela_livro,3,14,3,62);
            box(borda_edicao,ACS_VLINE,ACS_HLINE);
            wprintw(borda_edicao,"Edicao");

            edicao = subwin(borda_edicao,1,12,4,63);

            borda_autor = subwin(janela_livro,3,68,6,8);
            box(borda_autor,ACS_VLINE,ACS_HLINE);
            wprintw(borda_autor,"Autor");

            autor = subwin(borda_autor,1,66,7,9);

            borda_titulo = subwin(janela_livro,3,68,9,8);
            box(borda_titulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_titulo,"Titulo");

            titulo = subwin(borda_titulo,1,66,10,9);

            borda_subtitulo = subwin(janela_livro,3,68,12,8);
            box(borda_subtitulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_subtitulo,"SubTitulo");

            subtitulo = subwin(borda_subtitulo,1,66,13,9);

            botao_ok = subwin(janela_livro,1,4,16,30);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,16,40);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_livro);

            le_caract(serie,livro->serie,MAX_BUFFER_SERIE);
            le_caract(editora,livro->editora,MAX_BUFFER_EDITORA);
            le_caract(area,livro->area,MAX_BUFFER_AREA);
            le_caract(edicao,livro->edicao,MAX_BUFFER_EDICAO);
            le_caract(autor,livro->autor,MAX_BUFFER_AUTOR);
            le_caract(titulo,livro->titulo,MAX_BUFFER_TITULO);
            le_caract(subtitulo,livro->subtitulo,MAX_BUFFER_SUBTITULO);

            if(!select_buttao(janela_livro,botao_ok,botao_cancelar))
                result = inserir_fluxo((void**)livros,livro,MODO_BIBLIOTECA);
        }
        break;
        case MODO_ALTERAR_LIVRO:
        {
            janela_livro = newwin(17,70,2,7);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Alterar Livro");

            borda_serie = subwin(janela_livro,3,18,3,8);
            box(borda_serie,ACS_VLINE,ACS_HLINE);
            wprintw(borda_serie,"Serie");

            serie = subwin(borda_serie,1,16,4,9);

            borda_editora = subwin(janela_livro,3,17,3,26);
            box(borda_editora,ACS_VLINE,ACS_HLINE);
            wprintw(borda_editora,"Editora");

            editora = subwin(borda_editora,1,15,4,27);

            borda_area = subwin(janela_livro,3,19,3,43);
            box(borda_area,ACS_VLINE,ACS_HLINE);
            wprintw(borda_area,"Area");

            area = subwin(borda_area,1,14,4,44);

            borda_edicao = subwin(janela_livro,3,14,3,62);
            box(borda_edicao,ACS_VLINE,ACS_HLINE);
            wprintw(borda_edicao,"Edicao");

            edicao = subwin(borda_edicao,1,12,4,63);

            borda_autor = subwin(janela_livro,3,68,6,8);
            box(borda_autor,ACS_VLINE,ACS_HLINE);
            wprintw(borda_autor,"Autor");

            autor = subwin(borda_autor,1,66,7,9);

            borda_titulo = subwin(janela_livro,3,68,9,8);
            box(borda_titulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_titulo,"Titulo");

            titulo = subwin(borda_titulo,1,66,10,9);

            borda_subtitulo = subwin(janela_livro,3,68,12,8);
            box(borda_subtitulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_subtitulo,"SubTitulo");

            subtitulo = subwin(borda_subtitulo,1,66,13,9);

            botao_ok = subwin(janela_livro,1,4,16,30);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,16,40);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_livro);

            le_caract(serie,livro->serie,MAX_BUFFER_SERIE);
            le_caract(editora,livro->editora,MAX_BUFFER_EDITORA);
            le_caract(area,livro->area,MAX_BUFFER_AREA);
            le_caract(edicao,livro->edicao,MAX_BUFFER_EDICAO);
            le_caract(autor,livro->autor,MAX_BUFFER_AUTOR);
            le_caract(titulo,livro->titulo,MAX_BUFFER_TITULO);
            le_caract(subtitulo,livro->subtitulo,MAX_BUFFER_SUBTITULO);

            if(!select_buttao(janela_livro,botao_ok,botao_cancelar))
                result = alterar_fluxo((void**)livros,livro,MODO_BIBLIOTECA);
        }
        break;
        case MODO_LISTAR_LIVRO:
        {
            LIVRO *livr = *livros;
            int key = 0;
            MEVENT event;

            janela_livro = newwin(17,70,2,7);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Cadastro Livro");

            borda_serie = subwin(janela_livro,3,18,3,8);
            box(borda_serie,ACS_VLINE,ACS_HLINE);
            wprintw(borda_serie,"Serie");

            serie = subwin(borda_serie,1,16,4,9);

            borda_editora = subwin(janela_livro,3,17,3,26);
            box(borda_editora,ACS_VLINE,ACS_HLINE);
            wprintw(borda_editora,"Editora");

            editora = subwin(borda_editora,1,15,4,27);

            borda_area = subwin(janela_livro,3,19,3,43);
            box(borda_area,ACS_VLINE,ACS_HLINE);
            wprintw(borda_area,"Area");

            area = subwin(borda_area,1,14,4,44);

            borda_edicao = subwin(janela_livro,3,14,3,62);
            box(borda_edicao,ACS_VLINE,ACS_HLINE);
            wprintw(borda_edicao,"Edicao");

            edicao = subwin(borda_edicao,1,12,4,63);

            borda_autor = subwin(janela_livro,3,68,6,8);
            box(borda_autor,ACS_VLINE,ACS_HLINE);
            wprintw(borda_autor,"Autor");

            autor = subwin(borda_autor,1,66,7,9);

            borda_titulo = subwin(janela_livro,3,68,9,8);
            box(borda_titulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_titulo,"Titulo");

            titulo = subwin(borda_titulo,1,66,10,9);

            borda_subtitulo = subwin(janela_livro,3,68,12,8);
            box(borda_subtitulo,ACS_VLINE,ACS_HLINE);
            wprintw(borda_subtitulo,"SubTitulo");

            subtitulo = subwin(borda_subtitulo,1,66,13,9);

            botao_ok = subwin(janela_livro,1,4,16,40);
            wprintw(botao_ok,"[OK]");

            botao_ant = subwin(janela_livro,1,2,16,25);
            wprintw(botao_ant,"<");

            botao_prox = subwin(janela_livro,1,2,16,58);
            wprintw(botao_prox,">");

            keypad(janela_livro,TRUE);
            mousemask(ALL_MOUSE_EVENTS,NULL);

            limpar_tela();
            wrefresh(janela_livro);

            if(!livr)
            {
                result = ERRO_LISTA_VAZIA;
                break;
            }

            for (;;)
            {
                werase(serie);
                werase(editora);
                werase(area);
                werase(edicao);
                werase(autor);
                werase(titulo);
                werase(subtitulo);

                mvwprintw(serie,0,0,"%s",livr->serie);
                mvwprintw(editora,0,0,"%s",livr->editora);
                mvwprintw(area,0,0,"%s",livr->area);
                mvwprintw(edicao,0,0,"%s",livr->edicao);
                mvwprintw(autor,0,0,"%s",livr->autor);
                mvwprintw(titulo,0,0,"%s",livr->titulo);
                mvwprintw(subtitulo,0,0,"%s",livr->subtitulo);
                touchwin(janela_livro);

                key = wgetch(janela_livro);

                if (key == KEY_MOUSE)
                {
                    if (getmouse(&event) == OK)
                    {
                        if (event.bstate & BUTTON1_CLICKED)
                        {
                            if ((event.y == 16) && (event.x == 25))
                            {
                                if(livr->ante)
                                livr = livr->ante;
                            }
                            else if ((event.y == 16) && (event.x == 58))
                            {
                                if(livr->prox)
                                livr = livr->prox;
                            }
                            else if ((event.y == 16) && ((event.x > 40) && (event.x < 45)))
                            {
                                break;
                            }
                        }
                    }
                }

                if (key == KEY_LEFT)
                {
                    if(livr->ante)
                        livr = livr->ante;
                }
                else if (key == KEY_RIGHT)
                {
                    if(livr->prox)
                        livr = livr->prox;
                }
                else if (key == KEY_ENVIAR)
                    break;

            }

        }
        break;
        case MODO_EMPRESITMO_LIVRO:
        {
            time_t tempo = 0;
            struct tm *info = NULL;
            int dia = 0, mes = 0, ano = 0;

            time(&tempo);
            info = gmtime(&tempo);

            dia = info->tm_mday;
            mes = info->tm_mon + 1;
            ano = info->tm_year + 1900;

            janela_livro = newwin(9,30,2,20);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Emprestimo Livro");

            borda_serie = subwin(janela_livro,3,28,3,21);
            box(borda_serie,ACS_VLINE,ACS_HLINE);
            wprintw(borda_serie,"Serie");

            serie = subwin(borda_serie,1,26,4,22);

            borda_cpf = subwin(janela_livro,3,28,6,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,7,22);

            botao_ok = subwin(janela_livro,1,4,9,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,9,35);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_livro);

            le_caract(serie,historico->serie,MAX_BUFFER_SERIE);
            le_cpf(cpf,historico->cpf);

            des_criptografar(historico->cpf);

            sprintf(historico->data_emp,"%02d\\%02d\\%d",dia,mes,ano);
            calcular_data(historico->data_dev,dia,mes,ano);

            if (!select_buttao(janela_livro,botao_ok,botao_cancelar))
                if ((result = verificar_user(*usuario,historico->cpf)) == SUCESSO_USUARIO_CADASTRADO)
                {
                    if ((result = verificar_livro(*livros,historico->serie)) == SUCESSO_LIVRO_CADASTRADO)
                    {
                        if ((result = verificar_historico(*hist,(void*)historico)) == SUCESSO_EMPRESTIMO)
                            result = inserir_fluxo((void**)hist,historico,MODO_HISTORICO);
                    }
                }
        }
        break;
        case MODO_DEVOLUCAO_LIVRO:
        {
            janela_livro = newwin(9,30,2,20);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Devolucao Livro");

            borda_serie = subwin(janela_livro,3,28,3,21);
            box(borda_serie,ACS_VLINE,ACS_HLINE);
            wprintw(borda_serie,"Serie");

            serie = subwin(borda_serie,1,26,4,22);

            borda_cpf = subwin(janela_livro,3,28,6,21);
            box(borda_cpf,ACS_VLINE,ACS_HLINE);
            wprintw(borda_cpf,"CPF");

            cpf = subwin(borda_cpf,1,26,7,22);

            botao_ok = subwin(janela_livro,1,4,9,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,9,35);
            wprintw(botao_cancelar,"[CANCELAR]");


            limpar_tela();
            wrefresh(janela_livro);

            le_caract(serie,historico->serie,MAX_BUFFER_SERIE);
            le_cpf(cpf,historico->cpf);

            des_criptografar(historico->cpf);

            if(!select_buttao(janela_livro,botao_ok,botao_cancelar))
                result = excluir_fluxo((void**)hist,(void*)historico,MODO_HISTORICO);
        }
        break;
        case MODO_IMPORTAR_LIVRO:
        {
            WINDOW *borda_nome = NULL, *edit_nome = NULL;
            char *nome = NULL;

            nome = (char*) malloc(MAX_BUFFER_NOME * sizeof(char));
            if (nome == NULL)
                return ERRO_ALOCAR_MEMORIA;

            janela_livro = newwin(7,30,2,20);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Importar Livro");

            borda_nome = subwin(janela_livro,3,28,3,21);
            box(borda_nome,ACS_VLINE,ACS_HLINE);
            wprintw(borda_nome,"Nome do arquivo");

            edit_nome = subwin(borda_nome,1,26,4,22);

            botao_ok = subwin(janela_livro,1,4,6,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,6,35);
            wprintw(botao_cancelar,"[CANCELAR]");

            limpar_tela();
            wrefresh(janela_livro);

            le_caract(edit_nome,nome,MAX_BUFFER_NOME);

            if(!select_buttao(janela_livro,botao_ok,botao_cancelar))
                result = import_livros(livros,nome);

            free(nome);
        }
        break;
        case MODO_EXCLUIR_LIVRO:
        {
            janela_livro = newwin(6,30,2,20);
            wbkgd(janela_livro,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            box(janela_livro,ACS_VLINE,ACS_HLINE);
            wprintw(janela_livro,"Excluir Livro");

            borda_serie = subwin(janela_livro,3,28,3,21);
            box(borda_serie,ACS_VLINE,ACS_HLINE);
            wprintw(borda_serie,"Serie");

            serie = subwin(borda_serie,1,26,4,22);

            botao_ok = subwin(janela_livro,1,4,6,25);
            wprintw(botao_ok,"[OK]");

            botao_cancelar = subwin(janela_livro,1,10,6,35);
            wprintw(botao_cancelar,"[CANCELAR]");


            limpar_tela();
            wrefresh(janela_livro);

            le_caract(serie,livro->serie,MAX_BUFFER_SERIE);

            if(!select_buttao(janela_livro,botao_ok,botao_cancelar))
                result = excluir_fluxo((void**)livros,(void*)livro,MODO_BIBLIOTECA);
        }
        break;


    }

    free(livro);
    free(historico);

    return result;
}

static int tela_principal(WINDOW *principal)
/*
    Está função cria uma tela 8X70
    para exibição do logotipo.
*/
{

    principal = subwin(stdscr,8,70,8,9);
    wbkgd(principal,COLOR_PAIR(PADRAO_PT_BR)|A_BOLD|A_ITALIC);
    wprintw(principal,"_-_-       ,,      /\\\\,/\\\\,                                    \n");
    wprintw(principal," //     '  ||     /| || ||    _           _     _              \n");
    wprintw(principal," ||     \\\\ ||/|,  || || ||   < \\, \\\\/\\\\  < \\,  / \\\\  _-_  ,._-_ \n");
    wprintw(principal,"~||     || || ||  || || ||   /-|| || ||  /-|| || || || \\\\  ||   \n");
    wprintw(principal," ||     || || |' ~|| || ||  (( || || || (( || || || ||/    ||   \n");
    wprintw(principal," (-_-_\\ \\\\ \\\\/    |, \\\\,\\\\,  \\/\\\\ \\\\ \\\\  \\/\\\\ \\\\_-| \\\\,/   \\\\,  \n");
    wprintw(principal,"                 _-                              ||             \n");
    wprintw(principal,"                                             \\\\__//           \n");
    wrefresh(principal);

    return SUCESSO;

}

static int tela_mensagem(int *resultado)
/*
    Está função gerenciar as mensagens
    que aparecem para o usuário.
*/
{
    if(!(*resultado))
        return SUCESSO;

    WINDOW *janela_mensagem = NULL, *msg = NULL;

    janela_mensagem = newwin(3,40,10,15);
    msg = subwin(janela_mensagem,1,38,11,16);
    box(janela_mensagem,ACS_VLINE,ACS_HLINE);
    wbkgd(janela_mensagem,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
    if (*resultado > 30)
        wprintw(janela_mensagem,"SUCESSO");
    else
        wprintw(janela_mensagem,"ERRO");

    switch(*resultado)
        {
            case SUCESSO_LIVRO_CADASTRADO:
            {
                wprintw(msg,"Livro cadastrado com sucesso!");
            }
            break;
            case SUCESSO_USUARIO_CADASTRADO:
            {
                wprintw(msg,"Usuario cadastrado com sucesso!");
            }
            break;
            case SUCESSO_EMPRESTIMO:
            {
                wprintw(msg,"Emprestimo realizado com sucesso!");
            }
            break;
            case SUCESSO_ALTERAR_LIVRO:
            {
                wprintw(msg,"Livro alterado com sucesso!");
            }
            break;
            case SUCESSO_ALTERAR_USUARIO:
            {
                wprintw(msg,"Usuario alterado com sucesso!");
            }
            break;
            case SUCESSO_ALTERAR_HISTORICO:
            {
                wprintw(msg,"Emprestimo alterado com sucesso!");
            }
            break;
            case SUCESSO_EXCLUIR_LIVRO:
            {
                wprintw(msg,"Livro excluido com sucesso!");
            }
            break;
            case SUCESSO_EXCLUIR_USUARIO:
            {
                wprintw(msg,"Usuario excluido com sucesso!");
            }
            break;
            case SUCESSO_EXCLUIR_HISTORICO:
            {
                wprintw(msg,"Devolucao realizada com sucesso!");
            }
            break;
            case SUCESSO_IMPORTAR_LIVROS:
            {
                wprintw(msg,"Livros importados com sucesso!");
            }
            break;
            case SUCESSO_LIVRO_DISPONIVEL:
            {
                wprintw(msg,"Livro esta disponivel!");
            }
            break;
            case ERRO_ALOCAR_MEMORIA:
            {
                wprintw(msg,"Nao foi possivel alocar memoria!");
            }
            break;
            case ERRO_LISTA_VAZIA:
            {
                wprintw(msg,"Nao foi possivel modificar os dados!");
            }
            break;
            case ERRO_ARQUIVO_NAO_ENCONTRADO:
            {
                wprintw(msg,"O arquivo nao foi encontrado!");
            }
            break;
            case ERRO_DEVOLCAO_DATA_ERRADA:
            {
                wprintw(msg,"Livro devolvido na data errada!");
            }
            break;
            case ERRO_ARQUIVO_VAZIO:
            {
                wprintw(msg,"O arquivo para importacao esta vazio!");
            }
            break;
            case ERRO_CRIAR_ARQUIVO:
            {
                wprintw(msg,"Arquivos nao foram criados!");
            }
            break;
            case ERRO_CPF_INVALIDO:
            {
                wprintw(msg,"O cpf do usuario e invalido!");
            }
            break;
            case ERRO_LIVRO_CADASTRADO:
            {
                wprintw(msg,"O livro ja esta cadastrado!");
            }
            break;
            case ERRO_LIVRO_NAO_CADASTRADO:
            {
                wprintw(msg,"O livro nao esta cadastrado!");
            }
            break;
            case ERRO_MAX_EMPRESTIMO:
            {
                wprintw(msg,"Usuario ja esta com 2 emprestimos!");
            }
            break;
            case ERRO_USUARIO_CADASTRADO:
            {
                wprintw(msg,"O usuario ja esta cadastrado!");
            }
            break;
            case ERRO_USUARIO_NAO_CADASTRADO:
            {
                wprintw(msg,"O usuario nao esta cadastrado!");
            }
            break;
            case ERRO_LIVRO_NAO_DISPONIVEL:
            {
                wprintw(msg,"O Livro nao esta disponivel!");
            }
            break;
            default:
                wprintw(msg,"Erro nao indentificado!");
        }

    limpar_tela();
    wrefresh(janela_mensagem);
    beep();

    sleep(2);

    *resultado = 0;

    return SUCESSO;
}

static int tela_sobre(void)
/*
    está função cria um tela 20X30
    para exibição de informações
    sobre o aplicativo.
*/
{
    WINDOW *borda = NULL, *sobre = NULL;

    borda = newwin(20,30,2,20);
    wbkgd(borda,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
    box(borda,ACS_VLINE,ACS_HLINE);

    keypad(borda,TRUE);
    mousemask(ALL_MOUSE_EVENTS,NULL);

    sobre = subwin(borda,18,28,3,21);

    mvwprintw(sobre,0,9,"LibManager");
    mvwprintw(sobre,2,9,"*  **   **");
    mvwprintw(sobre,3,9,"*  * * * *");
    mvwprintw(sobre,4,9,"*  *  *  *");
    mvwprintw(sobre,5,9,"*");
    mvwprintw(sobre,6,9,"******");
    mvwprintw(sobre,8,11,"1.0.0");
    mvwprintw(sobre,10,2,"Gerencie sua biblioteca facilmente com este programa, que torna uma tarefa complicada em simples.");
    mvwprintw(sobre,16,0,"ATENCAO: Este programa oferece garantia.");

    limpar_tela();
    wrefresh(borda);
    wgetch(borda);

    return SUCESSO;
}


static int le_cpf(WINDOW *edit, char *dado)
/*
    Está função lê do teclado os caracteres
    digitados e aplica uma mascara (###.###.###.##).
*/
{
    register int8_t cont = 0;
    register int digit = 0;

    keypad(edit,TRUE);
    noecho();
    curs_set(TRUE);

    for (cont = 0; cont < MAX_BUFFER_CPF;)
    {

        digit = wgetch(edit);

        if (((digit == KEY_ENVIAR) && (cont == MAX_BUFFER_CPF-1)) || (cont == MAX_BUFFER_CPF-1))
        {
            dado[cont] = '\0';
            break;
        }
        else if ((digit == KEY_BACKSPACE) && (cont > 0))
        {
            waddch(edit,0x08);
            waddch(edit,' ');
            waddch(edit,0x08);
            cont--;
            continue;
        }
        else if (isdigit(digit))
        {
            if ((cont == 3) || (cont == 7) || (cont == 11))
            {
                waddch(edit,'.');
                dado[cont] = '.';
                cont++;
            }
            dado[cont] = digit;
            waddch(edit,digit);
            cont++;
            continue;
        }

    }

    noecho();
    curs_set(FALSE);

    return SUCESSO;
}

static int le_telefone(WINDOW *edit, char *dado)
/*
    Está função lê do teclado os caracteres
    digitados e aplica uma mascara ((##)####-#####).
*/
{
    register int8_t cont = 0;
    register int digit = 0;

    keypad(edit,TRUE);
    noecho();
    curs_set(TRUE);

    for (cont = 0; cont < MAX_BUFFER_TELEFONE;)
    {

        digit = wgetch(edit);


        if (((digit == KEY_ENVIAR) && (cont == MAX_BUFFER_TELEFONE-2)) || (cont == MAX_BUFFER_TELEFONE-1))
        {
            dado[cont] = '\0';
            break;
        }
        else if ((digit == KEY_BACKSPACE) && (cont > 0))
        {
            waddch(edit,0x08);
            waddch(edit,' ');
            waddch(edit,0x08);
            cont--;
            continue;
        }
        else if (isdigit(digit))
        {
            switch (cont)
            {
                case 0:
                {
                    waddch(edit,'(');
                    dado[cont] = '(';
                    cont++;
                }
                break;
                case 3:
                {
                    waddch(edit,')');
                    dado[cont] = ')';
                    cont++;
                }
                break;
                case 8:
                {
                    waddch(edit,'-');
                    dado[cont] = '-';
                    cont++;
                }
                break;

            }
            dado[cont] = digit;
            waddch(edit,digit);
            cont++;
            continue;
        }

    }

    noecho();
    curs_set(FALSE);

    return SUCESSO;
}

static int le_caract(WINDOW *edit, char *buffer, const int8_t max_buffer)
/*
    Está função lê do teclado os caracteres
    digitados.
*/
{
    register int8_t cont = 0;
    register int digit = 0;

    keypad(edit,TRUE);
    noecho();
    curs_set(TRUE);

    for (cont = 0; cont < max_buffer;)
    {
        digit = wgetch(edit);

        if ((digit == KEY_ENVIAR) || (cont == max_buffer))
        {
            buffer[cont] = '\0';
            break;
        }
        else if ((digit == KEY_BACKSPACE) && (cont > 0))
        {
            waddch(edit,0x08);
            waddch(edit,' ');
            waddch(edit,0x08);
            cont--;
            continue;
        }
        else if (isalnum(digit) || isspace(digit) || ispunct(digit))
        {
            buffer[cont] = digit;
            waddch(edit,digit);
            cont++;
            continue;
        }
    }

    noecho();
    curs_set(FALSE);

    return SUCESSO;
}

static int select_buttao(WINDOW *janela, WINDOW *butao1, WINDOW *botao2)
/*
    Está função faz a mudança de cor do botão
    conforme a escolha do usuário.
*/
{
    register int key = 0, selecionado = 0;

    keypad(janela,TRUE);
    noecho();
    curs_set(FALSE);

    wbkgd(butao1,COLOR_PAIR(PADRAO_PT_BR)|A_BOLD);
    wnoutrefresh(butao1);
    doupdate();

    for(;;)
    {
        key = wgetch(janela);

        if (key == KEY_LEFT)
        {
            wbkgd(butao1,COLOR_PAIR(PADRAO_PT_BR)|A_BOLD);
            wbkgd(botao2,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            wnoutrefresh(butao1);
            wnoutrefresh(botao2);
            doupdate();

            selecionado = (selecionado + 1) % 2;

        }
        else if (key == KEY_RIGHT)
        {
            wbkgd(botao2,COLOR_PAIR(PADRAO_PT_BR)|A_BOLD);
            wbkgd(butao1,COLOR_PAIR(PADRAO_BR_PT)|A_BOLD);
            wnoutrefresh(butao1);
            wnoutrefresh(botao2);
            doupdate();

            selecionado = (selecionado + 2 - 1) % 2;
        }
        else if ((key == KEY_ENVIAR) || (key == KEY_ESC))
            break;
    }

    return selecionado;
}

static int limpar_tela(void)
/*
    Está função remove as telas que
    estiverem sobrepondo a tela principal.
*/
{
    touchwin(stdscr);
    refresh();

    return SUCESSO;
}

static int verificar_user(USUARIO *user, char *dado)
/*
    Está função verificar se o usuário
    possui um cadastro no sistema.
*/
{
    while (user)
    {
        if (!strcmp(user->cpf,dado))
            return SUCESSO_USUARIO_CADASTRADO;
        user = user->prox;
    }

    return ERRO_USUARIO_NAO_CADASTRADO;
}

static int verificar_livro(LIVRO *livro, char *dado)
/*
    Está função verificar se o livro
    possui um cadastro no sistema.
*/
{
    while (livro)
    {
        if (!strcmp(livro->serie,dado))
            return SUCESSO_LIVRO_CADASTRADO;
        livro = livro->prox;
    }

    return ERRO_LIVRO_NAO_CADASTRADO;
}

static int consultar_livro(LIVRO *livro, HISTORICO *historico, void *dado)
/*
    Está função verificar se o livro
    se encontra disponível para empréstimo.
*/
{
    while (livro)
    {
        if (!strcmp(livro->titulo,((LIVRO*)dado)->titulo) || !strcmp(livro->subtitulo,((LIVRO*)dado)->subtitulo))
        {
            while(historico)
            {
                if (!strcmp(historico->serie,livro->serie))
                    return ERRO_LIVRO_NAO_DISPONIVEL;
                historico = historico->next;
            }

            return SUCESSO_LIVRO_DISPONIVEL;
        }

        livro = livro->prox;
    }

    return ERRO_LIVRO_NAO_CADASTRADO;
}

static int verificar_historico(HISTORICO *hist, void *dado)
/*
    Está função verificar a quantidade
    de livros emprestados para um determinado
    usuário e se o livro se encontra disponível.
*/
{
    int8_t cont = 0;

    while (hist != NULL)
    {
        if (!strcmp(hist->cpf,((HISTORICO*)dado)->cpf))
            cont++;
        if (!strcmp(hist->serie,((HISTORICO*)dado)->serie))
            return ERRO_LIVRO_NAO_DISPONIVEL;
        hist = hist->next;
    }

    if (cont == 2)
        return ERRO_MAX_EMPRESTIMO;

    return SUCESSO_EMPRESTIMO;
}

static int validar_cpf(char *dado)
/*
    Está função verificar se o CPF
    digitado pelo usuário e válido.
*/
{
    int resto_part1 = 0, resto_part2 = 0, digit_1 = 0, digit_2 = 0;
    char cpf[10][2];
    register int8_t cont = 0, ctl = 0;

    if (!strcmp(dado,"111.111.111.11") ||
        !strcmp(dado,"222.222.222.22") ||
        !strcmp(dado,"333.333.333.33") ||
        !strcmp(dado,"444.444.444.44") ||
        !strcmp(dado,"555.555.555.55") ||
        !strcmp(dado,"666.666.666.66") ||
        !strcmp(dado,"777.777.777.77") ||
        !strcmp(dado,"888.888.888.88") ||
        !strcmp(dado,"999.999.999.99"))
            return ERRO;

    for (cont = 0; cont < 15; cont++)
    {
        if (isdigit(dado[cont]))
        {
            cpf[ctl][0] = dado[cont];
            cpf[ctl][1] = '\0' ;
            ctl++;
        }
    }

    digit_1 = atoi(cpf[9]);
    digit_2 = atoi(cpf[10]);

    for (cont = 10; cont > 1; cont--)
    {
        resto_part1 += atoi(cpf[10-cont]) * cont;
    }

    resto_part1 = (resto_part1 * 10) % 11;

    if(resto_part1 == 10)
        resto_part1 = 0;

    for (cont = 11; cont > 1; cont--)
    {
        resto_part2 += atoi(cpf[11-cont]) * cont;
    }

    resto_part2 = (resto_part2 * 10) % 11;

    if(resto_part2 == 10)
        resto_part2 = 0;

    if ((resto_part1 == digit_1) && (resto_part2 == digit_2))
        return SUCESSO;

    return ERRO;
}

static int calcular_data(char *data_dev, int dia_emp, int mes_emp, int ano_emp)
/*
    Está função calcula a data para
    o livro ser devolvido.
*/
{
    #define DIAS_EMPRESTIMO  7
    #define BISSEXTO(x) ((((x) % 4 == 0) && ((x) % 100 != 0)) || (((x) % 100 == 0) && ((x) % 400 == 0)))
    int dia_dev = 0, mes_dev = 0, ano_dev = 0, dias = 0;

    switch(mes_emp)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            dias = 31;
        break;
        case 2:
            dias = 28 + BISSEXTO(ano_emp);
        break;
        case 4:
        case 6:
        case 9:
        case 11:
            dias = 30;
        break;

    }

    if((DIAS_EMPRESTIMO+dia_emp) > dias)
    {
        if (mes_emp == 12)
        {
            mes_dev = 01;
            ano_dev = ++ano_emp;
        }
        else
            mes_dev = ++mes_emp;
        dia_dev = (DIAS_EMPRESTIMO+dia_emp) - dias;
        ano_dev = ano_emp;
    }
    else if ((DIAS_EMPRESTIMO+dia_emp) < dias)
    {
        mes_dev = mes_emp;
        dia_dev = (DIAS_EMPRESTIMO+dia_emp);
        ano_dev = ano_emp;
    }

    sprintf(data_dev,"%02d\\%02d\\%d",dia_dev,mes_dev,ano_dev);

    return 0;
}

static int des_criptografar(char *dado)
/*
    Está função cifra os caracteres
    para dificultar sua leitura.
*/
{
    #define CHAVE_S1 ((((81 ^ 5) + 100) - 100) % 256)
    #define CHAVE_S2 ((((23 ^ 4) * 5) - 100) % 256)

    int8_t tamh = strlen(dado);
    int8_t cont = 0;

    for(cont = 0; cont < tamh; cont++)
        dado[cont] = ~((dado[cont] + ((CHAVE_S2 ^ CHAVE_S1) * 100) % 256) ^ 2) % 256;

    return SUCESSO;
}

static int import_livros(LIVRO **livros, const char *lcarquivo)
/*
    está função lê um aquivo padrão editado
    pelo usuário com os livros que o mesmo
    deseja importar para o estoque da biblioteca.
*/
{
    auto int le_dados(const int);

    FILE *arquivo = NULL;
    char buffer = 0;
    int erro;
    LIVRO *livro = NULL;

    livro = (LIVRO*) calloc(1,sizeof(LIVRO));
    if(!livro)
        return ERRO_ALOCAR_MEMORIA;

    if (!(arquivo = fopen(lcarquivo,"r")))
        return ERRO_ARQUIVO_NAO_ENCONTRADO;

    fseek(arquivo,0,SEEK_END);
    if (!ftell(arquivo))
        return ERRO_ARQUIVO_VAZIO;

    rewind(arquivo);

    while ((buffer = getc(arquivo)) != EOF)
    {
        if (buffer == '#')
        {
            if ((strlen(livro->serie) > 2) && (strlen(livro->autor) > 2) && (strlen(livro->titulo) > 2) && (strlen(livro->area) > 2) && (strlen(livro->editora) > 0))
                erro = inserir_fluxo((void**)livros,livro,MODO_BIBLIOTECA);
        }

        if (buffer == '-')
        {

            switch (getc(arquivo))
            {
                case 'i':
                    le_dados(0);
                    break;
                case 'a':
                    le_dados(1);
                    break;
                case 't':
                    le_dados(2);
                    break;
                case 's':
                    le_dados(3);
                    break;
                case 'e':
                    le_dados(4);
                    break;
                case 'A':
                    le_dados(5);
                    break;
                case 'E':
                    le_dados(6);
                    break;
            }
        }
    }

    if (arquivo != NULL)
        fclose(arquivo);

    free(livro);
    if (erro == SUCESSO_LIVRO_CADASTRADO)
        return SUCESSO_IMPORTAR_LIVROS;
    else
        return erro;

    int le_dados(const int op)
    {
        char ctl;
        register int16_t cont;

        if ((ctl = getc(arquivo)) == '{')
            switch (op)
            {
                case 0:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_SERIE); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->serie[cont] = ctl;
                    }
                    livro->serie[cont] = '\0';
                break;
                case 1:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_AUTOR); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->autor[cont] = ctl;
                    }
                    livro->autor[cont] = '\0';
                break;
                case 2:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_TITULO); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->titulo[cont] = ctl;
                    }
                    livro->titulo[cont] = '\0';
                break;
                case 3:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_SUBTITULO); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->subtitulo[cont] = ctl;
                    }
                    livro->subtitulo[cont] = '\0';
                break;
                case 4:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_EDITORA); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->editora[cont] = ctl;
                    }
                    livro->editora[cont] = '\0';
                break;
                case 5:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_AREA); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->area[cont] = ctl;
                    }
                    livro->area[cont] = '\0';
                break;
                case 6:
                    for (cont = 0; ((ctl = fgetc(arquivo)) != '}') && (cont < MAX_BUFFER_EDICAO); cont++)
                    {
                        if (ctl == '\n')
                        {
                            cont--;
                            continue;
                        }
                        livro->edicao[cont] = ctl;
                    }
                    livro->edicao[cont] = '\0';
                break;
                }

        return SUCESSO;
    }
}
