const int anPin = 1;
long anVolt, inches, cm;
int last=0;//Create sum variable so it can be averaged
int avgrange=10;//Quantity of values to average (sample size)
int sum=0;
int comingCount = 0;
int leavingCount =0;

void setup() {
	Serial.begin(9600);
}

void loop() {
	for(int i = 0; i < avgrange ; i++)
	{
		anVolt = analogRead(anPin);
		sum += anVolt;
		delay(10);
	}  
	anVolt = sum/avgrange;
	if (anVolt+10 < last){
		leavingCount =0;
		comingCount++;
	}
	if (anVolt-10 > last){
		comingCount=0;
		leavingCount++;
	}
	
	if (comingCount >2){
		Serial.println("Off");
	}
	if (leavingCount >2){
		Serial.println("On");
	}
	last = anVolt;
	
	sum=0;
	delay(200);
}
