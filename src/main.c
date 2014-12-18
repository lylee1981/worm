/*
 * main.c
 *
 *  Created on: Jul 25, 2014
 *      Author: lingyun
 */

#include "base.h"
#include "master.h"
#include "monitor.h"

int main(int argc, char *argv[]) {
	char *type = argv[1];
	int category = atoi(type);
	if (category == 1) {

		master_start();

	} else if (category ==2) {

		monitor_start();

	} else {

		printf("invalid input type\n");
	}
}
