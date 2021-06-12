#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<stdbool.h>

#define N 26 //liczba zmiennych w programie (a,b,...,z)
#define WARTOSC_a 97 //wartosc litery a w ASCII
#define ADD 65 // wartosc litery A w ASCII
#define CLR 67 // wartosc litery C w ASCII
#define DJZ 68 // wartosc litery D w ASCII
#define INC 73 // wartosc litery I w ASCII
#define HLT 72 // wartosc litery H w ASCII
#define JMP 74 // wartosc litery J w ASCII
#define PODSTAWA_SYSTEMU 10000 // podstawa systemu liczbowego wykorzystywanego w programie

//struktura zawierajaca informacje o poleceniu: komende, parametr1 (adres lub zmienna) oraz parametr2 (adres lub zmienna)
typedef struct{
    char komenda[5];
    int parametr1;
    int parametr2;
}Polecenie;

//struktura zawierajaca informacje o stosie: jego zawartosc, rozmiar i pierwszy z wierzchu element
typedef struct{
	int *tablica;
	int rozmiar;
	int wierzch;
}Sstos;

//struktura zawierajaca informacje o liczbach (a,b,...,z): ich wartosc, rozmiar
typedef struct{
	int **tablica;
	int rozmiar[N];
}Zzmienne;

//funkcja nadaje wartosc rowna 1 kazdemu elementowi tablicy zawierajacej poczatkowe dlugosci zmiennych (a,b,...,z)
void nadaj_dlugosc_jeden(int* tab){
    int i;
    for(i=0;i<N;i++)
        tab[i]=1;
}

//funkcja nadaje poczatkowo zmiennym (a,b,...,z) wartosc 0
void wyzeruj(int **tab){
    int i;
    for(i=0;i<N;i++)
        tab[i][0]=0;
}

//funkcja przyjmuje jako argument tablice ze zmiennymi programu (a,b,...,z) i tworzy 26 tablic, po jednej dla kazdej zmiennej
void alokuj_zmienne(int ***tab){
    int i;
    *tab=(int**)malloc((long unsigned int)N*sizeof(int*));
    for(i = 0; i < N ; i++ )
        (*tab)[i]=(int*)malloc((long unsigned int)1*sizeof(int));
    //nadajemy wszystkim zmiennym poczatkowo wartosc 0
    wyzeruj(*tab);
}

//funkcja inicjalizuje zmienne (a,b,...,z)
void inicjalizuj_zmienne(Zzmienne *zmienne){
	nadaj_dlugosc_jeden( zmienne->rozmiar);
	alokuj_zmienne( &zmienne->tablica);
}
//funkcja inicjalizuje stos
void inicjalizuj_stos(Sstos *stos){
	(*stos).rozmiar = 1;
    (*stos).tablica = malloc(sizeof(stos));
    (*stos).wierzch = 0;
}

//funkcja zwieksza argumenty dwukrotnie + 1
int wiecej(int a){
    return (2*a + 1);
}

//funkcja uwalnia pamiec zarezerwowana na tablice t_zmiennych
void uwolnij_pamiec( int ***t_zmiennych ){
	int i;
	for( i = 0; i < N; i++)
        free((*t_zmiennych)[i]);
    free(*t_zmiennych);
}

//funkcja sprawdza czy argument jest znakiem rownosci
bool wczytano_rowna_sie( int c ){
	return( c == '=' );
}

//funkcja sprawdza czy potrzebna jest realokacja pamieci
bool brakuje_pamieci( int rozmiar, int licznik){
	return ( licznik + 1 >= rozmiar );
}

//funkcja realokuje pamiec w tablicy
void realokuj( int **tablica, int *rozmiar){
	(*rozmiar) = wiecej( *rozmiar );
	*tablica = realloc((*tablica), (long unsigned int)(*rozmiar) * sizeof **tablica);
}

//funkcja realokuje pamiec w tablicy o jedno dodatkowe miejsce
void realokuj_o_jeden( Zzmienne *zmienne, int x ){
    ((*zmienne).rozmiar[x])++;
    (*zmienne).tablica[x] =(int*) realloc((*zmienne).tablica[x],(long unsigned int)(*zmienne).rozmiar[x] * sizeof *(*zmienne).tablica[x]);
    (*zmienne).tablica[x][(*zmienne).rozmiar[x]-1]=0;
}

