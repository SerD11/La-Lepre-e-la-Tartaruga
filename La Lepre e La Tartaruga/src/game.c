#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include "game.h"
#include "linked_lists.h"
#include "arrays.h"
#include "gui.h"
#include "error_msg.h"
#include "ia.h"
#include "tools.h"

int newGame(players *players, array *played, deck *deck)
{
	int i, j;

	/* Resetta le Carte Giocate */
	arrLoad(played, 0);

	/* Prepara il mazzo*/
	deck->card_list = listInit();
	shuffle_deck(deck);

	/* Resetta le Carte Scommessa*/
	i = 0;
	while( i < players->n_players ){
		arrLoad(&players->player[i].bet_cards, 0);
		i++;
	}

	/* Assegna ai giocatori le prime Carte Scommessa e le Carte Corsa */
	firstBetCard(players);
	j = 0;
	while( j < players->n_players ){
		i = 0;
		while(i < MAX_CARDS){

			players->player[j].run_cards.d[i] = GetCard(deck);
			i++;
		}
		j++;
	}

	/* Stamp l'intefaccia di base*/
	printRoute();
	printAnimal(0, 0);
	printStatics();
	printRef();

	return 0;
}

int loadGame(void)
{
	return 0;
}

int settings(void)
{

	system("pause > nul");
	return 0;
}

int rules(void)
{
	system("pause > nul");
	return 0;

}

deck shuffle_deck( deck *deck )
{
	int number;
	srand(time(NULL));
	deck->totals = arrLoad(&deck->totals, 0);

	while ( deck->totals.d[0] < 81 ){
		//Sleep(100);
		number = rand()% 5 + 1;
		if (number == WOLF && deck->totals.d[1] < 16){
			deck->card_list = listAdd(deck->card_list, WOLF);
			deck->totals.d[WOLF]++;
			deck->totals.d[0]++;
		}
		if (number == HARE && deck->totals.d[2] < 18){
			deck->card_list = listAdd(deck->card_list, HARE);
			deck->totals.d[HARE]++;
			deck->totals.d[0]++;
		}
		if (number == TORTOISE && deck->totals.d[3] < 17){
			deck->card_list = listAdd(deck->card_list, TORTOISE);
			deck->totals.d[TORTOISE]++;
			deck->totals.d[0]++;
		}
		if (number == LAMB && deck->totals.d[4] < 15){
			deck->card_list = listAdd(deck->card_list, LAMB);
			deck->totals.d[LAMB]++;
			deck->totals.d[0]++;
		}
		if (number == FOX && deck->totals.d[5] < 15){
			deck->card_list = listAdd(deck->card_list, FOX);
			deck->totals.d[FOX]++;
			deck->totals.d[0]++;
		}

	}
	return *deck;
}

players create_players(players *players)
{
	int i;

	players->player = malloc( MAX_PLAYERS * sizeof(player) );
	i = 0;
	while ( i < MAX_PLAYERS ){

		// crea giocatori di default
		players->player[i].ai = false;

		players->player[i].bet_cards = arrInit( &players->player[i].bet_cards, MAX_BETS);
		players->player[i].bet_cards = arrLoad( &players->player[i].bet_cards, 0);

		players->player[i].run_cards= arrInit( &players->player[i].run_cards, MAX_CARDS);
		players->player[i].run_cards= arrLoad( &players->player[i].run_cards, 0);

		i++;
	}

	return *players;
}

players name_players(players *players_d)
{
	int i;

	i = 0;
	while ( i < MAX_PLAYERS ){

		if (players_d->player[i].ai == false){
			strcpy(players_d->player[i].name, "Giocatore");
			players_d->player[i].name[9] = 49 + i;
			players_d->player[i].name[10] = '\0';

		} else if (players_d->player[i].ai == true){
			strcpy(players_d->player[i].name, "I.A.       ");
		}

		i++;
	}

	return *players_d;
}

int GetCard(deck *deck)
{
	int card;
	//node * temp;

	card = 0;
	//temp = deck->card_list;
	// se il mazzo � finito lo rimescola
	if ( deck->totals.d[0] == 0 ){
		shuffle_deck(deck);
	}
	// Prende l'ultima carta del mazzo e la elimina
	card = listGetLast(deck->card_list);
	deck->card_list = listDLast( deck->card_list);

	deck->totals.d[card]--;
	deck->totals.d[0]--;

	return card;
}

