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
//
//Pin defnition
int led3 = 3;
int led5 = 5;
int led6 = 6;
int sysled = 9;
//network properties
static byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static byte ip[] = { 192, 168, 0, 64 };
static byte gateway[] = { 192, 168, 0, 1 };
static byte subnet[] = { 255, 255, 255, 0 };

// RFC1213-MIB OIDs

//
// Arduino defined OIDs
// .iso.org.dod.internet.private (.1.3.6.1.4)
// .iso.org.dod.internet.private.enterprises (.1.3.6.1.4.1)
// .iso.org.dod.internet.private.enterprises.arduino (.1.3.6.1.4.1.36582)
//

//definite a OID for our controler item
static char sysLight[] PROGMEM = "1.3.6.1.4.1.36582.1.0";
static char ledLight3[] PROGMEM = "1.3.6.1.4.1.36582.1.3";
static char ledLight5[] PROGMEM = "1.3.6.1.4.1.36582.1.5";
static char ledLight6[] PROGMEM = "1.3.6.1.4.1.36582.1.6";
static char sysIpAddr[] PROGMEM = "1.3.6.1.2.1.4.20.1.1";
//
// RFC1213 local values

static int16_t sysLightOn = 2;
static int16_t ledLightOn3 = 2;
static int16_t ledLightOn5 = 2;
static int16_t ledLightOn6 = 2;

//SNMP resvered variables
char oid[SNMP_MAX_OID_LEN];
SNMP_API_STAT_CODES api_status;
SNMP_ERR_CODES status;

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
				int16_t* tempvalue = &sysLightOn;
				status = pdu.VALUE.decode(tempvalue);

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			} else {
				// response packet from get-request - locDescr
				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, sysLightOn);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
			//
		} else if (strcmp_P(oid, sysIpAddr) == 0) {

			// handle sysDescr (set/get) requests
			if (pdu.type == SNMP_PDU_SET) {
				// response packet from set-request - object is read-only
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = SNMP_ERR_READ_ONLY;
			} else {
				// response packet from get-request - locDescr
				status = pdu.VALUE.encode(SNMP_SYNTAX_IP_ADDRESS, ip);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
		} else if (strcmp_P(oid, ledLight3) == 0) {
			// handle sysLight (set/get) requests
			if (pdu.type == SNMP_PDU_SET) {
				//make a temp pointer to parse pdu value
				int16_t* tempvalue = &ledLightOn3;
				status = pdu.VALUE.decode(tempvalue);

				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			} else {
				// response packet from get-request - locDescr
				status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ledLightOn3);
				pdu.type = SNMP_PDU_RESPONSE;
				pdu.error = status;
			}
		}

		else if (strcmp_P(oid, ledLight5) == 0) {
					// handle sysLight (set/get) requests
					if (pdu.type == SNMP_PDU_SET) {
						//make a temp pointer to parse pdu value
						int16_t* tempvalue = &ledLightOn5;
						status = pdu.VALUE.decode(tempvalue);

						pdu.type = SNMP_PDU_RESPONSE;
						pdu.error = status;
					} else {
						// response packet from get-request - locDescr
						status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ledLightOn5);
						pdu.type = SNMP_PDU_RESPONSE;
						pdu.error = status;
					}
		}

		else if (strcmp_P(oid, ledLight6) == 0) {
							// handle sysLight (set/get) requests
							if (pdu.type == SNMP_PDU_SET) {
								//make a temp pointer to parse pdu value
								int16_t* tempvalue = &ledLightOn6;
								status = pdu.VALUE.decode(tempvalue);

								pdu.type = SNMP_PDU_RESPONSE;
								pdu.error = status;
							} else {
								// response packet from get-request - locDescr
								status = pdu.VALUE.encode(SNMP_SYNTAX_INT, ledLightOn6);
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
	pinMode(sysled, OUTPUT);
	pinMode(led3, OUTPUT);
	pinMode(led5, OUTPUT);
	pinMode(led6, OUTPUT);

	Serial.begin(9600);
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

	if (sysLightOn == 1) {
		digitalWrite(sysled, HIGH);
	} else if (sysLightOn == 2) {
		digitalWrite(sysled, LOW);

	}

	if (ledLightOn3 == 1) {
		digitalWrite(led3, HIGH);
	} else if (ledLightOn3 == 2) {
		digitalWrite(led3, LOW);

	}

	if (ledLightOn5 == 1) {
		digitalWrite(led5, HIGH);
	} else if (ledLightOn5 == 2) {
		digitalWrite(led5, LOW);

	}

	if (ledLightOn6 == 1) {
		digitalWrite(led6, HIGH);
	} else if (ledLightOn6 == 2) {
		digitalWrite(led6, LOW);

	}


}

