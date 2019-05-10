#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "main.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
//#define ABS(x)  ( (x<0) ? -x : x )


//TODO:

int main() {
    srand(time(NULL));

    lerDados("entradas.txt");
    testarDados("teste.txt");

    gerarPop();
    ordenacao();

    int i = 0, aux, flag;
    while(i < 500000) {
        if (i % 1000 == 0) {
            printf("%i\t%i\n", i, pop[0].fo);
        }

        aux = pop[0].fo;
        flag = 1;
        for (int j = 1; j < TAM_POP + QTD_CRUZ; j++){
            if (pop[j].fo != aux){
                flag = 0;
                break;
            }
        }
        if (flag == 1){
            epidemia();
        }

        crossover();
        i++;
    }
    calFO(pop[0]);
    escreverResultado(pop[0]);

    return 0;
}

void lerDados(char *arq) {
    int aux;

    FILE *f = fopen(arq, "r");
    fscanf(f, "%i %i", &qtdCalLin, &qtdCalCol);
    fscanf(f, "%i %i %i", &qtdProf, &qtdHorLin, &qtdHorCol);

    for (int i = 0; i < qtdCalLin; i++) {
        for (int j = 0; j < qtdCalCol; j++) {
            fscanf(f, "%d", &aux);
            calendario.matriz[i][j] = aux;
        }
    }

    for (int i = 0; i < qtdProf; i++) {
        for (int j = 0; j < qtdHorLin; j++) {
            for (int k = 0; k < qtdHorCol; k++) {
                fscanf(f, "%d", &aux);
                listaProfessores[i].horario[j][k] = aux;
            }
        }
    }

    for (int i = 0; i < qtdProf; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 5; k++) {
                fscanf(f, "%d", &aux);
                listaProfessores[i].tercTunro[j][k] = aux;
            }
        }
    }

    fclose(f);
}

void testarDados(char *arq) {
    FILE *f;
    if (arq == "")
        f = stdout;
    else
        f = fopen(arq, "w");

    fprintf(f, "%i %i\n", qtdCalLin, qtdCalCol);
    fprintf(f, "%i %i %i\n\n", qtdProf, qtdHorLin, qtdHorCol);

    for (int i = 0; i < qtdCalLin; i++) {
        for (int j = 0; j < qtdCalCol; j++) {
            fprintf(f, "%i ", calendario.matriz[i][j]);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");

    for (int i = 0; i < qtdProf; i++) {
        for (int j = 0; j < qtdHorLin; j++) {
            for (int k = 0; k < qtdHorCol; k++) {
                fprintf(f, "%i ", listaProfessores[i].horario[j][k]);
            }
            fprintf(f, "\n");
        }
        fprintf(f, "\n");
    }

    for (int i = 0; i < qtdProf; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 5; k++) {
                fprintf(f, "%i ", listaProfessores[i].tercTunro[j][k]);
            }
            fprintf(f, "\n");
        }
        fprintf(f, "\n");
    }


    if (arq == "")
        fclose(f);
}

void zerarVariaveis(){
    for (int i = 0; i < qtdProf; i++){
        listaProfessores[i].qtdAulasPerdidas = 0;
        listaProfessores[i].qtdReuTercTurno = 0;
        listaProfessores[i].qtdAulasNoiteAntesReuniao = 0;
    }
    qtdAulasPerdidasTotal = 0;
    qtdReuTerTurnoTotal = 0;
    qtdAulasNoiteAntesReuniTotal = 0;
}

void heuCon(Solucao &s) {
    int dia = 0, x, y, horario;

    for (int i = 0; i < 9; i++) { //reunioes
        x = (int) (dia / 7);
        y = dia % 7;
        if (calendario.matriz[x][y]) {

            s.reunioes[i].data[0] = x;
            s.reunioes[i].data[1] = y;

            horario = rand() % 4;
            s.reunioes[i].horario = horario;

            //printf("Reu: %i\tDia: %i\tX: %i, Y: %i\tHorario: %i\n", i, dia, x, y, reunioes[i].horario);

            dia += (rand() % ALPHA) + 7;
        } else {
            dia++;
            i--;
        }
    }
}

void heuAle(Solucao &s){
    int vet[9];
    for (int i = 0; i < 9; i++){
        vet[i] = rand() % 126;
        if ((vet[i] % 7 == 0) || (vet[i] % 7 == 6))
            i--;
    }

    int escolhido, j;

    for (int i = 1; i < 9; i++) {
        escolhido = vet[i];
        j = i - 1;

        while ((j >= 0) && (vet[j] > escolhido)) {
            vet[j + 1] = vet[j];
            j--;
        }

        vet[j + 1] = escolhido;
    }

    for (int i = 0; i < 9; i++){
        s.reunioes[i].data[0] = (int)(vet[i]/7);
        s.reunioes[i].data[1] = vet[i] % 7;
        s.reunioes[i].horario = rand() % 4;
    }
}

