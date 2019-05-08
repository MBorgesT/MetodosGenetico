#ifndef PMM1_H_INCLUDED
#define PMM1_H_INCLUDED

#define ALPHA 7

//TODO: mudar a estrutura professor para poder dar memset somente em certas variaveis
typedef struct professor{
    bool horario[6][5];
    bool tercTunro[2][5];
    int qtdAulasPerdidas;
    int qtdAulasNoiteAntesReuniao;
    int qtdReuTercTurno;
}Professor;

typedef struct reuniao{
    int data[2]; //linha e coluna da matriz;
    int horario; //0 até 3
}Reuniao;

typedef struct calendario{
    bool matriz[18][7];
}Calendario;

typedef struct solucao{
    Reuniao reunioes[9];
    int fo;
}Solucao;


int qtdCalLin, qtdCalCol;
int qtdProf, qtdHorLin, qtdHorCol;

int qtdAulasPerdidasTotal;
int qtdAulasNoiteAntesReuniTotal;
int qtdReuTerTurnoTotal;

Professor listaProfessores[16];
Calendario calendario;

int pesos[7] = {10,5,1,1,100,500,500};

void lerDados(char *arq);
void testarDados(char *arq);
void zerarVariaveis();
void heuCon(Solucao &s);
void calRestricoes(Solucao &s);
float calRestricao2();
float calRestricao5();
float calRestricao6(Solucao &s);
float calRestricao7(Solucao &s);
int calQtdReuMes(Solucao &s, int op);
void calFO(Solucao &s);
void escreverResultado(Solucao &s);



#endif