players firstBetCard(players *players_d)
{
	int i, n;
	array betcards;
	srand(time(NULL));
	betcards = arrInit(&betcards,5);
	betcards.d[0] = WOLF;
	betcards.d[1] = HARE;
	betcards.d[2] = TORTOISE;
	betcards.d[3] = LAMB;
	betcards.d[4] = FOX;
	i = 0;

	while ( i < players_d->n_players ){
		n = rand() % 5;
		if ( betcards.d[n] != 0){
			players_d->player[i].bet_cards.d[0] = betcards.d[n];
			betcards.d[n] = 0;
			i++;
		}
	}
	free(betcards.d);

	return *players_d;
}

int secondBetCard(player player, deck *deck, array *played)
{
	int i, j, pos, input_value, previous;
	char input;
	array temp;
	bool flag;
	previous = 0;
	pos = - 1;
	arrInit(&temp, 7);
	arrLoad(&temp, 0);

	// carica il vettore temporaneo per la settima carta scommessa
	i = 0;
	while ( i < MAX_CARDS){
		temp.d[i] = player.run_cards.d[i];
		i++;
	}
	temp.d[6] = GetCard(deck);
	flag = true;
	while( flag == true){

			printPlayed(played);
			printTurn(player.name);
			printBet(&player.bet_cards);
			printHand(&temp);
		/*
		if (player.ai == true){
			// ia2betcard
		} else */if ( player.ai == false || player.ai == true ) {

			input = getch();
			input_value = input -'0';

			if ( input == 's' || input == 'S' ){ // in caso venga premuto il tasto per salvare
				//salva
				return 2;
			}
			if ( input == 27 ){ // in caso venga premuto il tasto esci
				//Torna al menu
				return 1;
			}
			if ( (input_value >= 1 && input_value <= 7)){

				pos = input_value - 1;

				if( player.bet_cards.d[1] == 0 ){
					// Seleziona la carta
					player.bet_cards.d[1] = temp.d[pos];
					printPointer(pos, false);

				}else if ( previous == pos ){
					// Rimuove il puntatore e la carta
					printPointer(pos, true);
					player.bet_cards.d[1] = 0;

				}else if ( player.bet_cards.d[1] != 0 ){
					// Sposta l'indicatore
					printPointer(previous, true);
					player.bet_cards.d[1] = temp.d[pos];
					printPointer(pos, false);
				}
			}
			if ( player.bet_cards.d[1] != 0 && input == ' ' ){
				GotoXY(0,22);
				printf("                                                                             ");
				temp.d[pos] = 0;
				flag = false;
			}
			previous = pos;
		}
	}

	// Riporta i valori nel vettore del giocatore
	i = 0;
	j = 0;
	while ( i < 7){
		if ( temp.d[i] != 0){
			player.run_cards.d[j] = temp.d[i];
			j++;
		}
		i++;
	}
	free(temp.d);
	return 0;
}

array playerGetCard(player *player, array *played, int input, array *pos)
{

	bool flag;
	int i, count;
	int played_card;
	int check = 0;

	/* Conta quante carte mancanti ha il giocatore */
	count = arrCountNotX(pos, -1);
	flag = false;

	//Impedisce di giocare carte di animali diversi
	if(count >= 1)
	{
		check = check_played_card(pos);
		played_card = player->run_cards.d[check-1];
	} else {
		played_card = player->run_cards.d[input-1];
	}

	i = 0;
	while ( i < MAX_TURN){

		if (pos->d[i] == input){
			flag = true;
			printPointer(input-1, flag);
			pos->d[i] = -1;
			count--;
			i = MAX_TURN;

		} else {
			i++;
		}
	}

	if ( (arrCountX(played, played_card) + count) < MAX_TURN && flag == false  && played_card == player->run_cards.d[input-1] && (arrCountNotX(played, 0)+ count) != 8 ){

		count++;
		arrFillavb(pos, input, -1);
		count = arrCountNotX(pos, -1);
		printPointer(input-1, flag);
	} else if ((arrCountX(played, played_card) + count) < MAX_TURN && flag == false  && count == 0 && (arrCountNotX(played, 0)+ count) != 8){

		count++;
		arrFillavb(pos, input, -1);
		count = arrCountNotX(pos, -1);
		printPointer(input-1, flag);

	}else if(flag == false){

		/* Mostra il messaggio di errore al giocatore*/
		playerErrors();
	}
	flag = false;

	return *pos;
}

