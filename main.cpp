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

    for (int i = 0; i < 20; i++){
        zerarVariaveis();
        heuCon();
        calFO();
    }

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
        listaProfessores[i].qtdAulasPerdidas = listaProfessores[i].qtdReuTercTurno = listaProfessores[i].qtdAulasNoiteAntesReuniao = 0;
    }
    qtdAulasPerdidasTotal = qtdReuTerTurnoTotal = qtdAulasNoiteAntesReuniTotal = 0;
}

void heuCon() {
    int dia = 0, x, y, horario;

    for (int i = 0; i < 9; i++) { //reunioes
        x = (int) (dia / 7);
        y = dia % 7;
        if (calendario.matriz[x][y]) {

            reunioes[i].data[0] = x;
            reunioes[i].data[1] = y;

            horario = rand() % 4;
            reunioes[i].horario = horario;

            //printf("Reu: %i\tDia: %i\tX: %i, Y: %i\tHorario: %i\n", i, dia, x, y, reunioes[i].horario);

            dia += (rand() % ALPHA) + 10;
        } else {
            dia++;
            i--;
        }
    }


}

void calRestricoes() {
    for (int i = 0; i < 9; i++) { //reunioes
        for (int j = 0; j < qtdProf; j++) { //professores
            //Aulas perdidas
            if (listaProfessores[j].horario[reunioes[i].horario][reunioes[i].data[1] - 1]) { //-1 porque a matriz de calendário conta sábado e domingo, sendo que a de horario de professores não
                listaProfessores[j].qtdAulasPerdidas++;
                qtdAulasPerdidasTotal++;
            }

            //Aulas de noite antes da reuniao
            if ((listaProfessores[j].horario[4][reunioes[i].data[1] - 2] || listaProfessores[j].horario[5][reunioes[i].data[1] - 2]) && (reunioes[i].horario == 0 || reunioes[i].horario == 1)) { //-2 porque a matriz de calendário conta sábado e domingo, sendo que a de horario de professores não
                listaProfessores[j].qtdAulasNoiteAntesReuniao++;
                qtdAulasNoiteAntesReuniTotal++;
            }

            //Terceiro turno
            if (listaProfessores[j].tercTunro[(int)(reunioes[i].horario/2)][reunioes[i].data[1] - 1]) {
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

float calRestricao6(){
    float resultado = 0;
    for (int i = 0; i < 4; i++){
        resultado += abs(calQtdReuMes(i) - 2);
    }
    resultado += abs(calQtdReuMes(4) - 1);
    return resultado;
}

float calRestricao7(){
    float resultado = 0;
    for (int i = 1; i < 9; i++){
        int dia1 = (reunioes[i-1].data[0] * 7) + reunioes[i-1].data[1] + 1;
        int dia2 = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
        resultado += MAX(0, 10 - dia2 - dia1);
    }
    return resultado;
}

int calQtdReuMes(int op){
    int qtd = 0;
    switch(op){
        case 0:
            for (int i = 0; i < 9; i++){
                int dia = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
                if(dia <= 22){
                    qtd++;
                }
            }
            break;
        case 1:
            for (int i = 0; i < 9; i++){
                int dia = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
                if((dia > 22 ) && (dia <= 52)){
                    qtd++;
                }
            }
            break;
        case 2:
            for (int i = 0; i < 9; i++){
                int dia = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
                if((dia > 52 ) && (dia <= 83)){
                    qtd++;
                }
            }
            break;
        case 3:
            for (int i = 0; i < 9; i++){
                int dia = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
                if((dia > 83) && (dia <= 113)){
                    qtd++;
                }
            }
            break;
        case 4:
            for (int i = 0; i < 9; i++){
                int dia = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
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

void calFO(){
    calRestricoes();

    printf("Teste: %i %f %i %i %f %f %f\n", qtdAulasPerdidasTotal, calRestricao2(), qtdAulasNoiteAntesReuniTotal, qtdReuTerTurnoTotal, calRestricao5(), calRestricao6(), calRestricao7());
    float fo = (pesos[0] * qtdAulasPerdidasTotal) +
            (pesos[1] * calRestricao2()) +
            (pesos[2] * qtdAulasNoiteAntesReuniTotal) +
            (pesos[3] * qtdReuTerTurnoTotal) +
            (pesos[4] * calRestricao5()) +
            (pesos[5] * calRestricao6()) +
            (pesos[6] * calRestricao7());

    printf("FO: %i\n\n", (int)fo);
    escreverResultado();
}

void escreverResultado(){
    printf("Reunioes: \n");
    for (int i = 0; i < 9; i++){
        int dia = (reunioes[i].data[0] * 7) + reunioes[i].data[1] + 1;
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

        switch (reunioes[i].data[1]){
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

        printf("Horario: %i\n", reunioes[i].horario);
    }
    printf("\n");

    for (int i = 0; i < qtdProf; i++){
        printf("Prof %i\tAulas Perdidas %i\tReu Ter Turno: %i\n", i, listaProfessores[i].qtdAulasPerdidas, listaProfessores[i].qtdReuTercTurno);
    }

    printf("\nAulas perdidas total: %i\tReuni Ter Turno total: %i\n", qtdAulasPerdidasTotal, qtdAulasNoiteAntesReuniTotal);

    printf("\n---------------------------\n");
}
