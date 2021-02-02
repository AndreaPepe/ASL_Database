#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"


static void registration(MYSQL *conn){

	MYSQL_STMT *prep_stmt;
	MYSQL_BIND param[3];
	char local_options[2] = {'1', '2'}; // possible roles
	char r;

	// input
	char username[46];
	char password[46];
	char var_ruolo[46];

	// get required info
	printf("\nUsername: ");
	getInput(46, username, false);
	printf("Password: ");
	getInput(46, password, true);

	printf("Assign a role. Possible options are:\n");
	printf("\t1) Amministratore\n");
	printf("\t2) PersonaleCUP\n");

	r = multiChoice("Select role", local_options, 2);
	// convert role into enum value
	switch(r){
		case '1':
			strcpy(var_ruolo, "amministratore");
			break;

		case '2':
			strcpy(var_ruolo, "personaleCUP");
			break;

		default:
			fprintf(stderr, "Invalid condition at %s: %d\n", __FILE__, __LINE__);
			abort();
	}

	// prepare stmt
	if(!setup_prepared_stmt(&prep_stmt, "call registration(?, ?, ?)", conn)){
		finish_with_stmt_error(conn, prep_stmt, "Unable to init registration stmt\n", false);
	}

	// prepare input params
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = var_ruolo;
	param[2].buffer_length = strlen(var_ruolo);

	// bind params
	if(mysql_stmt_bind_param(prep_stmt, param) != 0){
		finish_with_stmt_error(conn, prep_stmt, "Could not bind parameters for registration procedure\n", true);
	}

	// run procedure
	if (mysql_stmt_execute(prep_stmt) != 0){
		print_stmt_error(prep_stmt, "An error occurred during registration");
		goto close;
	} else{
		printf("The new user has been created successfully\n");
	}

	// closing stmt
close:
	mysql_stmt_close(prep_stmt);
}



static void crea_parametro(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];

	char name[46];

	printf ("\nName of parameter: ");
	getInput(46, name, false);

	if(!setup_prepared_stmt(&p_stmt, "call crea_parametro(?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'crea_parametro' stmt", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = name;
	param[0].buffer_length = strlen(name);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'crea_parametro' procedure", true);
	}

	//execute
	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'crea_parametro' procedure");
	}else{
		printf("\nParameter created successfully\n");
	}

	mysql_stmt_close(p_stmt);
}



