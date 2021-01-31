#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

void list_laboratori(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	int status;
	char header[512];

	if (!setup_prepared_stmt(&p_stmt, "call list_laboratori()", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'list_laboratori' stmt\n", false);
	}

	// Nothing to bind!

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nLaboratories\n");

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



void list_ospedali(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	int status;
	char header[512];

	if (!setup_prepared_stmt(&p_stmt, "call list_ospedali()", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'list_ospedali' stmt\n", false);
	}

	// Nothing to bind!

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nOspedali\n");

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




void list_reparti(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];

	int status;
	char header[512];

	char c_ospedale[46];
	int ospedale;

	printf("\nHospital (ID): ");
	getInput(46, c_ospedale, false);

	ospedale = atoi(c_ospedale);

	if (!setup_prepared_stmt(&p_stmt, "call list_reparti (?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'list_reparti' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale;
	param[0].buffer_length = sizeof(ospedale);

	if (mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'list_reparti' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nDepartments of hospital %d\n", ospedale);

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


void select_personale_by_hosp(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[1];
	int status;
	char header[512];

	char c_ospedale[46];
	int ospedale;

	printf("\nHospital (ID): ");
	getInput(46, c_ospedale, false);

	ospedale = atoi(c_ospedale);

	if(!setup_prepared_stmt(&p_stmt, "call select_personale_by_hosp (?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'select_personale_by_hosp' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale;
	param[0].buffer_length = sizeof(ospedale);

	if (mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'select_personale_by_hosp' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nStaff members working in hospital %d\n", ospedale);

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


void select_personale_by_rep(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	MYSQL_BIND param[2];
	int status;
	char header[512];

	char c_ospedale[46], c_reparto[46];
	int ospedale, reparto;

	printf("\nHospital (ID): ");
	getInput(46, c_ospedale, false);
	printf("\nDepartment (ID): ");
	getInput(46, c_reparto, false);

	ospedale = atoi(c_ospedale);
	reparto = atoi(c_reparto);

	if(!setup_prepared_stmt(&p_stmt, "call select_personale_by_rep (?, ?)", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'select_personale_by_rep' stmt\n", false);
	}

	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &ospedale;
	param[0].buffer_length = sizeof(ospedale);

	param[1].buffer_type = MYSQL_TYPE_LONG;
	param[1].buffer = &reparto;
	param[1].buffer_length = sizeof(reparto);

	if (mysql_stmt_bind_param(p_stmt, param) != 0){
		finish_with_stmt_error(conn, p_stmt, "Could not bind parameters for 'select_personale_by_rep' procedure\n", true);
	}

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nStaff members working in hospital %d, department %d\n", ospedale, reparto);

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



void list_exams(MYSQL *conn){

	MYSQL_STMT *p_stmt;
	int status;
	char header[512];

	if (!setup_prepared_stmt(&p_stmt, "call list_esami_disponibili()", conn)){
		finish_with_stmt_error(conn, p_stmt, "Unable to init 'list_esami_disponibili' stmt\n", false);
	}

	// Nothing to bind!

	if (mysql_stmt_execute(p_stmt) != 0){
		print_stmt_error(p_stmt, "An error occurred during the execution of the query");
		goto close;
	}

	sprintf (header, "\nEsami disponibili\n");

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


// MENU
void list_medical_structures (MYSQL *conn){

	char options[4] = {'1', '2', '3', '4'};
	char op;

	while(true){	// while back option is selected
		printf("\033[2J\033[H");	// clean screen
		printf("*** What do you want to list? ***\n\n");
		printf("1) Hospitals\n");
		printf("2) Laboratories\n");
		printf("3) Departments\n");
		printf("4) Go back\n");

		op = multiChoice("Select an option", options, 4);
		switch(op){
			case '1':
				list_ospedali(conn);
				break;
			case '2':
				list_laboratori(conn);
				break;
			case '3':
				list_reparti(conn);
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
