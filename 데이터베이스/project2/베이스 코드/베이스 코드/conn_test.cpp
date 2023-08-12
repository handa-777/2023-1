#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include<iostream>
#include <fstream>
#include <string>
#include "mysql.h"
using namespace std;

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
//const char* pw = "mysql";
//const char* db = "project";
const char* pw = "hdhO43475!";
const char* db = "project2";

void query1();
void query2();
void query3();
void query4();
void query5();

MYSQL* connection = NULL;
MYSQL conn;
MYSQL_RES* sql_result;
MYSQL_ROW sql_row;

int main(void) {

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	//mysql connect
	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);

	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}

		ifstream fp("20190963.txt");
		if (!fp.is_open()) {
			printf("file open error\n");
			return 1;
		}

		string line;
		char line_char[500];
		
		//한줄씩 읽어서 sql에 저장
		while (!fp.eof()) {
			getline(fp, line);
			strcpy(line_char, line.c_str());
			int state = mysql_query(connection, line_char);
		}

		fp.close();

		int num;

		while (1) {
			printf("\n------- SELECT QUERY TYPES -------\n\n");
			printf("\t1. TYPE I\n");
			printf("\t2. TYPE II\n");
			printf("\t3. TYPE III\n");
			printf("\t4. TYPE IV\n");
			printf("\t5. TYPE V\n");
			printf("\t0. QUIT\n");
			printf("----------------------------------\n");

			cin >> num;
			if (num == 1)
				query1();
			else if (num == 2)
				query2();
			else if (num == 3)
				query3();
			else if (num == 4)
				query4();
			else if (num == 5)
				query5();
			else if (num == 0) {
				printf("Quit\n");
				break;
			}
			else {
				printf("invalid input");
			}
		}
		
		ifstream fp2("20190963_del.txt");
		if (!fp2.is_open()) {
			printf("file open error\n");
			return 1;
		}

		string line2;
		char line_2[500];

		//한줄씩 읽어서 sql에 저장
		while (!fp2.eof()) {
			getline(fp2, line2);
			strcpy(line_2, line2.c_str());
			int state = mysql_query(connection, line_2);
		}

		fp2.close();

		mysql_close(connection);
	}

	return 0;
}

void query1() {
	int sub;
	string ve_id;
	while (1) {
		printf("\n------- subtypes in TYPE I -------\n\n");
		printf("\t1. TYPE I-1.\n");
		printf("\t2. TYPE I-2.\n");
		printf("\t3. TYPE I-3.\n");
		printf("\t0. Quit.\n\n");
		printf("Subtype: ");
		cin >> sub;

		if (sub == 1) {
			printf("\n** Find all customers who had a package on the truck at the time of the crash **\n");
			printf("(Accident Time: 2023-06-04 12:53:49)\n");
			printf("vehicle ID (ve01 ~ ve10): ");
			cin >> ve_id;
			int state = 0;
			string query ="select customer_ID, cus_name from ((shipment join delivery using (vehicle_ID)) join pack_order using(package_ID)) join customer using(customer_ID) where delivery.depart_time < '2023-06-04 12:53:49' and delivery.arrive_time > '2023-06-04 12:53:49' and vehicle_ID = '";
			query += ve_id;
			query += "'";
			char chquery[500];
			strcpy(chquery, query.c_str());
			cout << endl;

			//cout << chquery << endl;
			state = mysql_query(connection, chquery);
			if (state == 0)
			{
				sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
				{
					printf("ID: %s,  name: %s\n", sql_row[0], sql_row[1]); //한줄씩 print
				}
				mysql_free_result(sql_result); //결과 free
			}
			else {
				printf("err\n");
			}
		}
		else if (sub == 2) {
			printf("\n** Find all recipients who had a package on the truck at the time of the crash **\n");
			printf("(Accident Time: 2023-06-04 12:53:49)\n");
			printf("vehicle ID (ve01 ~ ve10): ");
			cin >> ve_id;

			int state = 0;
			string query = "select recipient_ID, rec_name from ((shipment join delivery using (vehicle_ID)) join receive using(package_ID)) join recipient using(recipient_ID) where delivery.depart_time < '2023-06-04 12:53:49' and delivery.arrive_time > '2023-06-04 12:53:49' and vehicle_ID = '";
			query += ve_id;
			query += "'";
			char chquery[500];
			strcpy(chquery, query.c_str());
			cout << endl;

			//cout << chquery << endl;
			state = mysql_query(connection, chquery);
			if (state == 0)
			{
				sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
				{
					printf("ID: %s,  name: %s\n", sql_row[0], sql_row[1]); //한줄씩 print
				}
				mysql_free_result(sql_result); //결과 free
			}
			else {
				printf("err\n");
			}
		}
		else if (sub == 3) {
			printf("\n** Find the last successful delivery by that truck prior to the crash **\n");
			printf("(Accident Time: 2023-06-20 12:53:49)\n");
			printf("vehicle ID (ve01 ~ ve10): ");
			cin >> ve_id;

			int state = 0;
			string query = "select warehouse_ID, destination, depart_time, arrive_time from delivery where arrive_time  < '2023-06-20 12:53:49' and vehicle_ID = '";
			query += ve_id;
			query += "' order by arrive_time desc";
			char chquery[500];
			strcpy(chquery, query.c_str());
			cout << endl;

			//cout << chquery << endl;
			state = mysql_query(connection, chquery);
			if (state == 0)
			{
				sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장

				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
				{
					printf("departure: %s,  destination: %s\ntime of departure: %s,  time of arrive: %s\n", sql_row[0], sql_row[1], sql_row[2], sql_row[3]); //한줄씩 print
					break;
				}
				mysql_free_result(sql_result); //결과 free
			}
			else {
				printf("err\n");
			}
		}
		else if (sub == 0) {
			break;
		}
		else {
			printf("invalid input");
		}
	}
}

