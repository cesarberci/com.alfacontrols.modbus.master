//============================================================================
// Name        : System.cpp
// Author      : Cesar Daltoe Berci
// Version     :
// Copyright   : TekSea 2019
// Description : IEC 61850 Server
//============================================================================

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <math.h>
#include <iostream>
#include "version_num.h"
/* MODBUS LIB */
#include "modbus-rtu.h"
#include <modbus.h>

using namespace std;

int main(int argc, char*argv[]) {
	if (argc < 2) {
		cout << "Version: " << completeVersion << endl << endl << endl;
		cout << info << endl;
		return -1;
	}
	string arg;
	bool range = false;
	int addr = atoi(argv[2]);
	int speed = atoi(argv[3]);
	uint8_t f_code = atoi(argv[4]);
	int firstAddress = atoi(argv[5]);
	int addressRange = atoi(argv[6]);
	int timeInterval = 1;



	cout << " --== Configuration ==-- " << endl;
	cout << "Slave address: " << ((range) ? "auto range" : "  ") << addr
			<< endl;
	cout << "Baud Rate: " << speed << endl;
	cout << "First register to read: " << firstAddress << endl;
	cout << "Number of address to read: " << addressRange << endl;
	cout << "Period: " << timeInterval << endl;
	cout << "starting program..." << endl << endl << endl << endl;

	/* ModBus configuration */
	uint16_t tab_reg[addressRange];
	uint8_t tab_bin[addressRange];
	int rc;
	struct timeval timeout;
	modbus_t* ctx = modbus_new_rtu(argv[1], speed, 'N', 8, 1);
	modbus_set_debug(ctx, true);
	modbus_connect(ctx);
	modbus_flush(ctx);
	rc = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
	printf("modbus_rtu_set_serial_mode: %d \n", rc);
	if (rc != 0) {
		printf("modbus_rtu_set_serial_mode: %s \n", modbus_strerror(errno));
		cout<<"Trying RS233 mode"<<endl;
		rc = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
		if (rc != 0) {
			printf("modbus_rtu_set_serial_mode: %s \n", modbus_strerror(errno));
			cout<<"exiting program..."<<endl;
		}
	}
	rc = modbus_set_slave(ctx, 1);
	printf("modbus_set_slave return: %d\n", rc);
	if (rc != 0) {
		printf("modbus_set_slave: %s \n", modbus_strerror(errno));
	}
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	modbus_set_response_timeout(ctx, &timeout);
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000;
	modbus_set_byte_timeout(ctx, &timeout);
	/* END OF MODBUS CONFIGURATION */

	rc = modbus_set_slave(ctx, addr);
	while (true) {
		cout << " *** TRYING TO READ ADDRESS: " << addr << endl;
		switch (f_code){
		case 1: //read coils
			rc = modbus_read_bits(ctx, firstAddress, addressRange, tab_bin);
			break;
		case 2: //read discrete
			rc = modbus_read_input_bits(ctx, firstAddress, addressRange, tab_bin);
			break;
		case 3: //read hr
			rc = modbus_read_registers(ctx, firstAddress, addressRange, tab_reg);
			break;
		case 4: //read ir
			rc = modbus_read_input_registers(ctx, firstAddress, addressRange, tab_reg);
			break;
		}

		sleep(timeInterval);
	}

	/* MODBUS STOP */
	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
/* End of File */