//Carta selezionata, su cui si deve basare la selezione delle successive carte
int check_played_card(array *arr)
{
	int i, x;
	i = 0;
	x = 0;

	while(i < 4)
	{
		if(arr->d[i] != -1)
		{
			x = arr->d[i];
			i = 4;
		}
		i++;
	}
	return x;

}

int play(players *players, array *played, deck *deck)
{
	int turn;
	int player_decision;
	array pos;
	pos = arrInit(&pos, 4);
	pos = arrLoad(&pos, -1);
	int i, check;
	bool finish;

	finish = false;
	// Scelta delle seconde carte scommessa
	i = 0;
	while( i < players->n_players ){

		check = secondBetCard(players->player[i], deck, played);

		if ( check == 1){
			// esci
			free(pos.d);
			return 0;

		} else if ( check == 2){
			//salva
			free(pos.d);
			return 0;
		}
		i++;
	}
	/* Rimuove la settima carta dallo schermo */
	GotoXY(44, 19);
	printf("     ");
	GotoXY(44, 20);
	printf("     ");
	GotoXY(44, 21);
	printf("     ");

	// resta nel ciclo finch� non finisce la partita
	while(finish == false){

		turn = 0;
		// fase di gioco
		while ( turn < players->n_players ){
			// stampa le carte
			printPlayed(played);
			printTurn(players->player[turn].name);
			printBet(&players->player[turn].bet_cards);
			printHand(&players->player[turn].run_cards);

			// caso in cui il giocatore sia umano
			while ( players->player[turn].ai == false){
				player_decision = playerTurn(players, played, deck, &pos, turn);

				if ( player_decision == 1){
					free(pos.d);
					return 0;
				}
				if ( player_decision == 2){
					free(pos.d);
					//salva
					return 0;
				}
				if ( player_decision == 0){
					turn++;
				}
			}
			// caso in cui il giocatore sia ia
			if (players->player[turn].ai == true){

				// turno dell'ia
				printTurn(players->player[turn].name);
				printBet(&players->player[turn].bet_cards);
				printPlayed(played);
				printHand(&players->player[turn].run_cards);
				iamain( &players->player[turn].run_cards, played, players->player[turn].bet_cards.d[0], players->player[turn].bet_cards.d[1] );

				// stampa le carte
				GotoXY(25, 22);
				printf(">>L'IA STA GIOCANDO<<");
				Sleep(1500);
				printPlayed(played);

				i = 0;
				while( i < MAX_CARDS ){
					if( players->player[turn].run_cards.d[i] == 0){
						players->player[turn].run_cards.d[i] = GetCard(deck);

					}
					i++;
				}

				printPlayed(played);
				printTurn(players->player[turn].name);
				printBet(&players->player[turn].bet_cards);
				//printHand(&players->player[turn].run_cards);
				Sleep(1000);
				GotoXY(0,22);
				printf("                                                                             ");
				turn++;
			}
			printPlayed(played);
			printTurn(players->player[turn].name);
			printBet(&players->player[turn].bet_cards);
			printHand(&players->player[turn].run_cards);

		} // fase di gioco
		/*
		// Condizioni di attivazione fase di corsa
		if(
		  arrCountNotX(played, 0) == 8
		||arrCountX(played, WOLF) == 4
		||arrCountX(played, HARE) == 4
		||arrCountX(played, TORTOISE) == 4
		||arrCountX(played, LAMB) == 4
		||arrCountX(played, FOX) == 4 ) {

			runPhase(players, played);
		}
		 */
		arrLoad(played, 0);
		printPlayed(played);
		printTurn(players->player[turn].name);
		printBet(&players->player[turn].bet_cards);
		printHand(&players->player[turn].run_cards);

	} // fine partita

	//free(pos.d);
	return 0;
}

