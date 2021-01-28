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
		finish_with_stmt_error(conn, prep_stmt, "Unable to init 'registration' stmt\n", false);
	}

	// prepare input params
	memset(param, 0, sizeof(param));
	memset(&dataN, 0, sizeof(dataN));

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
	param[4].buffer_length = sizeof(dataN);

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
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for 'inserisci_paziente' procedure\n", true);
	}

	// run procedure
	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred while creating the new patient");
	} else{
		printf("Nuovo paziente creato correttamente\n");
	}

	// closing stmt
	mysql_stmt_close(prep_stmt);
}


static void prenota_esame(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[9];
	MYSQL_TIME date;
	MYSQL_TIME hour;

	//input
	// diagnosi sempre NULL nell'inserimento, si aggiorna in seguito con altra procedure
	bool is_null = true;

	char c_examId[46], paziente[46], diagnosi[256], c_lab[46], c_hosp[46], c_codiceP[46];
	int examId, lab, hosp, codiceP;
	char day[3], month[3], year[5], ore[3], minuti[3];
	int i_ore, i_minuti;
	char urgenza[10];

	// get input
	printf("\nTipologia esame (ID): ");
	getInput(46, c_examId, false);
	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);

	printf("\nData esame [DD/MM/YYYY]");
	printf("\nGiorno: ");
	getInput(3, day, false);
	printf("\nMese: ");
	getInput(3, month, false);
	printf("\nAnno: ");
	getInput(5, year, false);

	printf("\nOrario esame [hh:mm]");
retry:
	printf("\nOre: ");
	getInput(3, ore, false);
	printf("\nMinuti: ");
	getInput(3, minuti, false);
	i_ore = atoi(ore);
	i_minuti = atoi(minuti);
	if(!((i_ore >= 0) && (i_ore <24) && (i_minuti >= 0) && (i_minuti < 60))){
		printf("\nLe ore devono essere comprese tra 0 e 23, i minuti tra 0 e 59.\nRiprova\n");
		goto retry;
	}

	if(yesOrNo("Con urgenza?", 'y', 'n', false, true))
		strcpy(urgenza, "sì");
	else
		strcpy(urgenza, "no");

	printf("\nCodice Prenotazione: ");
	getInput(46, c_codiceP, false);
	printf("\nOspedale (ID): ");
	getInput(46, c_hosp, false);
	printf("\nLaboratorio (ID): ");
	getInput(46, c_lab, false);

	examId = atoi(c_examId);
	codiceP = atoi(c_codiceP);
	lab = atoi(c_lab);
	hosp = atoi(c_hosp);

	// prepare stmt
	if(!setup_prepared_stmt(&prep_stmt, "call prenota_esame(?, ?, ?, ?, ?, ?, ?, ?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init 'prenota_esame' stmt\n", false);
	}

	// prepare input params
	memset(param, 0, sizeof(param));
	memset(&date, 0, sizeof(date));
	memset(&hour, 0, sizeof(hour));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &examId;
	param[0].buffer_length = sizeof(examId);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = paziente;
	param[1].buffer_length = strlen(paziente);

	param[2].buffer_type = MYSQL_TYPE_DATE;
	param[2].buffer = (char *)&date;
	param[2].buffer_length = sizeof(date);

	param[3].buffer_type = MYSQL_TYPE_TIME;
	param[3].buffer = (char *)&hour;
	param[3].buffer_length = sizeof(hour);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = urgenza;
	param[4].buffer_length = strlen(urgenza);

	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = diagnosi;
	param[5].is_null = &is_null;		// always true!
	param[5].buffer_length = strlen(diagnosi);

	param[6].buffer_type = MYSQL_TYPE_LONG;
	param[6].buffer = &codiceP;
	param[6].buffer_length = sizeof(codiceP);

	param[7].buffer_type = MYSQL_TYPE_LONG;
	param[7].buffer = &lab;
	param[7].buffer_length = sizeof(lab);

	param[8].buffer_type = MYSQL_TYPE_LONG;
	param[8].buffer = &hosp;
	param[8].buffer_length = sizeof(examId);


	// set struct MYSQL_TIME for date param
	date.day = atoi(day);
	date.month = atoi(month);
	date.year = atoi(year);
	date.time_type = MYSQL_TIMESTAMP_DATE;

	// set struct MYSQL_TIME for hour param
	hour.hour = i_ore;
	hour.minute = i_minuti;
	hour.time_type = MYSQL_TIMESTAMP_TIME; //!!!

	// bind params
	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for 'prenota_esame' procedure\n", true);
	}

	// run procedure
	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred creating the reservation");
	} else{
		printf("Esame prenotato correttamente\n");
	}

	// closing stmt
	mysql_stmt_close(prep_stmt);
}