//funckja realokuje pamiec w strukturze
void realokuj_strukture( Polecenie **tab, int *roz_tab){
	(*roz_tab) = wiecej(*roz_tab);
	(*tab) = realloc((*tab), (long unsigned int)(*roz_tab)* sizeof **tab);
}

//funkcja zapisuje polecenie JMP w tablicy struktur Polecenie *tab
void dopisz_skok( Polecenie **tab, Sstos *stos , int *roz_tab, int *nr_tab){
	if ( brakuje_pamieci( *roz_tab, *nr_tab ) )
        realokuj_strukture( tab, roz_tab );
	strcpy((*tab)[(*nr_tab)].komenda, "JMP");
	((*stos).wierzch)--;
	(*tab)[(*nr_tab)].parametr1 = ((*stos).tablica)[((*stos).wierzch)];
	(*tab)[((*stos).tablica)[((*stos).wierzch)]].parametr2 = (*nr_tab) + 1;
	(*nr_tab)++;
}

//funkcja zapisuje polecenie INC w tablicy struktur Polecenie *tab
void dopisz_inkrementacje( Polecenie **tab, int *znak, int *roz_tab, int *nr_tab, int j ){
	if ( brakuje_pamieci( *roz_tab, *nr_tab ) )
        realokuj_strukture( tab, roz_tab );
	strcpy((*tab)[(*nr_tab)].komenda, "INC");
    (*tab)[(*nr_tab)].parametr1 = znak[j];
    (*nr_tab)++;
}

//funkcja zapisuje polecenie CLR w tablicy struktur Polecenie *tab
void dopisz_wyczyszczenie( Polecenie **tab, int *nr_tab, int *roz_tab, int zmienna0){
	if ( brakuje_pamieci(*roz_tab, *nr_tab) )
        realokuj_strukture(tab, roz_tab);
	strcpy((*tab)[(*nr_tab)].komenda, "CLR");
    (*tab)[(*nr_tab)].parametr1 = zmienna0;
    (*nr_tab)++;
}

//funkcja zapisuje polecenie ADD w tablicy struktur Polecenie *tab
void dopisz_dodawanie( Polecenie **tab, int *nr_tab, int *roz_tab, int *znak, int zmienna0, int l){
	if ( brakuje_pamieci(*roz_tab, *nr_tab) )
        realokuj_strukture(tab, roz_tab);
	strcpy((*tab)[*nr_tab].komenda, "ADD");
    (*tab)[(*nr_tab)].parametr1 = znak[l];
    (*tab)[(*nr_tab)].parametr2 = zmienna0;
    (*nr_tab)++;
}

//funkcja zapisuje polecenie DJZ w tablicy struktur Polecenie *tab
void dopisz_djz( Polecenie **tab, int *nr_tab, int *roz_tab, int *znak, int j){
	if ( brakuje_pamieci(*roz_tab, *nr_tab) )
        realokuj_strukture(tab, roz_tab);
	strcpy((*tab)[(*nr_tab)].komenda, "DJZ");
    (*tab)[(*nr_tab)].parametr1 = znak[j+1];
    (*nr_tab)++;
}

//funkcja zapisuje polecenie HLT w tablicy struktur Polecenie *tab
void dopisz_halt( Polecenie **tab, int *roz_tab, int nr_tab){
	if ( brakuje_pamieci( *roz_tab, nr_tab ) )
    	realokuj_strukture( tab, roz_tab );
    strcpy( (*tab)[nr_tab].komenda, "HLT" );
}

//funkcja inkrementuje zmienna podana w argumencie
void inkrementuj(Polecenie* tab, Zzmienne *zmienne, int i){
    int j = 0;
    int x = tab[i].parametr1 - WARTOSC_a;
    (*zmienne).tablica[x][0] = (*zmienne).tablica[x][0] + 1;
    while( (*zmienne).tablica[x][j] > PODSTAWA_SYSTEMU - 1 ){
    	if( brakuje_pamieci( (*zmienne).rozmiar[x], j ) )
    		realokuj_o_jeden(zmienne,x);
        (*zmienne).tablica[x][j] = 0;
        j++;
        (*zmienne).tablica[x][j]++;
    }
}

