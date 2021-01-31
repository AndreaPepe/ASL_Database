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
		strcpy(urgenza, "si");
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
	printf("\nDiagnosi (max 256 caratteri):\n");
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



static void add_email(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[2];

	char email[46], paziente[46];

	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);
	printf("\nEmail: ");
	getInput(46, email, false);

	if(!setup_prepared_stmt(&prep_stmt, "call aggiungi_email(?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init 'aggiungi_email' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = email;
	param[0].buffer_length = strlen(email);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = paziente;
	param[1].buffer_length = strlen(paziente);

	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for 'aggiungi_email' procedure\n", true);
	}

	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred during the operation");
	} else{
		printf("\nEmail assegnata correttamente\n");
	}

	mysql_stmt_close(prep_stmt);
}


static void add_telefono(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[3];

	char options[] = {'1', '2'};
	char op;

	char numero[46], paziente[46], tipo[46];

	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);
	printf("\nTipologia ");
	printf("\n\t1) Cellulare");
	printf("\n\t2) Fisso\n");

	op = multiChoice("Seleziona tipologia di numero di telefono", options, 2);
	switch(op){
		case '1':
			strcpy(tipo, "cellulare");
			break;
		case '2':
			strcpy(tipo, "fisso");
			break;
		default:
			fprintf(stderr, "Invalid condition at %s: %d\n", __FILE__, __LINE__);
			abort();
	}

	printf("\nNumero di telefono: ");
	getInput(46, numero, false);

	if(!setup_prepared_stmt(&prep_stmt, "call aggiungi_recapito_tel(?, ?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init 'aggiungi_recapito_tel' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = numero;
	param[0].buffer_length = strlen(numero);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = tipo;
	param[1].buffer_length = strlen(tipo);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = paziente;
	param[2].buffer_length = strlen(paziente);


	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for 'aggiungi_recapito_tel' procedure\n", true);
	}

	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred during the operation");
	} else{
		printf("\nNumero di telefono assegnato correttamente\n");
	}

	mysql_stmt_close(prep_stmt);
}




static void add_personale_ad_esame(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[4];
	MYSQL_TIME date;

	char c_esame[46], paziente[46], day[3], month[3], year[5], personale[46];
	int examId;

	printf("\nEsame (ID): ");
	getInput(46, c_esame, false);
	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);

	printf("\nData esame [DD/MM/YYYY]");
	printf("\nGiorno: ");
	getInput(3, day, false);
	printf("\nMese: ");
	getInput(3, month, false);
	printf("\nAnno: ");
	getInput(5, year, false);

	printf("\nMembro del personale da assegnare all'esame (CF): ");
	getInput(46, personale, false);

	examId = atoi(c_esame);


	if(!setup_prepared_stmt(&p_stmt, "call assegna_personale_ad_esame(?, ?, ?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'assegna_personale_ad_esame' stmt\n", false);
	}

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
	param[3].buffer = personale;
	param[3].buffer_length = strlen(personale);

	// set struct MYSQL_TIME for date param
	date.day = atoi(day);
	date.month = atoi(month);
	date.year = atoi(year);
	date.time_type = MYSQL_TIMESTAMP_DATE;


	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'assegna_personale_ad_esame' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the operation");
	} else{
		printf("\nMembro del personale assegnato correttamente\n");
	}

	mysql_stmt_close(p_stmt);
}







