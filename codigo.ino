String _ssidName     = "Simulator Wifi";
String _ssidPassword = "";
int _tcpHttpPort     = 80;
String _siteHost  = "https://api.thingspeak.com";
String _siteAPPID = "TYF9SYW93IC6QA00";
String _siteURI	 = "/update?api_key=";
String _siteURI2 = "/channels/1064459/fields/1.json?api_key=9V7DVZTVSUSHKKW5&results=1";

int m1_frente = 8;
int m1_tras = 10;

int m2_frente = 11;
int m2_tras = 12;

int celsius = 0;
int bateria = 0;
int direcao;

void setup()
{
  Serial.begin(115200);
  
  pinMode(m1_frente, OUTPUT);
  pinMode(m1_tras, OUTPUT);
  
  pinMode(m2_frente, OUTPUT);
  pinMode(m2_tras, OUTPUT);
  
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  
  sendCommandTo8266("AT", "OK");
  String loginWiFi = "AT+CWJAP=\"" + _ssidName + "\",\"" + _ssidPassword + "\"";
  sendCommandTo8266(loginWiFi, "OK");
  
  String acessoTCP = "AT+CIPSTART=\"TCP\",\"" + _siteHost + "\"," + String(_tcpHttpPort);
  sendCommandTo8266(acessoTCP, "OK");
}

void loop()
{
  solicitaDado();
  String dir = BuscarDirecao();
  direcao = dir.toInt();
  Serial.println(dir);
  inputs();
  delay(1000);
  lerDados();
  
  _siteURI	 = "/update?api_key=TYF9SYW93IC6QA00&field1=" + numberToString(bateria) + "&field2=" + numberToString(celsius);
  
  delay(50);
  EnviarDados();
  delay(1000);
}

bool sendCommandTo8266(String comando, char * aguardar) {
  bool sucesso = false;
  
  Serial.println(comando);
  Serial.flush();
  delay(50);
 
  if (0 == aguardar[0])		    		sucesso = true;
  else if (Serial.find(aguardar))       sucesso = true;
  else      							digitalWrite(13, HIGH);
    
  return sucesso;
}

bool solicitaDado() {
  bool sucesso = true;
  String httpPacket = "GET " + _siteURI2 + " HTTP/1.1\r\nHost: " + _siteHost + "\r\n\r\n";
  int length = httpPacket.length();
  String tamanhoPacote = "AT+CIPSEND=" + numberToString(length);  
  sucesso &= sendCommandTo8266(tamanhoPacote, ">");
  sucesso &= sendCommandTo8266(httpPacket, "SEND OK\r\n");
  
  return sucesso;
}

bool recebeuResposta() {
  while(!Serial.available()) 
  {
    delay(1);
  }
 
  return Serial.find("\r\n\r\n");
}

String BuscarDirecao(){  
  unsigned int i = 0;
  String outputString = "";

  
  while (!Serial.find("\"entry_id\":"))
  { 
  }
  while (!Serial.find("\"field1\":\""))
  { 
  }
  
  
  while (i<60000) {
    
    if(Serial.available()) {
     
      char c = Serial.read();
     
     
      if((c=='"'))
      {
        break;
      }
      outputString += c;
    }
    i++;
  }
  return outputString;
}

String numberToString(int valor) {
  char numero[6];
  sprintf(numero, "%i", valor);
  return String(numero);
}
void pararMotores(){
    digitalWrite(m1_frente, LOW);
    digitalWrite(m1_tras, LOW);
    digitalWrite(m2_frente, LOW);
    digitalWrite(m2_tras, LOW);
}
void moverFrente(){
  	pararMotores();
    digitalWrite(m1_frente, HIGH);
    digitalWrite(m2_frente, HIGH);
}
void moverTras(){
  	pararMotores();
    digitalWrite(m1_tras, HIGH);
    digitalWrite(m2_tras, HIGH);
}
void moverDireita(){
  	pararMotores();
    digitalWrite(m1_tras, HIGH);
    digitalWrite(m2_frente, HIGH);
}
void moverEsquerda(){
  	pararMotores();
    digitalWrite(m1_frente, HIGH);
    digitalWrite(m2_tras, HIGH);
}

void inputs(){

    switch (direcao)
    {
    case 0:
      	pararMotores();
      	break;
    case 1:
        moverFrente();
        break;
    case 2:
        moverTras();
        break;
    case 3:
        moverEsquerda();
    break;
    case 4:
        moverDireita();
    break;
    }
}
bool EnviarDados() {
  bool sucesso = true;
  String httpPacket = "GET " + _siteURI + " HTTP/1.1\r\nHost: " + _siteHost + "\r\n\r\n";
  int length = httpPacket.length();
  String tamanhoPacote = "AT+CIPSEND=" + numberToString(length);
  Serial.println("\n" + httpPacket + "\n");
  sucesso &= sendCommandTo8266(tamanhoPacote, ">");
  sucesso &= sendCommandTo8266(httpPacket, "SEND OK\r\n");
  
  return sucesso;
}
void lerDados(){
    int t = analogRead(A0);
    celsius = map(t,20, 358,-40,125);
    int b = analogRead(A1);
    bateria = map(b, 0, 1023, 0, 100);
}