//funkcja "czysci", czyli zeruje zmienna podana w argumencie i zmniejsza zaalokowana pamiec
void czysc(Polecenie* tab, Zzmienne *zmienne, int i){
    int x = tab[i].parametr1 - WARTOSC_a;
    free((*zmienne).tablica[x]);
    (*zmienne).rozmiar[x] = 1;
    (*zmienne).tablica[x] = (int*) malloc((long unsigned int)(*zmienne).rozmiar[x]*sizeof(int));
    (*zmienne).tablica[x][0] = 0;
}

//funkcja wypelnia zerami nowo zaalokowane elementy w tablicy t_zmiennych
void wypelnij_zerami(Zzmienne *zmienne,int x, int pom){
    while ( pom < (*zmienne).rozmiar[x] ){
        (*zmienne).tablica[x][pom] = 0;
        pom++;
	}  	
}

//funkcja wyrownuje dlugosc zmiennej x do dlugosci zmiennej y
void wyrownaj_dlugosci( Zzmienne *zmienne, int x, int y){
	int pom = (*zmienne).rozmiar[x];
    (*zmienne).rozmiar[x] = (*zmienne).rozmiar[y];
	(*zmienne).tablica[x] =(int*) realloc((*zmienne).tablica[x],(long unsigned int)(*zmienne).rozmiar[x] * sizeof *(*zmienne).tablica[x]);
	wypelnij_zerami(zmienne, x, pom);
}

//funkcja odejmuje podstawe systemu liczbowego od liczby podanej w argumencie
void odejmij_podstawe_systemu( Zzmienne *zmienne, int x, int j){
	if( brakuje_pamieci( (*zmienne).rozmiar[x], j ) )
        realokuj_o_jeden( zmienne, x );
    ( (*zmienne).tablica[x][j + 1] )++;
    (*zmienne).tablica[x][j] -= PODSTAWA_SYSTEMU;
}

//funkcja nadaje wartosc wszystkim cyfrom w tyle liczby wartosc 9, gdy przy dekrementacji zmniejszyl sie rozmiar liczby
void aktualizuj_wartosci(int ***t_zmiennych, int x, int i){
	(*t_zmiennych)[x][i]--;
    int j;
    for(j=i-1;j>=0;j--)
    	(*t_zmiennych)[x][j]=PODSTAWA_SYSTEMU - 1;
}

//funkcja zmniejsza rozmiar zmiennej
void zmniejsz_rozmiar( Zzmienne *zmienne, int x){
	(*zmienne).rozmiar[x]--;
    (*zmienne).tablica[x] = (int*) realloc ((*zmienne).tablica[x],(long unsigned int)(*zmienne).rozmiar[x] * sizeof *(*zmienne).tablica[x]);
}

//funkcja dodaje wartosc parametru2 do parametru1, podane sa one w argumencie jako element struktury Polecenie* tab
void dodaj(Polecenie *tab, Zzmienne *zmienne, int i){
    int j;
    int x = tab[i].parametr1 - WARTOSC_a; //do tej wartosci dodajemy y
    int y = tab[i].parametr2 - WARTOSC_a; //ta wartosc dodajemy do x
    if((*zmienne).rozmiar[x] < (*zmienne).rozmiar[y])
    	wyrownaj_dlugosci(zmienne, x,y);
    	
    for(j = 0 ; j < (*zmienne).rozmiar[x] && j < (*zmienne).rozmiar[y] ; j++ ){
        (*zmienne).tablica[x][j] += (*zmienne).tablica[y][j];
        if( (*zmienne).tablica[x][j] > PODSTAWA_SYSTEMU - 1 )
        	odejmij_podstawe_systemu( zmienne, x, j );
	}
}

//funkcja dekrementuje zmienna
void dekrementuj(int x,  Zzmienne *zmienne){
    int i=0;
    bool stop=false;
    if( (*zmienne).tablica[x][0] > 0 )
        (*zmienne).tablica[x][0]--;
    else{
        while( (*zmienne).tablica[x][i] == 0 && !stop ){
            if( i == (*zmienne).rozmiar[x])
                stop=true;
            else
                i++;
        }
        if( !stop )
        	aktualizuj_wartosci(&zmienne->tablica, x, i);
    }
    if( ((*zmienne).rozmiar[x] > 1) && ((*zmienne).tablica[x][(*zmienne).rozmiar[x]-1] == 0) )
    	zmniejsz_rozmiar(zmienne, x);
}