static void crea_tipologia_esame(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[3];

	char description[101];
	char cost[12];
	unsigned int examId;	// output

	float f_cost;

	printf("\nDescription (exam name): " );
	getInput(101, description, false);
	printf("\nCost [€€.**]: ");
	getInput(12, cost, false);


	f_cost = strtof(cost, NULL);

	if(!setup_prepared_stmt(&p_stmt, "call crea_tipologia_esame(?, ?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'crea_tipologia_esame' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = description;
	param[0].buffer_length = strlen(description);

	param[1].buffer_type = MYSQL_TYPE_FLOAT;
	param[1].buffer = &f_cost;
	param[1].buffer_length = sizeof(f_cost);

	param[2].buffer_type = MYSQL_TYPE_LONG; //out
	param[2].buffer = &examId;
	param[2].buffer_length = sizeof(examId);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'crea_tipologia_esame' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'crea_tipologia_esame' procedure ");
		goto exit;
	}

	// get back the examID
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &examId;
	param[0].buffer_length = sizeof(examId);

	// bind result
	if (mysql_stmt_bind_result(p_stmt, param)){
		finish_with_stmt_error(conn, p_stmt, "Could not retrieve output parameter", true);
	}

	// Retrieve output param
	if (mysql_stmt_fetch(p_stmt)){
		finish_with_stmt_error(conn, p_stmt, "Could not buffer results", true);
	}

	printf("New type of exam correctly created with ID: %d\n", examId);

exit:
	mysql_stmt_close(p_stmt);
}


static void aggiungi_parametro_ad_esame(MYSQL *conn){
	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[2];

	int examId;
	char parametro[46], c_examId[12];

	printf("\nExam (ID number): ");
	getInput(12, c_examId, false);
	printf("\nParameter name: ");
	getInput(46, parametro, false);

	examId = atoi(c_examId);

	if(!setup_prepared_stmt(&p_stmt, "call add_parametro_ad_esame(?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'add_parametro_ad_esame' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &examId;
	param[0].buffer_length = sizeof(examId);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = parametro;
	param[1].buffer_length = strlen(parametro);


	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'add_parametro_ad_esame' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'add_parametro_ad_esame' procedure ");
	}else{
		printf("Parameter correctly added\n");
	}

	mysql_stmt_close(p_stmt);
}


static void crea_personale(MYSQL *conn){
	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[6];
	bool is_null = true;


	char local_options[3] = {'1', '2', '3'}; // possible roles
	char r;

	//params
	char cf[46], nome[46], cognome[46], indirizzo[101], tipo[46], associazione[46];

	printf("\nFiscal Code (CF): ");
	getInput(46, cf, false);
	printf("\nName: ");
	getInput(46, nome, false);
	printf("\nSurname: ");
	getInput(46, cognome, false);
	printf("\nAddress: ");
	getInput(46, indirizzo, false);
	printf("\nRole:\n");
	printf("\t1) Medico\n");
	printf("\t2) Primario\n");
	printf("\t3) Volontario\n");

	r = multiChoice("Select role", local_options, 3);
	switch(r){
		case '1':
			strcpy(tipo, "medico");
			break;
		case '2':
			strcpy(tipo, "primario");
			break;
		case '3':
			strcpy(tipo, "volontario");
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	if (r == '3'){
		printf("\nVoluntary association (optional): ");
		getInput(46, associazione, false);
		if (strcmp(associazione, "") != 0){
			is_null = false;
		}
	}

	if(!setup_prepared_stmt(&p_stmt, "call add_personale_anagrafica (?, ?, ?, ?, ?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'add_personale_anagrafica' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = nome;
	param[1].buffer_length = strlen(nome);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = cognome;
	param[2].buffer_length = strlen(cognome);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = indirizzo;
	param[3].buffer_length = strlen(indirizzo);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = tipo;
	param[4].buffer_length = strlen(tipo);

	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;	// possible NULL parameter
	param[5].buffer = associazione;
	param[5].is_null = &(is_null);
	param[5].buffer_length = strlen(associazione);



	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'add_personale_anagrafica' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'add_personale_anagrafica' procedure ");
	}else{
		printf("New staff member correctly created");
	}

	mysql_stmt_close(p_stmt);
}


static void aggiungi_dati_lavorativi(MYSQL *conn){
	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[3];
	//params
	char cf[46], c_ospedale[46], c_reparto[46];
	int ospedale, reparto;

	printf("\nFiscal Code (CF): ");
	getInput(46, cf, false);
	printf("\nHospital (ID): ");
	getInput(46, c_ospedale, false);
	printf("\nDepartment (ID): ");
	getInput(46, c_reparto, false);

	ospedale = atoi(c_ospedale);
	reparto = atoi(c_reparto);

	if(!setup_prepared_stmt(&p_stmt, "call add_personale_lavorativo (?, ?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'add_personale_lavorativo' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);

	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &ospedale;
	param[1].buffer_length = sizeof(ospedale);

	param[2].buffer_type = MYSQL_TYPE_LONG;
	param[2].buffer = &reparto;
	param[2].buffer_length = sizeof(reparto);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'add_personale_lavorativo' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'add_personale_lavorativo' procedure ");
	}else{
		printf("Work data correctly added\n");
	}

	mysql_stmt_close(p_stmt);
}


static void crea_specializzazione(MYSQL *conn){
	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];

	char titolo[101];


	printf("\nSpecialization name: ");
	getInput(101, titolo, false);

	if(!setup_prepared_stmt(&p_stmt, "call aggiungi_tipo_specializzazione (?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'aggiungi_tipo_specializzazione' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = titolo;
	param[0].buffer_length = strlen(titolo);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'aggiungi_tipo_specializzazione' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'aggiungi_tipo_specializzazione' procedure ");
	}else{
		printf("New specialization correctly created\n");
	}

	mysql_stmt_close(p_stmt);
}



static void assegna_specializzazione(MYSQL *conn){
	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[2];

	char cf[46], titolo[101];

	printf("\nFiscal Code (CF): ");
	getInput(46, cf, false);
	printf("\nSpecialization name: ");
	getInput(101, titolo, false);

	if(!setup_prepared_stmt(&p_stmt, "call assegna_specializzazione (?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'assegna_specializzazione' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = cf;
	param[0].buffer_length = strlen(cf);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = titolo;
	param[1].buffer_length = strlen(titolo);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'assegna_specializzazione' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'assegna_specializzazione' procedure ");
	}else{
		printf("Specialization correctly assigned\n");
	}

	mysql_stmt_close(p_stmt);
}




static void crea_ospedale (MYSQL *conn){

		MYSQL_STMT *p_stmt;
		MYSQL_BIND param[4];

		char nome[46], indirizzo[101], primario[46];
		int hospId;

		printf("\nHospital name: ");
		getInput(46, nome, false);
		printf("\nAddress: ");
		getInput(101, indirizzo, false);
		printf("\nHead physician (CF): ");
		getInput(46, primario, false);

		if(!setup_prepared_stmt(&p_stmt, "call inserisci_ospedale (?, ?, ?, ?)", conn)){
			finish_with_stmt_error(conn, p_stmt, "Unable to init 'inserisci_ospedale' statement", false);
		}

		memset(param, 0, sizeof(param));

		param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[0].buffer = nome;
		param[0].buffer_length = strlen(nome);

		param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[1].buffer = indirizzo;
		param[1].buffer_length = strlen(indirizzo);

		param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[2].buffer = primario;
		param[2].buffer_length = strlen(primario);

		param[3].buffer_type = MYSQL_TYPE_LONG;	//out
		param[3].buffer = &hospId;
		param[3].buffer_length = sizeof(hospId);

		if(mysql_stmt_bind_param(p_stmt, param) != 0){
			finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'inserisci_ospedale' procedure", true);
		}

		if(mysql_stmt_execute(p_stmt) != 0){
			print_stmt_error(p_stmt, "An error occurred during 'inserisci_ospedale' procedure ");
			goto exit_stmt;
		}

		//get back the hospId
		memset(param, 0, sizeof(param));
		param[0].buffer_type = MYSQL_TYPE_LONG;
		param[0].buffer = &hospId;
		param[0].buffer_length = sizeof(hospId);

		// binding results
		if(mysql_stmt_bind_result(p_stmt, param)){
			finish_with_stmt_error(conn, p_stmt, "Could not retrieve output parameters", true);
		}

		//retrieve results
		if(mysql_stmt_fetch(p_stmt)){
			finish_with_stmt_error(conn, p_stmt, "Could not retrieve buffer results", true);
		}

		printf("New hospital correctly created with ID: %d\n", hospId);
	exit_stmt:
		mysql_stmt_close(p_stmt);

}


static void crea_laboratorio (MYSQL *conn){

		MYSQL_STMT *p_stmt;
		MYSQL_BIND param[6];

		char c_ospedale[46], c_labId[46], nome[46], c_piano[46], stanza[46], primario[46];
		int hospId, piano, labId;

		printf("\nHospital (ID): ");
		getInput(46, c_ospedale, false);
		printf("\nLaboratory ID: ");
		getInput(46, c_labId, false);
		printf("\nName: ");
		getInput(46, nome, false);
		printf("\nFloor [number]: ");
		getInput(46, c_piano, false);
		printf("\nRoom: ");
		getInput(46, stanza, false);
		printf("\nHead physician (CF): ");
		getInput(46, primario, false);

		hospId = atoi(c_ospedale);
		labId = atoi(c_labId);
		piano = atoi(c_piano);

		if(!setup_prepared_stmt(&p_stmt, "call inserisci_laboratorio (?, ?, ?, ?, ?, ?)", conn)){
			finish_with_stmt_error(conn, p_stmt, "Unable to init 'inserisci_laboratorio' statement", false);
		}

		memset(param, 0, sizeof(param));

		param[0].buffer_type = MYSQL_TYPE_LONG;
		param[0].buffer = &hospId;
		param[0].buffer_length = sizeof(hospId);

		param[1].buffer_type = MYSQL_TYPE_LONG;
		param[1].buffer = &labId;
		param[1].buffer_length = sizeof(labId);

		param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[2].buffer = nome;
		param[2].buffer_length = strlen(nome);

		param[3].buffer_type = MYSQL_TYPE_LONG;
		param[3].buffer = &piano;
		param[3].buffer_length = sizeof(piano);

		param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[4].buffer = stanza;
		param[4].buffer_length = strlen(stanza);

		param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
		param[5].buffer = primario;
		param[5].buffer_length = strlen(primario);



		if(mysql_stmt_bind_param(p_stmt, param) != 0){
			finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'inserisci_laboratorio' procedure", true);
		}

		if(mysql_stmt_execute(p_stmt) != 0){
			print_stmt_error(p_stmt, "An error occurred during 'inserisci_laboratorio' procedure ");
		}else{
			printf("New laboratory correctly created \n");
		}

		mysql_stmt_close(p_stmt);

}


static void crea_reparto(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[4];

	char c_ospedale[46], c_repId[46], nome[46], tel[46] ;
	int hospId, repId;

	printf("\nHospital (ID): ");
	getInput(46, c_ospedale, false);
	printf("\nDepartment ID: ");
	getInput(46, c_repId, false);
	printf("\nName: ");
	getInput(46, nome, false);
	printf("\nPhone number: ");
	getInput(46, tel, false);

	hospId = atoi(c_ospedale);
	repId = atoi(c_repId);

	if(!setup_prepared_stmt(&p_stmt, "call inserisci_reparto (?, ?, ?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'inserisci_reparto' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &hospId;
	param[0].buffer_length = sizeof(hospId);

	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &repId;
	param[1].buffer_length = sizeof(repId);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = nome;
	param[2].buffer_length = strlen(nome);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = tel;
	param[3].buffer_length = strlen(tel);


	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for 'inserisci_reparto' procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during 'inserisci_reparto' procedure ");
	}else{
		printf("New department correctly created\n");
	}

	mysql_stmt_close(p_stmt);
}



static void report_personale(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[2];
	int status;
	char header[512];
	bool first = true;

	char options[] = {'1', '2'};
	char op;
	bool month_report = true;

	char personale[46];
	int num_esami;


	printf("\nStaff member (CF): ");
	getInput(46, personale, false);
	printf("\nWhat report do you want to select?");
	printf("\n\t1) Last month");
	printf("\n\t2) Last year\n");

	op = multiChoice("Select an option.", options, 2);
	switch(op){
		case '1':
			month_report = true;
			break;

		case '2':
			month_report = false;
			break;

		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}


	if(month_report){
		if(!setup_prepared_stmt(&p_stmt, "call report_personale_mese (?, ?)", conn)){
			finish_with_stmt_error(conn, p_stmt, "Unable to init 'report_personale_mese' statement", false);
		}
		sprintf(header, "\nExecuted exams in the last month by staff member with CF: %s\n", personale);
	}else{
		if(!setup_prepared_stmt(&p_stmt, "call report_personale_anno (?, ?)", conn)){
			finish_with_stmt_error(conn, p_stmt, "Unable to init 'report_personale_mese' statement", false);
		}
		sprintf(header, "\nExecuted exams in the last year by staff member with CF: %s\n", personale);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = personale;
	param[0].buffer_length = strlen(personale);

	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &num_esami;
	param[1].buffer_length = sizeof(num_esami);

	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for the procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the procedure ");
		goto close;
	}


	//get back the num_exams
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &num_esami;
	param[0].buffer_length = sizeof(num_esami);


	//MULTIPLE RESULT SETS!!!
		do{

			if(conn->server_status & SERVER_PS_OUT_PARAMS){
				goto next;
			}

			if(first){
				dump_result_set(conn, p_stmt, header);
				first = false;
			}else{
				dump_result_set(conn, p_stmt, "");
			}

		next:
			status = mysql_stmt_next_result(p_stmt);
			if (status > 0){		// > 0 error, 0 keep looking, -1 finished
				finish_with_stmt_error(conn, p_stmt, "Unexpected condition", true);
			}
		}while(status == 0);

close:
	mysql_stmt_close(p_stmt);
}



static void search_strutture_primario(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];
	int status;
	char header[512];
	bool first = true;

	char primario[46];

	printf("\nStaff member (CF): ");
	getInput(46, primario, false);

	if(!setup_prepared_stmt(&p_stmt, "call search_strutture_primario (?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'search_strutture_primario' statement", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = primario;
	param[0].buffer_length = strlen(primario);


	if(mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Unable to bind parameters for the procedure", true);
	}

	if(mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the procedure ");
		goto close;
	}

	//MULTIPLE RESULT SETS!!!
		do{

			if(conn->server_status & SERVER_PS_OUT_PARAMS){
				goto next;
			}

			if(first){
				sprintf(header, "\nHospitals with head physician: %s\n", primario);
				first = false;
			}else{
				sprintf(header, "\nLaboratories with head physician: %s\n", primario);
			}

			dump_result_set(conn, p_stmt, header);
		next:
			status = mysql_stmt_next_result(p_stmt);
			if (status > 0){		// > 0 error, 0 keep looking, -1 finished
				finish_with_stmt_error(conn, p_stmt, "Unexpected condition", true);
			}
		}while(status == 0);

close:
	mysql_stmt_close(p_stmt);
}



//MENU
static void manage_exams(MYSQL *conn){
	char options[] = {'1', '2', '3', '4', '5'};
	char op;

	while(true){	// while back option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** What do you want to do? ***\n\n");
		printf("1) Create new exam type\n");
		printf("2) Create new parameter\n");
		printf("3) Assign parameter to an exam\n");
		printf("4) List available exams\n");
		printf("5) Go back\n");

		op = multiChoice("Select an option", options, 5);
		switch(op){
			case '1':
				crea_tipologia_esame(conn);
				break;
			case '2':
				crea_parametro(conn);
				break;
			case '3':
				aggiungi_parametro_ad_esame(conn);
				break;
			case '4':
				list_exams(conn);
				break;
			case '5':
				printf("\nPress 'Enter' to continue\n");
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}

//MENU
static void manage_staff_members (MYSQL *conn){

	char options[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
	char op;

	while(true){	// while back option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** What do you want to do? ***\n\n");
		printf("1) Create new staff member (personal data)\n");
		printf("2) Add work data to a staff member\n");
		printf("3) Create new specialization\n");
		printf("4) Assign specialization to a primary\n");
		printf("5) Search staff members by hospital\n");
		printf("6) Search staff members by department\n");
		printf("7) Report of executed exams by a staff member\n");
		printf("8) Search hospitals and laboratories by head physician\n");
		printf("9) Go back\n");

		op = multiChoice("Select an option", options, 9);
		switch(op){
			case '1':
				crea_personale(conn);
				break;
			case '2':
				aggiungi_dati_lavorativi(conn);
				break;
			case '3':
				crea_specializzazione(conn);
				break;
			case '4':
				assegna_specializzazione(conn);
				break;
			case '5':
				select_personale_by_hosp(conn);
				break;
			case '6':
				select_personale_by_rep(conn);
				break;
			case '7':
				report_personale(conn);
				break;
			case '8':
				search_strutture_primario(conn);
				break;
			case '9':
				printf("\nPress 'Enter' to continue\n");
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}

//MENU
static void create_medical_structure (MYSQL *conn){

	char options[4] = {'1', '2', '3', '4'};
	char op;

	while(true){	// while back option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** What do you want to do? ***\n\n");
		printf("1) Create new hospital\n");
		printf("2) Create new laboratory\n");
		printf("3) Create new department\n");
		printf("4) Go back\n");

		op = multiChoice("Select an option", options, 4);
		switch(op){
			case '1':
				crea_ospedale(conn);
				break;
			case '2':
				crea_laboratorio(conn);
				break;
			case '3':
				crea_reparto(conn);
				break;
			case '4':
				printf("\nPress 'Enter' to continue\n");
				return;
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}


void run_as_amministratore(MYSQL *conn){

	char options[] = {'1', '2', '3', '4', '5', '6'};
	char op;

	printf ("Switching to administrative role ...\n");

	if(!parse_config("Users/amministratore.json", &conf)){
		fprintf(stderr, "Unable to load 'amministratore' configuration\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)){
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	// showing multiChoice after cleaning screen
	while(true){	// while quit option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** What do you want to do? ***\n\n");
		printf("1) Create new user\n");
		printf("2) Manage exams\n");
		printf("3) Manage staff members\n");
		printf("4) Create new medical structure\n");
		printf("5) List of medical structure\n");
		printf("6) Quit\n");

		op = multiChoice("Select an option", options, 6);
		switch(op){
			case '1':
				registration(conn);
				break;

			case '2':
				manage_exams(conn);
				break;

			case '3':
				manage_staff_members(conn);
				break;

			case '4':
				create_medical_structure(conn);
				break;

			case '5':
				list_medical_structures(conn);
				break;

			case '6':
				return;

			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar(); //cleaning '\n'
	}
}