void calRestricoes(Solucao &s) {
    for (int i = 0; i < 9; i++) { //reunioes
        for (int j = 0; j < qtdProf; j++) { //professores
            //Aulas perdidas
            if (listaProfessores[j].horario[s.reunioes[i].horario][s.reunioes[i].data[1] - 1]) { //-1 porque a matriz de calendário conta sábado e domingo, sendo que a de horario de professores não
                listaProfessores[j].qtdAulasPerdidas++;
                qtdAulasPerdidasTotal++;
            }

            //Aulas de noite antes da reuniao
            if (s.reunioes[i].data[1] != 1) {
                if ((listaProfessores[j].horario[4][s.reunioes[i].data[1] - 2] ||
                     listaProfessores[j].horario[5][s.reunioes[i].data[1] - 2]) && (s.reunioes[i].horario == 0 ||
                                                                                    s.reunioes[i].horario ==
                                                                                    1)) { //-2 porque a matriz de calendário conta sábado e domingo, sendo que a de horario de professores não
                    listaProfessores[j].qtdAulasNoiteAntesReuniao++;
                    qtdAulasNoiteAntesReuniTotal++;
                }
            }

            //Terceiro turno
            if (listaProfessores[j].tercTunro[(int)(s.reunioes[i].horario/2)][s.reunioes[i].data[1] - 1]) {
                listaProfessores[j].qtdReuTercTurno++;
                qtdReuTerTurnoTotal++;
            }
        }
    }
}

float calRestricao2(){
    float MAP = qtdAulasPerdidasTotal / qtdProf;
    float resultado = 0;
    for (int i = 0; i < qtdProf; i++){
        resultado += abs(listaProfessores[i].qtdAulasPerdidas - MAP);
    }
    return resultado;
}

float calRestricao5(){
    float resultado = 0;
    for (int i = 0; i < qtdProf; i++){
        resultado += MAX(0, listaProfessores[i].qtdAulasPerdidas - 3);
    }
    return resultado;
}

float calRestricao6(Solucao &s){
    float resultado = 0;
    for (int i = 0; i < 4; i++){
        resultado += abs(calQtdReuMes(s, i) - 2);
    }
    resultado += abs(calQtdReuMes(s, 4) - 1);
    return resultado;
}

float calRestricao7(Solucao &s){
    float resultado = 0;
    for (int i = 1; i < 9; i++){
        int dia1 = (s.reunioes[i-1].data[0] * 7) + s.reunioes[i-1].data[1] + 1;
        int dia2 = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
        resultado += MAX(0, 10 - dia2 + dia1);
    }
    return resultado;
}

int calQtdReuMes(Solucao &s, int op){
    int qtd = 0;
    switch(op){
        case 0:
            for (int i = 0; i < 9; i++){
                int dia = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
                if(dia <= 22){
                    qtd++;
                }
            }
            break;
        case 1:
            for (int i = 0; i < 9; i++){
                int dia = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
                if((dia > 22 ) && (dia <= 52)){
                    qtd++;
                }
            }
            break;
        case 2:
            for (int i = 0; i < 9; i++){
                int dia = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
                if((dia > 52 ) && (dia <= 83)){
                    qtd++;
                }
            }
            break;
        case 3:
            for (int i = 0; i < 9; i++){
                int dia = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
                if((dia > 83) && (dia <= 113)){
                    qtd++;
                }
            }
            break;
        case 4:
            for (int i = 0; i < 9; i++){
                int dia = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
                if((dia > 113 ) && (dia <= 126)){
                    qtd++;
                }
            }
            break;
        default:
            printf("Parametro incorreto na funcao calQtdReuMes");
            break;
    }
    return qtd;
}

void calFO(Solucao &s){
    zerarVariaveis();
    calRestricoes(s);

    s.fo = (pesos[0] * qtdAulasPerdidasTotal) +
            (pesos[1] * calRestricao2()) +
            (pesos[2] * qtdAulasNoiteAntesReuniTotal) +
            (pesos[3] * qtdReuTerTurnoTotal) +
            (pesos[4] * calRestricao5()) +
            (pesos[5] * calRestricao6(s)) +
            (pesos[6] * calRestricao7(s));

    for (int i = 0; i < 9; i++){
        if ((s.reunioes[i].data[1] == 0) || (s.reunioes[i].data[1] == 6))
            s.fo += 10000;
    }

}