//funkcja wykonuje polecenie "decrement or jump if zero"
void dekrementuj_lub_skocz( Polecenie *tab, Zzmienne *zmienne, int *i){
	if( (*zmienne).tablica[tab[*i].parametr1 - WARTOSC_a][(*zmienne).rozmiar[tab[*i].parametr1-WARTOSC_a] - 1] == 0 )
        *i=tab[*i].parametr2;
    else{
        int x = tab[*i].parametr1 - WARTOSC_a;
        dekrementuj(x, zmienne);
        (*i)++;
    }
}

//program przyjmuje jako argument tablice struktur zawierajacych informacje o kolejnych poleceniach oraz tablice zmiennych (a,b,...,z)
//funkcja wykonuje kolejne polecenia zapisane w kodzie maszynowym
void wykonaj_polecenia(Polecenie* tab, Zzmienne *zmienne){
    int i = 0;
    int halt = 0;
    while( ( i < INT_MAX ) && ( !halt ) ){
        switch(tab[i].komenda[0]){
            case INC:
                inkrementuj(tab, zmienne,i);
				i++;
                break;
            case ADD: 
                dodaj(tab, zmienne,i);
				i++;
                break;
            case CLR:
                czysc(tab, zmienne, i);
				i++;
                break;
            case JMP:
                i=tab[i].parametr1;
                break;
            case DJZ:
            	dekrementuj_lub_skocz( tab, zmienne, &i );
                break;
            case HLT:
                halt=1;
                break;
        }
    }
}

//funkcja optymalizuje kod maszynowy
void optymalizuj(Polecenie **tab, int *znak, int j, int k, int *nr_tab, int *roz_tab){
    int zmienna0 = znak[j+1];
    int l;
    for(l = j + 2; l < k; l++){
        dopisz_dodawanie( tab, nr_tab, roz_tab, znak, zmienna0, l);
    }
    dopisz_wyczyszczenie( tab, nr_tab, roz_tab, zmienna0);
}

//funkcja standardowo tlumaczy kod, tj. bez optymalizacji
void nie_optymalizuj(Polecenie **tab, int *znak, int j, int k, int *nr_tab, int *roz_tab){
    dopisz_djz( tab, nr_tab, roz_tab, znak, j);
    int l;
    for(l = j + 2; l < k; l++)
        dopisz_inkrementacje(tab, znak, roz_tab, nr_tab, l);
}

//funkcja dodaje na wierzch stosu nr ostatniego polecenia DJZ
void wrzuc_na_stos( Sstos *stos, int nr_tab ){
	if( brakuje_pamieci( ((*stos).rozmiar), (*stos).wierzch) )
	    realokuj( &stos->tablica, &stos->rozmiar );
	((*stos).tablica)[((*stos).wierzch)]  = nr_tab;
	((*stos).wierzch)++;
}

//funkcja sprawdza czy miedzy niezagniezdzonymi nawiasami pierwsza zmienna wystepuje wiecej niz jeden raz
bool zmienna_sie_powtarza( int* znak, int j, int k ){
    int i = j + 2;
    int wynik = false;
    while( i < k && !wynik ){
        if( znak[i] == znak[j+1] )
            wynik = true;
        i++;
    }
    return wynik;
}

//funkcja wykonuje dzialania po napotkaniu nawiasu otwierajacego w wierszu
void otworz_nawias( Polecenie **tab, Sstos *stos, int *znak, int *roz_tab, int *nr_tab, int *j, bool *zag ){ 
	int k = *j + 1;
	while( ( znak[k] != ')' ) && ( znak[k] != '(' ) ) // dopoki nie napotkamy kolejnego nawiasu
	    k++;
	//sprawdzamy rodzaj nawiasu
	if( znak[k] == '(' ){
		wrzuc_na_stos( stos, *nr_tab );
	    nie_optymalizuj( tab, znak, *j, k, nr_tab, roz_tab );
	}
	else{ // znak[k] == ')'
	    if( zmienna_sie_powtarza( znak, *j,k ) ){
	        wrzuc_na_stos( stos, *nr_tab );
	        nie_optymalizuj( tab, znak, *j, k, nr_tab, roz_tab );
	    }
	    else{
	    	optymalizuj( tab, znak, *j, k, nr_tab, roz_tab );
	        *zag = true;
	    }
	}
	*j = k - 1;
}

