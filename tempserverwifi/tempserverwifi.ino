#include<ESP8266WiFi.h> //Biblioteca para funcionamento do WiFi do ESP
#include<ESP8266WebServer.h> //Biblioteca para o ESP funcionar como servidor
#include <DHT.h> //Biblioteca para funcionamento do sensor de temperatura e umidade DHT11
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x3F for a 20 chars and 4 line display

const char* ssid = "Miyakoda";  // Rede WiFi
const char* password = "nicole12";  //Senha da Rede WiFi
//DEFINIÇÃO DE IP FIXO PARA O NODEMCU
IPAddress ip(192,168,0,91); //COLOQUE UMA FAIXA DE IP DISPONÍVEL 
IPAddress gateway(192,168,0,1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255,255,255,0); //MASCARA DE REDE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 


ESP8266WebServer server(80); //server na porta 80

#define DHTPIN 2 //Pino digital D4 (GPIO2) conectado ao DHT11
#define DHTTYPE DHT11 //Tipo do sensor DHT11

DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)

float temperatura; //variável para armazenar a temperatura
float umidade; //Variável para armazenar a umidade

void setup() 
{
  lcd.begin();                      // initialize the lcd   
  lcd.backlight();
  
  Serial.begin(115200); //Inicializa a comunicação serial
  delay(50); // ?Intervalo para aguardar a estabilização do sistema
  dht.begin(); //Inicializa o sensor DHT11

  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  lcd.setCursor(0,0);
  lcd.print("CONECTANDO WIFI");
  Serial.println(ssid); //Imprime na serial o nome da Rede Wi-Fi
  lcd.setCursor(0,1);
  lcd.print(ssid);

  WiFi.begin(ssid, password); //Inicialização da comunicação Wi-Fi
  WiFi.config(ip, gateway, subnet); //PASSA OS PARÂMETROS
  

  //Verificação da conexão
  while (WiFi.status() != WL_CONNECTED) { //Enquanto estiver aguardando status da conexão
    delay(1000);
    Serial.print("."); //Imprime pontos
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); //Função para exibir o IP da ESP
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(ssid);  
  lcd.setCursor(0,1);//cursor 0 linha 1
  lcd.print(WiFi.localIP());

  server.on("/", handle_OnConnect); //Servidor recebe uma solicitação HTTP - chama a função handle_OnConnect
  server.onNotFound(handle_NotFound); //Servidor recebe uma solicitação HTTP não especificada - chama a função handle_NotFound

  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");

}

void loop()
{
  server.handleClient(); //Chama o método handleClient()
  temperatura = dht.readTemperature();  //Realiza a leitura da temperatura
  umidade = dht.readHumidity(); //Realiza a leitura da umidade
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);//cursor linha 0
  lcd.print("Temp:"); 
  lcd.setCursor(5,0);//cursor linha 0
  lcd.print(temperatura); 
  lcd.setCursor(11,0);//cursor linha 0
  lcd.print("C");
  lcd.setCursor(1,1);//cursor 0 linha 1
  lcd.print(WiFi.localIP());
}

void handle_OnConnect()
{
  temperatura = dht.readTemperature();  //Realiza a leitura da temperatura
  umidade = dht.readHumidity(); //Realiza a leitura da umidade
  Serial.print("Temperatura: ");
  Serial.print(temperatura); //Imprime no monitor serial o valor da temperatura lida
  Serial.println(" ºC");
  Serial.print("Umidade: ");
  Serial.print(umidade); //Imprime no monitor serial o valor da umidade lida
  Serial.println(" %");
  server.send(200, "text/html", EnvioHTML(temperatura, umidade)); //Envia as informações usando o código 200, especifica o conteúdo como "text/html" e chama a função EnvioHTML

}

void handle_NotFound() { //Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado"); //Envia o código 404, especifica o conteúdo como "text/pain" e envia a mensagem "Não encontrado"

}

String EnvioHTML(float Temperaturastat, float Umidadestat) 
{ //Exibindo a página da web em HTML
  String ptr = "<!DOCTYPE html> <html>\n"; //Indica o envio do código HTML
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"; //Torna a página da Web responsiva em qualquer navegador Web
  ptr += "<meta http-equiv='refresh' content='2'>";//Atualizar browser a cada 2 segundos
  ptr += "<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr += "<title>Monitor de Temperatura e Umidade</title>\n"; //Define o título da página

  //Configurações de fonte do título e do corpo do texto da página web
  ptr += "<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #000000;}\n";
  ptr += "body{margin-top: 50px;}\n";
  ptr += "h1 {margin: 50px auto 30px;}\n";
  ptr += "h2 {margin: 40px auto 20px;}\n";
  ptr += "p {font-size: 24px;color: #000000;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Monitor de Temperatura e Umidade</h1>\n";
  ptr += "<h2>NODEMCU ESP8266 Web Server</h2>\n";

  //Exibe as informações de temperatura e umidade na página web
  ptr += "<p><b>Temperatura: </b>";
  ptr += (float)Temperaturastat;
  ptr += " Graus Celsius</p>";
  ptr += "<p><b>Umidade: </b>";
  ptr += (float)Umidadestat;
  ptr += " %</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;

}
