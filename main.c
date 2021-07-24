#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
//#include "grafo_matrice.h"
//#include "albero.h"
//#include "dijstra.h"

typedef struct grafo{
    unsigned int numero_nodi;
    unsigned int ordine_entrata;
    unsigned int **matrice;
    unsigned int metrica;

} *grafo;

typedef struct classifica{
    unsigned int grandezzaClassifica;
    unsigned int prossimoIndice;
    struct elementoClassifica *elem;

} *classifica;

typedef struct elementoClassifica{
    unsigned int ordine_entrata;
    unsigned int metrica;
} elementoClassifica;

//prototipi
void stampaGrafo(grafo g, unsigned int stampaMetrica);
unsigned int calcolaMetrica(grafo g);
unsigned int minDistance(grafo graph, unsigned int dist[], bool sptSet[]);
void printSolution(grafo graph, unsigned int dist[]);
unsigned int dijkstra(grafo graph, unsigned int src);
void inserisciMetrica(grafo g, classifica c);
void stampaClassifica(classifica c, unsigned int stampaMetrica);
void buildMaxHeap(classifica c);
void maxHeapify(classifica c,unsigned int i);


int main(){
    // Variables
    // d = Nodi del Grafo
    // k = Grandezza della Classifica
    unsigned int numero_nodi,k;
    char inputLine[14];
    unsigned int inputIndex = 0;
    grafo g = NULL;

    if(scanf("%u %u",&numero_nodi,&k)==0) printf("error");
    //printf("\nNumero di nodi richiesti: %u", numero_nodi);
    //printf("\nGrandezza Classifica: %u", k);

    // Inizializzo classifica con struttura ad albero
    classifica c = (classifica) malloc(sizeof(struct classifica));
    c->grandezzaClassifica = k;
    c->prossimoIndice = 0;
    c->elem = (elementoClassifica*) malloc(sizeof(elementoClassifica) * k);

    // Scorro le linee del file in ingresso
    while (scanf("%s",inputLine)!=EOF){

        if (inputLine[0]=='A'){

            //Start: Inizializzo il Grafo
           //printf("\nInizializzo il Grafo");
            g = (grafo) malloc(sizeof(struct grafo));
            g->numero_nodi = numero_nodi;
            g->ordine_entrata = inputIndex;
            //Alloco spazio per la matrice
            g->matrice = (unsigned int **)malloc( numero_nodi * sizeof(unsigned int*));// Dubbio se spostare allocazione fuori dalla funzione aggiungi grafo così è più efficente.
            for (unsigned int i=0;i<numero_nodi;i++) g->matrice[i]=(unsigned int*) malloc(numero_nodi* sizeof(unsigned int));
            // Aumento l'ordine di entrata
            inputIndex++;
            unsigned int metricaZero=0;
            for (unsigned int i = 0; i < numero_nodi; i++)
            {
                for (unsigned int j = 0; j < numero_nodi; j++)
                {
                    if(scanf("%u", &g->matrice[i][j])==0) printf("error");  //capire se posso ottimizzare questa scansione
                    if (i == 0 && g->matrice[i][j] == 0) metricaZero+=1;
                    if(scanf(",") != 0) printf("error");
                }
            }
            // End: Grafo Popolato

            // Calcola Somma Cammini Minimi
            if (metricaZero == numero_nodi) g->metrica = 0; // nodo sconnesso ha metrica 0
            else g->metrica = calcolaMetrica(g);

            // Stampa Grafo
            //stampaGrafo(g,1);

            // Inserisci in Classifica
            inserisciMetrica(g, c);

        } else if(inputLine[0] == 'T'){
            if (g == NULL) printf("\n");
            else stampaClassifica(c,1);
        }

    }   // end while


    // Libero la memoria
    free(g);
    free(c);

    return 0;
}

void stampaGrafo(grafo g, unsigned int stampaMetrica)
{
    //printf("\nNumero nodi: %u", g->numero_nodi);
    //printf("\nOrdine entrata: %u\n", g->ordine_entrata);

    for (unsigned  i = 0; i < g->numero_nodi; i++)
    {
        for (unsigned int j = 0; j < g->numero_nodi; j++)
        {
            printf("\t%u ", g->matrice[i][j]);
        }
        printf("\n");
    }
    if (stampaMetrica == 1) printf("\nMetrica: %u\n", g->metrica);
}

unsigned int calcolaMetrica(grafo g)
{
    return dijkstra(g, 0);
}

