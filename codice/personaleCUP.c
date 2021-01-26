#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

static void add_patient(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[6];

	//input
	char tess_sanitaria[46], nome[46], cognome[46], indirizzo [101], luogoN[46];
	MYSQL_TIME dataN;
	char day[3], month[3], year[5];

	// get input
	printf("\nTessera sanitaria: ");
	getInput(46, tess_sanitaria, false);
	printf("\nNome: ");
	getInput(46, nome, false);
	printf("\nCognome: ");
	getInput(46, cognome, false);
	printf("\nIndirizzo: ");
	getInput(101, indirizzo, false);

	printf("\nData di nascita [DD/MM/YYYY]");
	printf("\nGiorno: ");
	getInput(3, day, false);
	printf("\nMese: ");
	getInput(3, month, false);
	printf("\nAnno: ");
	getInput(5, year, false);

	printf("\nLuogo di nascita: ");
	getInput(46, luogoN, false);

	// prepare stmt
	if(!setup_prepared_stmt(&prep_stmt, "call inserisci_paziente(?, ?, ?, ?, ?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init registration stmt\n", false);
	}

	// prepare input params
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = tess_sanitaria;
	param[0].buffer_length = strlen(tess_sanitaria);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = strlen(nome);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = cognome;
	param[2].buffer_length = strlen(cognome);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = indirizzo;
	param[3].buffer_length = strlen(indirizzo);

	param[4].buffer_type = MYSQL_TYPE_DATE;
	param[4].buffer = (char *)&dataN;
	param[4].is_null = 0;
	param[4].buffer_length = 0;

	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = luogoN;
	param[5].buffer_length = strlen(luogoN);

	// set struct MYSQL_TIME params
	dataN.day = atoi(day);
	dataN.month = atoi(month);
	dataN.year = atoi(year);
	dataN.time_type = MYSQL_TIMESTAMP_DATE; //!!!

	// bind params
	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for inserisci_paziente procedure\n", true);
	}

	// run procedure
	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred while creating the new patient");
	} else{
		printf("The new patient has been created successfully\n");
	}

	// closing stmt
	mysql_stmt_close(prep_stmt);
}

void run_as_personaleCUP(MYSQL *conn){

	char options[] = {'1', '2', '3'};
	char op;

	printf("Switching to personaleCUP role...\n");

	if(!parse_config("Users/personaleCUP.json", &conf)){
		fprintf(stderr, "Unable to load 'personaleCUP' configuration\n");
		exit(EXIT_FAILURE);
	}

	//change user on DBMS
	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)){
		fprintf(stderr, "mysql_change_user() failed, Error: %s\n", mysql_error(conn));
		exit(EXIT_FAILURE);
	}

	while(true){
		printf("\033[2J\033[H");
		printf("*** What do you want to do? ***\n\n");
		printf("1) Insert a new patient\n");
		printf("2) Make a medical reservation\n");
		printf("3) Quit\n");

		op = multiChoice("Select an option", options, 3);

		switch(op){
			case '1':
				add_patient(conn);
				break;

			case '2':
				//make_reservation(conn);
				break;

			case '3':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__ );
				abort();
		}

		getchar(); // read the '\n'
	}
}
