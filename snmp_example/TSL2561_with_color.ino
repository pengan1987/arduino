/**
 USE_GITHUB_USERNAME=pengan1987
 * Agentuino SNMP Agent Library Prototyping...
 *
 * Original made 2010 Eric C. Gionet <lavco_eg@hotmail.com>
 * Modified by Andy Zhou
 */
#include <Streaming.h>         // Include the Streaming library
#include <Ethernet.h>          // Include the Ethernet library
#include <SPI.h>
#include <MemoryFree.h>
#include <Agentuino.h>
#include <Flash.h>
#include <TSL2561.h>
#include <Wire.h>
//

//network properties
static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 0, 0, 0, 0 };
//static byte gateway[] = { 192, 168, 0, 1 };
//static byte subnet[] = { 255, 255, 255, 0 };

//definite a OID for our controler item
static char sysLight[] PROGMEM = "1.3.6.1.4.1.36582.1.0";
static char sysLux2561[] PROGMEM = "1.3.6.1.4.1.36582.2.0";
static char sysIpAddr[] PROGMEM = "1.3.6.1.2.1.4.20.1.1";
static char ledLight3[] PROGMEM = "1.3.6.1.4.1.36582.1.3";
static char ledLight5[] PROGMEM = "1.3.6.1.4.1.36582.1.5";
static char ledLight6[] PROGMEM = "1.3.6.1.4.1.36582.1.6";

//
// RFC1213 local values

static int16_t ledLevel3 = 25;
static int16_t ledLevel5 = 25;
static int16_t ledLevel6 = 25;

static int16_t locLight = 2;
int16_t* tempvalue;
//SNMP resvered variables
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;

//Hardware Variables
TSL2561 tsl(TSL2561_ADDR_FLOAT);
int getLux();

void pduReceived() {
	SNMP_PDU pdu;

	//
	api_status = Agentuino.requestPdu(&pdu);
	//
	if (pdu.type == SNMP_PDU_GET || pdu.type == SNMP_PDU_GET_NEXT
			|| pdu.type == SNMP_PDU_SET && pdu.error == SNMP_ERR_NO_ERROR
					&& api_status == SNMP_API_STAT_SUCCESS) {
		//
		pdu.OID.toString(oid);
		//
		//Serial << "OID: " << oid << endl;
		//
		if (strcmp_P(oid, sysLight) == 0) {
			// handle sysLight (set/get) requests
			if (pdu.type == SNMP_PDU_SET) {
				//make a temp pointer to parse pdu value
				tempvalue = &locLight;
				status = pdu.VALUE.decode(tempvalue);

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			} else {

				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, locLight);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}

		} else if (strcmp_P(oid, ledLight3) == 0) {
			// handle sysLight (set/get) requests
			if (pdu.type == SNMP_PDU_SET) {
				//make a temp pointer to parse pdu value
				tempvalue = &ledLevel3;
				status = pdu.VALUE.decode(tempvalue);

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			} else {

				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ledLevel3);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}

		} else if (strcmp_P(oid, ledLight5) == 0) {
			// handle sysLight (set/get) requests
			if (pdu.type == SNMP_PDU_SET) {
				//make a temp pointer to parse pdu value
				tempvalue = &ledLevel5;
				status = pdu.VALUE.decode(tempvalue);

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			} else {

				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ledLevel5);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}

		} else if (strcmp_P(oid, ledLight6) == 0) {
			// handle sysLight (set/get) requests
			if (pdu.type == SNMP_PDU_SET) {
				//make a temp pointer to parse pdu value
				tempvalue = &ledLevel6;
				status = pdu.VALUE.decode(tempvalue);

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			} else {

				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ledLevel6);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}

		} else if (strcmp_P(oid, sysIpAddr) == 0) {

			if (pdu.type == SNMP_PDU_SET) {

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = SNMP_ERR_READ_ONLY;
			} else {

				status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
		} else if (strcmp_P(oid, sysLux2561) == 0) {

			if (pdu.type == SNMP_PDU_SET) {

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = SNMP_ERR_READ_ONLY;
			} else {
				//get the lux value from sensor
				int lux2561;
				lux2561 = getLux();
				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, lux2561);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
		}

		else {
			// oid does not exist
			//
			// response packet - object not found
			pdu.type = SNMP_PDU_RESPONSE;
			pdu.error = SNMP_ERR_NO_SUCH_NAME;
		}
		//
		Agentuino.responsePdu(&pdu);
	}
	//
	Agentuino.freePdu(&pdu);
	//
	//Serial << "UDP Packet Received End.." << " RAM:" << freeMemory() << endl;
}

void setup() {
	pinMode(9, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	Serial.begin(9600);

	//hardware initialize

	if (tsl.begin()) {
		Serial.println("Found sensor");
	} else {
		Serial.println("No sensor?");
		while (1)
			;
	}
	tsl.setGain(TSL2561_GAIN_16X);
	tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);

	//Start Ethernet
	Ethernet.begin(mac);
	//print local ip address on console
	Serial.print("My IP address: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print(".");
		//set local ip variable to real ip address
		ip[thisByte] = Ethernet.localIP()[thisByte];
	}
	Serial.println();

	//
	api_status = Agentuino.begin();
	//
	if (api_status == SNMP_API_STAT_SUCCESS) {
		//
		Agentuino.onPduReceive(pduReceived);
		//
		delay(10);
		//
		Serial << F("SNMP Agent Initalized...") << endl;
		//
		return;
	}
	//
	delay(10);
	//
	Serial << F("SNMP Agent Initalization Problem...") << status << endl;
}

void loop() {
	// listen/handle for incoming SNMP requests
	Agentuino.listen();
	//
	//

	if (locLight == 1) {
		digitalWrite(9, HIGH);
	} else if (locLight == 2) {
		digitalWrite(9, LOW);

	}

	if (ledLevel3 >= 0 && ledLevel3 <= 255) {
		analogWrite(3, ledLevel3);
	}

	if (ledLevel5 >= 0 && ledLevel5 <= 255) {
		analogWrite(5, ledLevel5);
	}

	if (ledLevel6 >= 0 && ledLevel6 <= 255) {
		analogWrite(6, ledLevel6);

	}
}

int getLux() {
	uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);
	//uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
	//uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);

	Serial.println(x, DEC);

	// More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
	// That way you can do whatever math and comparisons you want!
	uint32_t lum;
	uint16_t ir, full;
	lum = tsl.getFullLuminosity();

	ir = lum >> 16;
	full = lum & 0xFFFF;
	Serial.print("IR: ");
	Serial.print(ir);
	Serial.print("\t\t");
	Serial.print("Full: ");
	Serial.print(full);
	Serial.print("\t");
	Serial.print("Visible: ");
	Serial.print(full - ir);
	Serial.print("\t");

	Serial.print("Lux: ");
	int lux = tsl.calculateLux(full, ir);
	Serial.println(lux);
	return lux;
}