void query2() {
	printf("\n---- TYPE2 ----\n\n");
	printf("** Find the customer who has shipped the most packages in certain year **\n");
	printf("Year (2023): ");
	char year[5];
	cin >> year;
	int state = 0;
	string query = "select customer_ID, cus_name from customer join pack_order using (customer_ID) where order_year = ";
	query += year; 
	query += " group by customer_ID order by count(package_ID) desc";
	char chquery[500];
	strcpy(chquery, query.c_str());

	cout << endl;

	//cout << chquery << endl;
	state = mysql_query(connection, chquery);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
		{
			printf("ID: %s,  name: %s\n\n", sql_row[0], sql_row[1]); //한줄씩 print
			break;
		}
		mysql_free_result(sql_result); //결과 free
	}
	else {
		printf("err\n");
	}
}

void query3() {
	printf("\n---- TYPE3 ----\n\n");
	printf("** Find the customer who has spent the most money on shipping in certain year **\n");
	printf("Year (2023): ");
	int state = 0;
	char year[5];
	cin >> year;
	string query = "select customer_ID, cus_name from customer join pack_order using (customer_ID) where order_year = ";
	query += year;
	query += " group by customer_ID order by sum(price) desc";
	char chquery[500];
	strcpy(chquery, query.c_str());
	cout << endl;

	//cout << chquery << endl;
	state = mysql_query(connection, chquery);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
		{
			printf("ID: %s,  name: %s\n\n", sql_row[0], sql_row[1]); //한줄씩 print
			break;
		}
		mysql_free_result(sql_result); //결과 free
	}
	else {
		printf("err\n");
	}
}

void query4() {
	printf("\n---- TYPE4 ----\n\n");
	printf("** Find the packages that were not delivered within the promised time **\n");
	int state = 0;
	string query = "select package_ID from receive where is_in_time = 1";
	char chquery[500];
	strcpy(chquery, query.c_str());
	cout << endl;

	//cout << chquery << endl;
	state = mysql_query(connection, chquery);
	if (state == 0)
	{
		sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
		while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
		{
			printf("Package ID: %s\n", sql_row[0]); //한줄씩 print
		}
		mysql_free_result(sql_result); //결과 free
	}
	else {
		printf("err\n");
	}
	cout << endl;
}

void query5() {
	while (1) {
		char cus_id[5];
		printf("\n---- TYPE5 ----\n");

		printf("\n---- bill type ----\n\n");
		printf("1. Simple Bill\n");
		printf("2. Service Type Bill\n");
		printf("3. Itemize Bill\n");
		printf("0. Quit\n");
		int bill_num;
		printf("\nWhich type? ");
		cin >> bill_num;
		
		if (bill_num == 0) {
			break;
		}
		printf("Your Customer ID (01 ~ 10): ");
		cin >> cus_id;
		printf("Year, Month (2023, 06): ");
		char year[5], month[3];
		cin >> year >> month;
		cout << endl;

		if (bill_num == 1) {
			int state = 0;
			string query = "select cus_name, address, sum(price) from customer join pack_order using (customer_ID) where payment_type = 'monthly' and order_year = ";
			query += year;
			query += " and order_month = ";
			query += month;
			query += " and customer_ID = '";
			query += cus_id;
			query += "'";
			//query += "' group by customer_ID order by sum(price) desc";
			char chquery[500];
			strcpy(chquery, query.c_str());

			//cout << chquery << endl;
			state = mysql_query(connection, chquery);
			if (state == 0)
			{
				sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
				{
					printf("name: %s,  address: (%s),  owed: $%s\n", sql_row[0], sql_row[1], sql_row[2]); //한줄씩 print
					break;
				}
				mysql_free_result(sql_result); //결과 free
			}
			else {
				printf("err\n");
			}
		}
		else if (bill_num == 2) {
			int state = 0;
			string query = "select package_type, sum(price) from (package join pack_order using (package_ID)) join customer using (customer_ID) where order_year = ";
			query += year;
			query += " and order_month = ";
			query += month;
			query += " and customer_ID = '";
			query += cus_id;
			query += "' group by package_type";
			char chquery[500];
			strcpy(chquery, query.c_str());

			//cout << chquery << endl;
			state = mysql_query(connection, chquery);
			if (state == 0)
			{
				sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
				{
					printf("service type: %s,  charges: $%s\n", sql_row[0], sql_row[1]); //한줄씩 print
				}
				mysql_free_result(sql_result); //결과 free
			}
			else {
				printf("err\n");
			}
		}
		else if (bill_num == 3) {
			int state = 0;
			string query = "select content, price from (package join pack_order using (package_ID)) join customer using (customer_ID) where order_year = ";
			query += year;
			query += " and order_month = ";
			query += month;
			query += " and customer_ID = '";
			query += cus_id;
			query += "'";
			char chquery[500];
			strcpy(chquery, query.c_str());

			//cout << chquery << endl;
			state = mysql_query(connection, chquery);
			if (state == 0)
			{
				sql_result = mysql_store_result(connection); //결과 모두 sql_result에 저장
				while ((sql_row = mysql_fetch_row(sql_result)) != NULL) //sql_result를 한줄씩 sql_row에 저장
				{
					printf("Item: %s,  charges: $%s\n", sql_row[0], sql_row[1]); //한줄씩 print
				}
				mysql_free_result(sql_result); //결과 free
			}
			else {
				printf("err\n");
			}
		}
		else {
			printf("invalid number\n");
		}
		cout << endl;
	}
}