unsigned int minDistance(grafo graph, unsigned int dist[], bool sptSet[])
{
    // Inizializzo valori
    unsigned int min = INT_MAX, min_index=0;

    for (unsigned int v = 0; v < graph->numero_nodi; v++)
        if (sptSet[v] == false && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    return min_index;
}

// stampo il peso dei singoli cammini minimi
void printSolution(grafo graph, unsigned int dist[])
{
    printf("Vertex \t\t Distance from Source\n");
    for (unsigned int i = 0; i < graph->numero_nodi; i++)
        printf("%d \t\t %d\n", i, dist[i]);
}


unsigned int dijkstra(grafo graph, unsigned int src)
{
    unsigned int dist[graph->numero_nodi]; // array dist[i] contiene i cammini minimi

    bool sptSet[graph->numero_nodi]; // array sptSet[i] è vero se il vertice include il cammino minimo oppure la distanza minima è finalizzata

    //Inizializzo tutte le distanze come infinito e stpSet[] come falso
    for (unsigned int i = 0; i < graph->numero_nodi; i++)
        dist[i] = INT_MAX, sptSet[i] = false;

    //distanza dal vertice con se stesso è nulla
    dist[src] = 0;

    //trovo i cammini minimi per ogni vertice
    for (unsigned int count = 0; count < graph->numero_nodi - 1; count++) {
        // Prendo la distanza minima del vertice da quelli non ancora processati, u è uguale a src alla prima iterazione
        unsigned int u = minDistance(graph, dist, sptSet);

        // marco il vertice preso come processato
        sptSet[u] = true;

        // aggiorno il valore della distanza dei vertici adiacenti di quello preso
        for (unsigned int v = 0; v < graph->numero_nodi; v++)

            // Aggiorna dist[v] solo se non è in sptSet, c'è un arco da u a v
            // e il peso totale del percorso da src a v attraverso u è
            // minore del valore corrente di dist[v]
            if (!sptSet[v] && graph->matrice[u][v] && dist[u] != INT_MAX
                && dist[u] + graph->matrice[u][v] < dist[v])
                dist[v] = dist[u] + graph->matrice[u][v];
    }

    // stampo i singoli cammini minimi
    //printSolution(graph, dist);

    //ottimizzabile nell'algoritmo
    unsigned int retValue = 0;
    for (unsigned int i = 0; i < graph->numero_nodi; i++){
        if (dist[i] == INT_MAX) dist[i] = 0;
        retValue += dist[i];
    }

    return retValue;
}

void inserisciMetrica(grafo g, classifica c){
    //printf("\nInserisco elemento in %u", c->prossimoIndice);
    //printf("\nMetrica Massima %u", c->metricaMassima);

    if (g->ordine_entrata < c->grandezzaClassifica){
        c->elem[c->prossimoIndice].metrica=g->metrica;
        c->elem[c->prossimoIndice].ordine_entrata=g->ordine_entrata;
        c->prossimoIndice+=1;
    } else if(g->ordine_entrata == c->grandezzaClassifica){
        buildMaxHeap(c);
        if (g->metrica < c->elem[0].metrica){
            c->elem[0].metrica=g->metrica;
            c->elem[0].ordine_entrata=g->ordine_entrata;
            maxHeapify(c,0);
        }

    } else if(g->ordine_entrata > c->grandezzaClassifica){
        if (g->metrica < c->elem[0].metrica){
            c->elem[0].metrica=g->metrica;
            c->elem[0].ordine_entrata=g->ordine_entrata;
            maxHeapify(c,0);
        }
    }
}

void stampaClassifica(classifica c, unsigned int stampaMetrica){

   if (c->prossimoIndice < c->grandezzaClassifica){
       for (unsigned int i = 0; i < c->prossimoIndice - 1; i++) {
           printf("%u ", c->elem[i].ordine_entrata);
       }
       //printf("\ngrandezza classifica: %u\n",c->grandezzaClassifica);
       printf("%u\n",c->elem[c->prossimoIndice-1].ordine_entrata);
   }else {
       for (unsigned int i = 0; i < c->grandezzaClassifica - 1; i++) {
       printf("%u ", c->elem[i].ordine_entrata);
        }
        //printf("\ngrandezza classifica: %u\n",c->grandezzaClassifica);
        printf("%u\n",c->elem[c->grandezzaClassifica-1].ordine_entrata);  //ultimo valore va stampato senza spazio ma con accapo
   }
    if (stampaMetrica == 0){
        printf("TopK: ");
        for (unsigned int i = 0; i < c->grandezzaClassifica -1; i++)
        {
            printf("%u ", c->elem[i].metrica);
        }
        printf("%u\n", c->elem[c->grandezzaClassifica-1].metrica);  //ultimo valore va stampato senza spazio ma con accapo
    }

}

void buildMaxHeap(classifica c){
    for (unsigned int i = (c->grandezzaClassifica/2 -1); i >=0 ; i--) {
        maxHeapify(c,i);
        //serve per non andare in segmentation fault
        if (i == 0) break;
    }
}

void maxHeapify(classifica c,unsigned int i){
   unsigned int temp1,temp2,l,r,maggiore,violazione=1;
    while (violazione){
        l=2*i+1;
        r=2*i+2;
        maggiore=i;
        if (l < c->grandezzaClassifica && c->elem[l].metrica > c->elem[i].metrica) maggiore=l;
        else if(l<c->grandezzaClassifica && c->elem[l].metrica == c->elem[i].metrica && c->elem[l].ordine_entrata > c->elem[i].ordine_entrata) maggiore = l;
        if (r < c->grandezzaClassifica && c->elem[r].metrica > c->elem[maggiore].metrica) maggiore=r;
        else if(r < c->grandezzaClassifica && c->elem[r].metrica == c->elem[i].metrica && c->elem[r].ordine_entrata > c->elem[i].ordine_entrata) maggiore = r;

        if (maggiore != i){
            //migliorabile la scrittura senza usare temp1 e temp2
           temp1 = c->elem[maggiore].metrica;
           temp2 = c->elem[maggiore].ordine_entrata;
           c->elem[maggiore].metrica = c->elem[i].metrica;
           c->elem[maggiore].ordine_entrata = c->elem[i].ordine_entrata;
           c->elem[i].metrica = temp1;
           c->elem[i].ordine_entrata = temp2;
           i=maggiore;
        } else  violazione=0;
    }
}