static void search_by_tess_sanitaria(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];
	int status;
	char header[512];

	char paziente[46];

	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);

	if(!setup_prepared_stmt(&p_stmt, "call search_patient(?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'search_patient' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = paziente;
	param[0].buffer_length = strlen(paziente);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'search_patient' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the operation");
		goto close;
	}

	sprintf (header, "\nPaziente con tessera sanitaria %s\n", paziente);

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


void list_patients(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	int status;
	char header[512];

	if (!setup_prepared_stmt(&p_stmt, "call list_patients()", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'list_patients' stmt\n", false);
	}

	// Nothing to bind!

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nPazienti registrati\n");

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



static void search_recapiti(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];
	int status;
	char header[512];

	char paziente[46];

	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);

	if(!setup_prepared_stmt(&p_stmt, "call search_recapiti(?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'search_recapiti' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = paziente;
	param[0].buffer_length = strlen(paziente);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'search_recapiti' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the operation");
		goto close;
	}

	sprintf (header, "\nRecapiti del paziente con tessera sanitaria %s\n", paziente);

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



static void report_paziente(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];
	int status;
	char header[512];
	bool first = true;

	char paziente[46];

	printf("\nPaziente (tessera sanitaria): ");
	getInput(46, paziente, false);

	if (!setup_prepared_stmt(&p_stmt, "call report_paziente (?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to initialize 'report_paziente' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = paziente;
	param[0].buffer_length = strlen(paziente);

	if (mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'report_paziente' stmt\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred while retrieving output.");
		goto exit;
	}

//MULTIPLE RESULT SETS!!!
	do{

		if(conn->server_status & SERVER_PS_OUT_PARAMS){
			goto next;
		}

		if(first){
			sprintf(header, "\nStorico esami eseguiti dal paziente con tessera sanitaria %s\n", paziente);
			first = false;
		}else{
			sprintf(header, "\nPrenotazioni attive per il paziente con tessera sanitaria %s\n", paziente);
		}

		dump_result_set(conn, p_stmt, header);

	next:
		status = mysql_stmt_next_result(p_stmt);
		if (status > 0){		// > 0 error, 0 keep looking, -1 finished
			finish_with_stmt_error(conn, p_stmt, "Unexpected condition", true);
		}
	}while(status == 0);

exit:
	mysql_stmt_close(p_stmt);
}


//MENU
static void manage_patients(MYSQL *conn){

	char options[] = {'1', '2', '3', '4', '5', '6', '7', '8'};
	char op;

	while(true){
		printf("\033[2J\033[H");
		printf("*** Cosa desideri fare? ***\n\n");
		printf("1) Aggiungi un nuovo paziente\n");
		printf("2) Aggiungi un'email di un paziente\n");
		printf("3) Aggiungi un numero di telefono di un paziente\n");
		printf("4) Cerca paziente\n");
		printf("5) Lista tutti i pazienti\n");
		printf("6) Cerca i recapiti di un paziente\n");
		printf("7) Report paziente (storico esami e prenotazioni)\n");
		printf("8) Indietro\n");

		op = multiChoice("Select an option", options, 8);

		switch(op){
			case '1':
				add_patient(conn);
				break;

			case '2':
				add_email(conn);
				break;

			case '3':
				add_telefono(conn);
				break;

			case '4':
				search_by_tess_sanitaria(conn);
				break;

			case '5':
				list_patients(conn);
				break;

			case '6':
				search_recapiti(conn);
				break;

			case '7':
				report_paziente(conn);
				break;

			case '8':
				printf("\nPress 'Enter' to continue\n");
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__ );
				abort();
		}

		getchar(); // read the '\n'
	}
}



//MENU
static void manage_exams(MYSQL *conn){

	char options[] = {'1', '2', '3', '4', '5', '6'};
	char op;

	while(true){
		printf("\033[2J\033[H");
		printf("*** Cosa desideri fare? ***\n\n");
		printf("1) Prenota un esame\n");
		printf("2) Aggiungi diagnosi\n");
		printf("3) Cerca esame per codice di prenotazione\n");
		printf("4) Visualizza gli esami disponibili\n");
		printf("5) Assegna un membro del personale ad un esame\n");
		printf("6) Indietro\n");

		op = multiChoice("Select an option", options, 6);

		switch(op){
			case '1':
				prenota_esame(conn);
				break;

			case '2':
				add_diagnosi(conn);
				break;

			case '3':
				search_by_codiceP(conn);
				break;

			case '4':
				list_exams(conn);
				break;

			case '5':
				add_personale_ad_esame (conn);
				break;

			case '6':
				printf("\nPress 'Enter' to continue\n");
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__ );
				abort();
		}

		getchar(); // read the '\n'
	}
}



//MENU
static void search_staff_members (MYSQL *conn){

	char options[3] = {'1', '2', '3'};
	char op;

	while(true){	// while back option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** Cosa desideri fare? ***\n\n");
		printf("1) Cerca membri del personale per ospedale\n");
		printf("2) Cerca membri del personale per reparto\n");
		printf("3) Indietro\n");

		op = multiChoice("Select an option", options, 7);
		switch(op){
			case '1':
				select_personale_by_hosp(conn);
				break;
			case '2':
				select_personale_by_rep(conn);
				break;
			case '3':
				printf("\nPress 'Enter' to continue\n");
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
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
		printf("*** Cosa desideri fare? ***\n\n");
		printf("1) Gestisci pazienti\n");
		printf("2) Gestisci esami\n");
		printf("3) Visualizza le strutture mediche\n");
		printf("4) Cerca membri del personale\n");
		printf("5) Esci\n");

		op = multiChoice("Select an option", options, 5);

		switch(op){
			case '1':
				manage_patients(conn);
				break;

			case '2':
				manage_exams(conn);
				break;

			case '3':
				list_medical_structures(conn);
				break;

			case '4':
				search_staff_members(conn);
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