int playerTurn(players *players, array *played,deck *deck, array * pos, int turn)
{
	char input;
	int converted;
	int i;

	input = getch();
	converted = input -'0';

	if ( input == 's' || input == 'S' ){
		//salva

		return 2;
	}
	if ( input == 27 ){
		//Torna al menu

		return 1;
	}
	if ( (converted >= 1 && converted <= 6) || input == ' '){

		if ( input == ' ' && arrCountNotX(pos, -1) > 0){
			// fine turno
			/*
			// traduce le posizioni
			i = 0;
			while ( i < MAX_TURN){
				if ( pos->d[i] != -1){
					pos->d[i]--;
				}
				i++;
			}
			*/
			// gioca le carte selezionate
			i = 0;
			while ( i < MAX_TURN){
				if ( pos->d[i] != -1){
					arrFillavb(played, players->player[turn].run_cards.d[ pos->d[i]-1 ] , 0);
					players->player[turn].run_cards.d[ pos->d[i]-1 ] = 0;
				}
				i++;
			}
			// Prende le carte dal mazzo
			i = 0;
			while( i < MAX_CARDS ){
				if( players->player[turn].run_cards.d[i] == 0){
					players->player[turn].run_cards.d[i] = GetCard(deck);
				}
				i++;
			}
			// resetta le posizioni
			arrLoad(pos, -1);


			// rimuove gli indicatori
			GotoXY(0,22);
			printf("                                                                             ");
			return 0;

		} else if (input != ' ' ) {
			playerGetCard( &players->player[turn], played, converted, pos);
		}
	}

	return 4;

}

int runPhase(players *players, array *played)
{
	array path;
	array run;
	int move;
	int i;
	arrInit( &path, 11);
	arrLoad( &path,0);
	arrInit( &run, 6);
	arrLoad( &run,0);
	path.d[3] = 1;
	path.d[7] = 1;

	move = 0;
	//spostamento lupo
	if ( arrCountX(played, WOLF) == 1 || arrCountX(played, WOLF) == 2 ){
		move = 1;
		i = 0;
		while ( run.d[WOLF]  < move + run.d[WOLF] ){
			run.d[WOLF] += i;
			printAnimal(WOLF, i + run.d[WOLF] );
			i++;
		}

		printAnimal(WOLF, move + run.d[WOLF] );
		run.d[WOLF] += move;

	} else {
		move = arrCountX(played, WOLF) - 1;

		i = 0;
		while ( run.d[WOLF]  < move + run.d[WOLF] ){
			run.d[WOLF] += i;
			printAnimal(WOLF, i + run.d[WOLF] );
			i++;
		}
	}

	//spostamento lepre
	if ( arrCountX(played, HARE) > 0  ) {
		move = 2;

		i = 0;
		while ( run.d[HARE]  < move + run.d[HARE] ){
			run.d[HARE] += i;
			printAnimal(HARE, i + run.d[HARE] );
			i++;
		}
	}

	//spostamento tartaruga
	if ( arrCountX(played, TORTOISE) >= 0  &&  arrCountX(played, TORTOISE) <= 3 ) {
		move = 1;

		i = 0;
		while ( run.d[TORTOISE]  < move + run.d[TORTOISE] ){
			run.d[TORTOISE] += i;
			printAnimal(TORTOISE, i + run.d[TORTOISE] );
			i++;
		}
	} else {
		move = 2;

		i = 0;
		while ( run.d[TORTOISE]  < move + run.d[TORTOISE] ){
			run.d[TORTOISE] += i;
			printAnimal(TORTOISE, i + run.d[TORTOISE] );
			i++;
		}
	}

	//spostamento agnello
	if ( arrCountX(played, LAMB) > 0 ) {
		move = arrCountX(played, LAMB) + 1;

		i = 0;
		while ( run.d[LAMB]  < move + run.d[LAMB] ){
			run.d[LAMB] += i;
			printAnimal(LAMB, i + run.d[LAMB] );
			i++;
		}
	}
	//spostamento volpe
	if ( arrCountX(played, FOX) > 0 ) {
		move = arrCountX(played, FOX);

		i = 0;
		while ( run.d[LAMB]  < move + run.d[LAMB] ){
			run.d[LAMB] += i;
			printAnimal(LAMB, i + run.d[LAMB] );
			i++;
		}
	}


	free(path.d);
	free(run.d);

	return 0;
}


void errorHandle(int error)
{
	if ( error == 1){
		printErrorGeneric();

	} else {
		printErrorGeneric();

	}
	system("pause >nul");
	return ;
}

int initConsole(void)
{
	system("mode 80,25");

	hidecursor();

	consoleTitle("La Lepre e La Tartaruga");

	return 0;
}