//funkcja wykonuje dzialania po napotkaniu nawiasu otwierajacego w wierszu
void zamknij_nawias( Polecenie **tab, Sstos *stos, int *roz_tab, int *nr_tab, bool *zag){
	if( *zag )
        *zag = false;
    else
        dopisz_skok( tab, stos , roz_tab, nr_tab );
}

//funkcja tlumaczy znaki zapisane w tablicy "znak" na kod maszynowy
void przetlumacz_na_kod( Polecenie **tab, int *roz_tab, int* znak, int rozmiar ){
    Sstos stos;
    inicjalizuj_stos(&stos);
    int nr_tab = 0; //numer polecenia
    bool zag = false; // czy jestesmy po optymalizacji lub wyczyszczeniu zmiennej
    int j = 0;
    while( j < rozmiar ){ //dopoki nie doszlismy do konca tablicy
    	switch( znak[j] ){
    		case '(':
    			otworz_nawias(tab, &stos, znak, roz_tab, &nr_tab, &j, &zag);
	            break;
            case ')':
            	zamknij_nawias(tab, &stos, roz_tab, &nr_tab, &zag);
            	break;
            default: //litera
            	dopisz_inkrementacje(tab, znak, roz_tab, &nr_tab, j );
            	break;
		}
        j++;
    }
    dopisz_halt(tab, roz_tab, nr_tab);
    free(stos.tablica);
}

//funkcja przyjmuje jako argument tablice struktur Polecenie* tab
//funkcja wczytuje wszystkie znaki stojace w wierszu
void wczytaj_wiersz( int **znak, int *rozmiar, int *i) {
    int c;
    while ( ( c = getchar() ) != '\n' ) {
		if( brakuje_pamieci( *rozmiar, *i ) )
           	realokuj( znak, rozmiar );
        (*znak)[*i] = c;
        (*i)++;
    }
}

//funkcja wypisuje wartosc zmiennej
void wypisz_zmienna( Zzmienne *zmienne, int c ){
	c = getchar();
    int x = c - WARTOSC_a;
    int i;
    for( i = (*zmienne).rozmiar[x]-1; i >= 0; i--){
        if( i == (*zmienne).rozmiar[x] - 1 )
            printf( "%d", (*zmienne).tablica[x][i] );
        else
            printf( "%04d", (*zmienne).tablica[x][i] );
    }
    getchar();
    puts("");
}

//funkcja wykonuje program w jezyku petlik
void wykonaj_petlik( Zzmienne *zmienne ){
	int roz_znak = 0; //rozmiar tablicy ze znakami w wierszu
    int *znak = NULL; //tablica zawierajaca znaki w wierszu
    int i = 0; // liczba wczytanych znakow do tablicy *znak
    int roz_tab = 1; //rozmiar tablicy polecen
    Polecenie *tab = NULL; // tablica struktur zawierajacych polecenia w jezyku petlik
    tab = realloc( tab, (long unsigned int)roz_tab * sizeof *tab );
	wczytaj_wiersz( &znak, &roz_znak, &i );
	przetlumacz_na_kod( &tab, &roz_tab, znak, i );
	wykonaj_polecenia( tab, zmienne );
	free(tab); //uwalniamy pamiec 
	free(znak);
}

//funkcja wczytuje znaki az do konca pliku i wypisuje wartosc zmiennej lub wykonuje program w jezyku petlik
void petlik(){
	Zzmienne zmienne; // struktura zawierajaca informacje o zmiennych
	inicjalizuj_zmienne(&zmienne);
	int c;
	while ( ( ( c = getchar() ) != EOF ) ){
		if ( wczytano_rowna_sie(c) )
			wypisz_zmienna( &zmienne, c);
		else{
			ungetc( c, stdin);
			wykonaj_petlik( &zmienne);
		}
	}
	uwolnij_pamiec( &zmienne.tablica);
}

int main(void){
	petlik();
    return 0;
}