void escreverResultado(Solucao &s){
    printf("FO: %i\n\n", s.fo);
    printf("Reunioes: \n");
    for (int i = 0; i < 9; i++){
        int dia = (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1;
        if(dia <= 22){
            printf("%i de marco\t", dia + 10);
        }else if((dia > 22 ) && (dia <= 52)){

            printf("%i de abril\t", dia - 22);

        }else if((dia > 52 ) && (dia <= 83)){
            printf("%i de maio\t", dia - 52);

        }else if((dia > 83 ) && (dia <= 113)){
            printf("%i de junho\t", dia - 83);

        }else if((dia > 113 ) && (dia <= 126)){
            printf("%i de julho\t", dia - 113);

        }

        switch (s.reunioes[i].data[1]){
            case 1:
                printf("Segunda feira\t");
                break;
            case 2:
                printf("Terca feira\t\t");
                break;
            case 3:
                printf("Quarta feira\t");
                break;
            case 4:
                printf("Quinta feira\t");
                break;
            case 5:
                printf("Sexta feira\t\t");
                break;
        }

        printf("Horario: %i\n", s.reunioes[i].horario);
    }
    printf("\n");

    for (int i = 0; i < qtdProf; i++){
        printf("Prof %i\tAulas Perdidas %i\tReu Ter Turno: %i\tReuni depois de aula a noite: %i\n", i, listaProfessores[i].qtdAulasPerdidas, listaProfessores[i].qtdReuTercTurno, listaProfessores[i].qtdAulasNoiteAntesReuniao);
    }

    printf("\nAulas perdidas total: %i\tReuni Ter Turno total: %i\tReuni depois de aula a noite total: %i\n", qtdAulasPerdidasTotal, qtdReuTerTurnoTotal, qtdAulasNoiteAntesReuniTotal);

    printf("\n---------------------------\n");
}

void escreverResultadoSimp(Solucao &s){
    printf("FO: %i\n\n", s.fo);
    for (int i = 0; i < 9; i++){
        printf("Dia: %i\tHorario: %i\n", (s.reunioes[i].data[0] * 7) + s.reunioes[i].data[1] + 1, s.reunioes[i].horario);
    }
    printf("\n-----------------------------------\n");
}

void gerarPop(){
    for (int i = 0; i < TAM_POP + QTD_CRUZ; i++){
        heuAle(pop[i]);
        calFO(pop[i]);
    }
}

void mutacao(Solucao &s){
    if (rand() % 2){
        int res = rand() % 9;
        s.reunioes[res].horario = (s.reunioes[res].horario++) % BETA;
    }else{
        int res = rand() % 9;
        int dia = ((s.reunioes[res].data[0] * 7) + s.reunioes[res].data[1] + 1) + (rand() % BETA) - 2;
        s.reunioes[res].data[0] = (int) (dia / 7);
        s.reunioes[res].data[1] = dia % 7;
    }
}

void cruzamento(Solucao s1, Solucao s2, Solucao &f1, Solucao &f2){
    for (int i = 0; i < 9; i++){
        memcpy(&f1.reunioes[i], &(i % 2 == 0 ? s1 : s2).reunioes[i], sizeof(f1.reunioes[i]));
    }
    if (rand() % 100 < MUTACAO)
        mutacao(f1);

    calFO(f1);

    for (int i = 0; i < 9; i++){
        memcpy(&f2.reunioes[i], &(i % 2 == 1 ? s1 : s2).reunioes[i], sizeof(f2.reunioes[i]));
    }
    if (rand() % 100 < MUTACAO)
        mutacao(f2);

    calFO(f2);
}

void ordenacao(){
    int escolhido, j, i;

    for (int i = 1; i < TAM_POP + QTD_CRUZ; i++) {
        escolhido = pop[i].fo;
        Solucao aux;
        memcpy(&aux, &pop[i], sizeof(aux));
        j = i - 1;

        while ((j >= 0) && (pop[j].fo > escolhido)) {

            memcpy(&pop[j+1], &pop[j], sizeof(pop[j+1]));
            j--;
        }

        memcpy(&pop[j+1], &aux, sizeof(pop[j+1]));
    }
}

void crossover(){
    for (int i = 0; i < QTD_CRUZ/2; i++){
        cruzamento(pop[rand() % 10], pop[rand() % 30], pop[TAM_POP + (i*2)], pop[TAM_POP + ((i*2)+1)]);
    }
    ordenacao();
}

void epidemia(){
    Solucao melhor;
    memcpy(&melhor, &pop[0], sizeof(melhor));
    gerarPop();
    memcpy(&pop[0], &melhor, sizeof(melhor));
    ordenacao();
}