static void add_diagnosi(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[4];

	//input
	char c_examId[46], paziente[46], diagnosi[257];
	MYSQL_TIME date;
	char day[3], month[3], year[5];
	int examId;

	// get input
	printf("\nCodice esame: ");
	getInput(46, c_examId, false);
	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);

	printf("\nData esame [DD/MM/YYYY]");
	printf("\nGiorno: ");
	getInput(3, day, false);
	printf("\nMese: ");
	getInput(3, month, false);
	printf("\nAnno: ");
	getInput(5, year, false);
	printf("\n Diagnosi (max 256 caratteri):\n");
	getInput(257, diagnosi, false);

	examId = atoi (c_examId);

	// prepare stmt
	if(!setup_prepared_stmt(&prep_stmt, "call scrivi_diagnosi(?, ?, ?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init 'scrivi_diagnosi' stmt\n", false);
	}

	// prepare input params
	memset(param, 0, sizeof(param));
	memset(&date, 0, sizeof(date));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &examId;
	param[0].buffer_length = sizeof(examId);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = paziente;
	param[1].buffer_length = strlen(paziente);

	param[2].buffer_type = MYSQL_TYPE_DATE;
	param[2].buffer = (char *)&date;
	param[2].buffer_length = sizeof(date);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = diagnosi;
	param[3].buffer_length = strlen(diagnosi);

	// set struct MYSQL_TIME params
	date.day = atoi(day);
	date.month = atoi(month);
	date.year = atoi(year);
	date.time_type = MYSQL_TIMESTAMP_DATE;

	// bind params
	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for 'scrivi_diagnosi' procedure\n", true);
	}

	// run procedure
	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred during the operation");
	} else{
		printf("\nDiagnosi assegnata correttamente\n");
	}

	// closing stmt
	mysql_stmt_close(prep_stmt);
}



static void search_by_codiceP(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];
	int status;
	char header[512];

	char c_codiceP[46];
	int codiceP;

	printf("\nCodice prenotazione: ");
	getInput(46, c_codiceP, false);

	codiceP = atoi(c_codiceP);

	if (!setup_prepared_stmt(&p_stmt, "call search_by_codiceP (?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'search_by_codiceP' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &codiceP;
	param[0].buffer_length = sizeof(codiceP);

	if (mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind params for 'search_by_codiceP' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nEsami con codice di prenotazione %d\n", codiceP);

	do{
		// dump result
		dump_result_set(conn, p_stmt, header);

		status = mysql_stmt_next_result(p_stmt);
		if(status > 0){
			finish_with_stmt_error(conn, p_stmt, "Unexpected condition", true);
		}
	}while(status == 0);

close:
	mysql_stmt_close(p_stmt);
}


void run_as_personaleCUP(MYSQL *conn){

	char options[] = {'1', '2', '3', '4', '5'};
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
		printf("3) Add diagnosis\n");
		printf("4) Search exams by reservation code\n");
		printf("5) Quit\n");

		op = multiChoice("Select an option", options, 5);

		switch(op){
			case '1':
				add_patient(conn);
				break;

			case '2':
				prenota_esame(conn);
				break;

			case '3':
				add_diagnosi(conn);
				break;

			case '4':
				search_by_codiceP(conn);
				break;

			case '5':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__ );
				abort();
		}

		getchar(); // read the '\n'
	}
}
