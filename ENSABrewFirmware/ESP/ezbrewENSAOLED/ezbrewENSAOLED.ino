//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- EZBrewControl
//-------------------------------------------------------------------------------------------
// Arquivo  : ezbrewENSAv2.1.ino
// Inicio   : 29/04/2013 10:23
// Revisão  : 11/05/2016 18:31
//
// AutorI   : Cláudio B. Pimenta, A.S
// AutorII  : Jânio Anselmo, Eng. Me.
// AutorIII : Thiago Anselmo, Tecgº
// AutorIIII : Gabriel da Silva Caetano, Estágio.
//
// CPU:     : ESP8266 - WEMOS D1 MINI Pro
// Clock:   : 16MHz
//*******************************************************************************************

# include <Wire.h>
# include <Thermistor.h>
# include <EEPROM.h>
# include <ArduinoJson.h>
# include <SSD1306Ascii.h>       	 // https://github.com/greiman/SSD1306Ascii
# include <SSD1306AsciiWire.h>   	 // https://github.com/greiman/SSD1306Ascii
# include <Ticker.h>                 // https://github.com/esp8266/Arduino/tree/master/libraries/Ticker
# include <ESP8266WiFi.h>            // https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi
# include <ESP8266mDNS.h>            // https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi
# include <WiFiUdp.h>                // https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi
# include <ArduinoOTA.h>             // https://github.com/esp8266/Arduino/tree/master/doc/esp8266wifi
# include <NewPingESP8266.h>         // https://github.com/jshaw/NewPingESP8266
# include <Adafruit_ADS1015.h>       // https://github.com/adafruit/Adafruit_ADS1X15

// ------------------------------------------------------------------------------------------
// VERSÃO DO FIRMWARE DO SISTEMA - EZBrewControl
// ------------------------------------------------------------------------------------------

# define SISTEMA F(" EZBrewControl  ")
# define VERSAO  F("  ver 1.0.00    ")

# define ERASE_EEPROM_USER        1        // 1 = Clear ALL EEPROM - USER
# define ERASE_EEPROM_CONFIG      1        // 1 = Clear ALL EEPROM - CONFIG

# define DEBUG_SERIAL             1        // 1 = Habilita o DEBUG pela Serial0
# define DEBUG_SOCKET             0        // 1 = Habilita o DEBUG pelo Socket (ETH0)

# define SENHA                    'A'      // Definição da senha de configuração: caracter 'A'
# define RESET                    0        // Definição de RESET

// ------------------------------------------------------------------------------------------
// DEFINIÇÕESS DOS PINOS DO MICROCONTROLADOR - ATMEGA 2561
// ------------------------------------------------------------------------------------------

/*# define PINO_TERMISTOR           1        // Pino onde o Termistor NTC 10k deve ser conectado - PINOS 2 e 5 (cabo EZBrew)
# define PINO_TECLADO             0        // Pino onde o KeyPad estão conectado - AD(0)
# define PINO_SSR                 5        // Pino de acionamento aquecedor (SSR) - HIGH -> ACENDE, LOW -> DESLIGA
# define PINO_RELAY1              22       // Pino Relé - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_SOLENOIDE1          34       // Pino Solenóide 1 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_SOLENOIDE2          35       // Pino Solenóide 2 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_NIVEL1              6        // Pino Sensor de Nível 1 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_NIVEL2              7        // Pino Sensor de Nível 2 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_FLUXO               19       // Pino Sensor Fluxo - INTERRUPÇÂO/INT1
# define PINO_AC                  18       // Pino de detecção de rede (AC) - INTERRUPÇÂO/INT0
# define PINO_BUZZER              4        // Pino do BUZZER/BEEP
# define LED_AMARELO              23       // Pino do LED AMARELO - HIGH -> ACENDE, LOW -> DESLIGA
# define LED_VERMELHO1            24       // Pino do LED VERMELHO 1 - HIGH -> ACENDE, LOW -> DESLIGA
# define LED_VERMELHO2            25       // Pino do LED VERMELHO 2 - HIGH -> ACENDE, LOW -> DESLIGA
*/
# define PINO_TERMISTOR           D8        // Pino onde o Termistor NTC 10k deve ser conectado - PINOS 2 e 5 (cabo EZBrew)
# define PINO_TECLADO             D8        // Pino onde o KeyPad estão conectado - AD(0)
# define PINO_SSR                 D8        // Pino de acionamento aquecedor (SSR) - HIGH -> ACENDE, LOW -> DESLIGA
# define PINO_RELAY1              D8       // Pino Relé - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_SOLENOIDE1          D8       // Pino Solenóide 1 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_SOLENOIDE2          D8       // Pino Solenóide 2 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_NIVEL1              D8        // Pino Sensor de Nível 1 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_NIVEL2              D8        // Pino Sensor de Nível 2 - LOW -> DESLIGA, HIGH -> ACENDE
# define PINO_FLUXO               D8       // Pino Sensor Fluxo - INTERRUPÇÂO/INT1
# define PINO_AC                  D8       // Pino de detecção de rede (AC) - INTERRUPÇÂO/INT0
# define PINO_BUZZER              D3        // Pino do BUZZER/BEEP
# define LED_AMARELO              D8       // Pino do LED AMARELO - HIGH -> ACENDE, LOW -> DESLIGA
# define LED_VERMELHO1            D8       // Pino do LED VERMELHO 1 - HIGH -> ACENDE, LOW -> DESLIGA
# define LED_VERMELHO2            D8       // Pino do LED VERMELHO 2 - HIGH -> ACENDE, LOW -> DESLIGA

// # define PINO_TIRE_BUTTON          D5        // Botão de seleção para pneus- INTERRUPÇÂO by Software - pcint.h
// # define PINO_CLEANER_BUTTON       D6        // Botão de seleção para aspirador - INTERRUPÇÂO by Software - pcint.h
// # define PINO_BUTTON               D7

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES GERAIS DO MICROCONTROLADOR - ATMEGA 2561
// ------------------------------------------------------------------------------------------

# define AD_DEBOUNCE              5        // AD Debounce do teclado

# define BUZZER_GERAL             2        // Número de vezes que soará o BIP na INTRODUCAO do sistema
# define BUZZER_SALVAR            2        // Número de vezes que soará o BIP ao SALVAR
# define BUZZER_ERRO              2        // Número de vezes que soará o BIP no ERRO do sistema
# define BUZZER_ALARME            1        // Número de vezes que soará o BIP no ALARME do sistema
# define BUZZER_PATAMAR           1        // Número de vezes que soará o BIP na chegada do PATAMAR
# define BUZZER_MOSTURA           4        // Número de vezes que soará o BIP ao final da MOSTURA
# define BUZZER_ADICOES           2        // Número de vezes que soará o BIP nas ADIÇÕES
# define BUZZER_WHIRLPOOL         2        // Número de vezes que soará o BIP nos WHIRLPOOL

# define DELAY_DEBOUNCE           5        // Delay Debounce do teclado
# define DELAY_EEPROM             5        // Delay Write/Read da EEPROM; Máximo de 3.3ms - DataSheet
# define DELAY_SALVAR             250      // Delay para SALVAR na EEPROM 
# define DELAY_GERAL              50       // Delay para tela GERAL
# define DELAY_ERRO               500      // Delay para tela de ERRO
# define DELAY_ALARME             500      // Delay para tela de ALARME
# define DELAY_OLED               5000     // Delay para para testes do display OLED
# define CLOCK_TIME 	          3000

# define TIME_GLOBAL              0.001    // TIME_GLOBAL(s) - 1ms
# define TIME_BUZZER              0.01     // TIME_BUZZER(s) - 10ms
# define TIME_RESET               5000     // Tempo para reiniciar o Módulo ENSAIoT
# define FREQ_PWM				  300      // 300Hz PWM

# define AMOSTRAS_TECLADO         10        // Número de amostras do AD do teclado
# define MAX_AD_TECLADO           26100    // Valor máximo do AD para considerar "tecla solta"
# define BOUNCE_AD                2        // Número de bounce do AD do teclado
# define BOUNCE_UP                10       // Número de bounce do teclado (antes da leitura)
# define F_TONE                   3500     // Frequencia do BUZZER
# define TEMPO_ALARME             2000     // Tempo (ms) de impressão das mensagens de alarme - LCD
# define TEMPO_CONF               1000     // Tempo (ms) de impressão das mensagens de configuração - LCD
# define TEMPO_PWM                180000   // Tempo (ms) para aguardar o PWM atingir o PATAMAR

# define RAMPA                    1        // RAMPA "statusFase" - pré-fervura
# define PATAMAR                  2        // PATAMAR "statusFase" - pré-fervura
# define FIM_MOSTURA              3        // Passo final da mostura "statusFase" - antes da FERVURA
# define FERVURA                  4        // Fervura "statusFase" - pós-fervura
# define WHIRLPOOL                5        // WHIRLPOOL "statusFase" - pós-fervura
# define DESCANSO                 6        // DESCANSO "statusFase" - pós-fervura
# define FINAL                    7        // FINAL do processo "statusFase" - pós-fervura

# define KEY_RIGHT                0        // Tecla pra DIREITA
# define KEY_UP                   1        // Tecla pra CIMA
# define KEY_DOWN                 2        // Tecla pra BAIXO
# define KEY_LEFT                 3        // Tecla pra ESQUERDA
# define KEY_SELECT               4        // Tecla ENTER para confirmação
# define KEY_ESC                  5        // Tecla para SAIR
# define KEY_NONE                 6        // Sem tecla Pressionada

# define V_KEY_UP                 F("UP")     // Tecla VIRTUAL pra CIMA
# define V_KEY_DOWN               F("DOWN")   // Tecla VIRTUAL pra BAIXO
# define V_KEY_RIGHT              F("RIGHT")  // Tecla VIRTUAL pra DIREITA
# define V_KEY_LEFT               F("LEFT")   // Tecla VIRTUAL pra ESQUERDA
# define V_KEY_SELECT             F("ENTER")  // Tecla VIRTUAL ENTER para confirmação
# define V_KEY_ESC                F("ESC")    // Tecla VIRTUAL para SAIR

# define LINHA_1				  0        // linha 1 do display OLED
# define LINHA_2				  2        // linha 2 do display OLED				
# define LINHA_3				  4        // linha 3 do display OLED
# define LINHA_4				  6        // linha 4 do display OLED

# define MIN_PATAMAR              1        // mínimo número de PATAMARES desejado
# define MIN_TEMPERATURA_PATAMAR  1        // Temperatura máxima desejada no PATAMAR
# define MIN_TEMPERATURA_FERVURA  25       // Temperatura máxima desejada na FERVURA
# define MIN_MINUTO_PATAMAR       0        // Tempo mínimo desejado no PATAMAR
# define MIN_MINUTO_FERVURA       1        // Tempo mínimo desejado na FERVURA
# define MIN_GRAMAS_ADICAO        1        // Gramas mínimas de adição de levedura

# define DEFAULT_MINUTO_PATAMAR   5        // DEFAULT MINUTO PATAMAR
# define DEFAULT_QT_PATAMAR       2        // DEFAULT QT PATAMAR
# define DEFAULT_TEMP_PATAMAR     25       // DEFAULT TEMPERATURA PATAMAR
# define DEFAULT_ADICAO           1        // DEFAULT número de ADIÇÕES
# define DEFAULT_MINUTO_ADICAO    15       // DEFAULT MINUTO de ADICAO
# define DEFAULT_GRAMAS_ADICAO    10       // DEFAULT GRAMAS de ADIÇÕES
# define DEFAULT_MINUTO_FERVURA   60       // DEFAULT MINUTO de FERVURA
# define DEFAULT_TEMP_FERVURA     100      // DEFAULT TEMPERATURA de FERVURA
# define DEFAULT_WHIRLPOOL        2        // DEFAULT número de WHIRLPOOL
# define DEFAULT_MINUTO_WHIRLPOOL 10       // DEFAULT MINUTO de WHIRLPOOL
# define DEFAULT_DESCANSO         1        // DEFAULT número de DESCANSO
# define DEFAULT_MINUTO_DESCANSO  10       // DEFAULT MINUTO de DESCANSO

# define MAX_PATAMAR              10       // máximo número de PATAMARES desejado
# define MAX_TEMPERATURA_PATAMAR  100      // Temperatura máxima desejada no PATAMAR
# define MAX_TEMPERATURA_FERVURA  200      // Temperatura máxima desejada na FERVURA
# define MAX_MINUTO_PATAMAR       300      // Tempo máximo desejado no PATAMAR
# define MAX_MINUTO_FERVURA       500      // Tempo máximo desejado na FERVURA
# define MAX_GRAMAS_ADICAO        500      // Gramas máximas de adição de levedura
# define MAX_MINUTO_WHIRLPOOL     60       // Tempo máximo desejado no WHIRLPOOL
# define MAX_MINUTO_DESCANSO      60       // Tempo máximo desejado no DESCANSO

# define MAX_PWM                  100      // PWM máximo (em percentual)

# define DEFAULT_MAX_PATAMAR      7        // máximo número default de PATAMARES desejado
# define DEFAULT_GRAU_MINUTO      1        // grau por minuto desejado
# define DEFAULT_PWM              95       // PWM default em percentual
# define DEFAULT_FERVURA          95       // Temperatura limite para iniciar tempo fervura

# define MIN_ADICAO               0        // MÍNIMO numeros de ADIÇÕES
# define MIN_MINUTOS_ADICAO       0        // MÍNIMO minutos de ADIÇÕES
# define MIN_WHIRLPOOL            0        // MÍNIMO numeros de WHIRLPOOL
# define MIN_MINUTOS_WHIRLPOOL    0        // MÍNIMO minutos de WHIRLPOOL
# define MIN_DESCANSO             0        // MÍNIMO numeros de DESCANSO
# define MIN_MINUTOS_DESCANSO     0        // MÍNIMO minutos de DESCANSO

# define MAX_RECEITA              5        // MAXIMA RECEITA
# define MAX_NOME                 13       // MAXIMO NOMES
# define MAX_ALFABETO             36       // MAXIMA letras do alfabeto
# define MAX_NUMEROS              10       // MAXIMA números
# define MAX_ADICAO               5        // MAXIMA numeros de ADIÇÕES
# define MAX_WHIRLPOOL            2        // MAXIMA numeros de WHIRLPOOL
# define MAX_DESCANSO             1        // MAXIMA numeros de DESCANSO
# define END_CONFIG               3800     // Endereço máximo da memória

# define ADDRESS_DS1307           0x68     // Endereço do RTC
# define I2C_ADDRESS              0x3C     // Address of SSD1306 CHIP - 128x64 OLED [0x3C or 0x3D]

# define MAX_CONFIG               1        // Máxima quantidade configurações - EEPROM
# define MAX_BYTES_EEPROM         4096     // Máxima quantidade de bytes na EEPROM
# define END_CONFIG               2048     // END position EEPROM configuration

# define CLOCK_EEPROM             400000L  // Let's go fast! Set I2C clock to 400kHz

# define SERVER_PORT              5000     // Porta de conexão TCP/IP - Server ENSA Tecnologia
# define ETH_CS                   10       // Select the Ethernet Module.
# define SD_CS                    4        // De-Select the internal SD Card
# define TIMEOUT_SOCKET           3        // Timeout connection socketENSA 
# define SERIAL_BAUD_RATE         115200   // Baud Rate Serial (DEBUG) e Serial1 (Bluetooth)
# define DATA_UTIL_SERIAL         512      // Tamanho máximo do array "DATA_UTIL_SERIAL"

# define MEDIA_TEMPERATURE        20       // Média da temperatura

# define ARDUINOJSON_ENABLE_PROGMEM    1   // Enable "ARDUINOJSON_ENABLE_PROGMEM"

# define DEFAULT_ID_MODULE		  F("1020ABC01") // Número de série/id padrão

//# define RTC_OFF

// ------------------------------------------------------------------------------------------
// PASSOS DA MAQUINA DE ESTADOS - FUNÇÕES E DISPLAY
// ------------------------------------------------------------------------------------------

# define mVERSAO                  0        // Passo inicial: mostra VERSAO do SISTEMA
# define mSTANDBY                 1        // Passo TELA BOAS VINDAS da máquina de estado
# define mMENU                    2        // Passo MENU da máquina de estado
# define mRECEITAS                3        // Passo MENU DE RECEITAS da máquina de estado
# define mRECEITASEL              4        // Passo RECEITA SELECIONADA da máquina de estado
# define mAPAGA_RECEITA           5        // Passo APAGA RECEITA SELECIONADA da máquina de estado
# define mCONTINUA_PRODUCAO       6        // Passo CONTINUA PRODUCAO da máquina de estado
# define mEXECUTA_PRODUCAO        7        // Passo EXECUTA PRODUCAO da máquina de estado
# define mDERRUBAR_MALTE          8        // Passo DERRUBAR MALTE da máquina de estado
# define mLOOP_BRASSAGEM          9        // Passo FASE BRASSAGEM da máquina de estado
# define mMENU_FERVURA            10       // Passo MENU FERVURA da máquina de estado
# define mEXECUTA_FERVURA         11       // Passo EXECUTA FERVURA da máquina de estado
# define mLOOP_FERVURA            12       // Passo FASE FERVURA da máquina de estado
# define mADICAO_FERVURA          13       // Passo FASE FERVURA da máquina de estado
# define mMENU_WHIRLPOOL          14       // Passo MENU WHIRLPOOL da máquina de estado
# define mEXECUTA_WHIRLPOOL       15       // Passo EXECUTA WHIRLPOOL da máquina de estado
# define mLOOP_WHIRLPOOL          16       // Passo FASE WHIRLPOOL da máquina de estado
# define mMENU_DESCANSO           17       // Passo FASE DESCANSO da máquina de estado
# define mEXECUTA_DESCANSO        18       // Passo EXECUTA DESCANSO da máquina de estado
# define mLOOP_DESCANSO           19       // Passo FASE DESCANSO da máquina de estado
# define mCRIAR_RECEITA           20       // Passo CRIAR RECEITA da máquina de estado
# define mNOME_RECEITA            21       // Passo MANTEM RECEITA da máquina de estado
# define mQT_PATAMAR              22       // Passo ENTRADA STRING da máquina de estado
# define mTEMPMAX_PATAMAR         23       // Passo TEMPERATURA MAXIMA PATAMAR da máquina de estado
# define mMINUTO_PATAMAR          24       // Passo MINUTO PATAMAR PATAMAR da máquina de estado
# define mTEMPMAX_FERVURA         25       // Passo TEMPERATURA TEMPERATURA DE FERVURA da máquina de estado
# define mMAXMIN_FERVURA          26       // Passo MAXIMO MINUTO DE FERVURA da máquina de estado
# define mQT_ADICAO               27       // Passo QUANTIDADE MAXIMA DE ADICAO da máquina de estado
# define mNOME_ADICAO             28       // Passo NOME DA ADICAO da máquina de estado 
# define mGRAMAS_ADICAO           29       // Passo GRAMAS DA ADICAO da máquina de estado 
# define mMINUTOS_ADICAO          30       // Passo MINUTOS DA ADICAO da máquina de estado
# define mQT_WHIRLPOOL            31       // Passo QUANTIDADE DE WHIRLPOOL da máquina de estado
# define mNOME_WHIRLPOOL          32       // Passo NOME DE WHIRLPOOL da máquina de estado
# define mMINUTO_WHIRLPOOL        33       // Passo MINUTOS DE WHIRLPOOL da máquina de estado
# define mQT_DESCANSO             34       // Passo QUANTIDADE DE DESCANSO da máquina de estado
# define mNOME_DESCANSO           35       // Passo NOME DE DESCANSO da máquina de estado
# define mMINUTO_DESCANSO         36       // Passo MINUTOS DE DESCANSO da máquina de estado
# define mSALVA_RECEITA           37       // Passo SALVA RECEITA da máquina de estado
# define mADICIONAR_AGUA          38       // Passo ADICIONAR AGUA da máquina de estado
# define mCALCULA_FLUXO           39       // Passo CALCULA FLUXO da máquina de estado
# define mSENHA_CONFIGURACAO      40       // Passo EXECUTA CONFIGURACAO da máquina de estado
# define mMENU_CONFIGURACAO       41       // Passo MENU CONFIGURACAO da máquina de estado
# define mSET_MAX_PATAMAR         42       // Passo SET MAX PATAMAR da máquina de estado
# define mSET_GRAU_MINUTO         43       // Passo SET GRAU POR MINUTO da máquina de estado
# define mSET_PWM_SETPOINT        44       // Passo SET PWM SETPOINT da máquina de estado
# define mSET_DATA_HORA           45       // Passo SET DATA HORA da máquina de estado
# define mLIMPA_EEPROM            46       // Passo LIMPA EEPROM da máquina de estado

// ------------------------------------------------------------------------------------------
// PASSOS DA MAQUINA DE ESTADOS - BUZZER
// ------------------------------------------------------------------------------------------

# define gmBUZZER_OFF            0         // Passo BUZZER OFF da máquina de estado do buzzer
# define gmBIP_GLOBAL            1         // Passo BIP GLOBAL da máquina de estado do buzzer
# define gmBIP_ALARME            2         // Passo BIP ALARME da máquina de estado do buzzer

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES DAS VARIÁVEIS GLOBAIS
// ------------------------------------------------------------------------------------------

#ifdef PROGMEM

boolean _atendeMaquina;
boolean _atendeDisplay;
boolean _atendeBuzzer;
String stringDisplay = "";

int countADTeclado = 0;
int ADTecladoAux = 0;
int CopyADTecladoAux = 0;
int bounceADTeclado = 0;

volatile int passoMaquina;
volatile int passoDisplay;
volatile int passoBuzzer;

unsigned long ADTeclado = 0;

int timerBuzzer;
int periodoBuzzer;
int nBips;

boolean _endString = false;
boolean _endInteger = false;
boolean _endValue = false;
boolean _digitarString = false;
boolean _digitarInteger = false;
boolean _digitarValue = false;

boolean _rewriteDisplay = true;

int numLetra = -1;
String stDigitado;
String stChar = "_";
float valorDigitado = 0;

int ind = 0;
int qtde;
int qtdeMin;
String prOper;

boolean _contaTempo = false;

byte numPatamar;
byte qtdePatamar;
int tempoInicial;
int tempoProximo;
String nomeReceita;
String nomeAdicao;
String nomeWhirlpool;
String nomeDescanso;

byte statusFase;

int valorPwm = MAX_PWM;
int valorPwmManual;
int valorTempoManual = 0;

int tempInicialAquecimento = -1000;
int qtdeReceita = 0;
int idReceitaSel;
float speedAquecimento = 0;

boolean _temperaturaMaxEntrou = false;
String stringSenha;
String stResp = "_";

boolean _derrubaMalte = false;
boolean _loadTimerPwm = false;

int tempoInicialVelocidade = 0;
float grauPorMinuto = 0;
float temperaturaFloatInicial = 0;

int tempoRestante = 0;
int salvaTempoAtual;

int wdt50ms;
int wdt1000ms;
int wdt3000ms;
uint8_t wdtDadosBrassagem;
int wdtTimesReceita;
int wdtTimesDisplay;
int wdtTimesBlink;
int wdtTimesTemperatura;
int bounceUP;
long wdtPwmFull;

boolean _KEY_RIGHT;
boolean _KEY_UP;
boolean _KEY_DOWN;
boolean _KEY_LEFT;
boolean _KEY_SELECT;
boolean _KEY_ESC;
boolean _KEY_NONE;

boolean _keyTask;
boolean _keyPress;
boolean _atendeTeclado;

boolean _flag50ms;
boolean _flag1000ms;
boolean _changeMsg;
boolean _flag3000ms;
boolean _pwmFull;
boolean _printDebug;

boolean _refreshDisplay = false;
boolean _msgConvite;
boolean _msgEzbrew;

boolean _msgDisplay1;
boolean _msgDisplay2;

boolean _faltouAC;
boolean _atendeSocket;

boolean _sistemaUp = false;

volatile byte pulseAC = 0;
volatile byte freqAC = 0;
volatile byte Npulse = 0;

int volumeDigitado;
float calFactor = 4.5;
float flow = 0.0;
float milli = 0.0;
float liters = 0.0;
float alarmNivel = 0;

boolean _readTemperature = false;
unsigned int countTemperature = 0;
float realTemperature = 0;
float mediaTemperature = 0;

boolean _flagAjusteDataHora = false;
uint8_t opcMenuDataHora = 0;

Thermistor thermistor(PINO_TERMISTOR);

SSD1306AsciiWire oled;

Ticker TimerGlobal;
Ticker TimerBuzzer;

Adafruit_ADS1115 ads;                              /* Use this for the 16-bit version */

boolean _newDataSerial = false;
char receivedSerial[DATA_UTIL_SERIAL];

static boolean _recvInProgress = false;
static int idx = 0;
volatile char dataChar;

String code = "";

uint8_t totalDeReceitas = 0;
uint8_t passoAnterior = 0;


//--------------------------------------

// Para placa genérica
/*unsigned int buttonState;                         
unsigned int lastButtonState = LOW;               
unsigned long lastDebounceTimeButton = 0;         

unsigned int tireButtonState;                     
unsigned int lastTireButtonState = LOW;           
unsigned long lastDebounceTimeTireButton = 0;     

unsigned int cleanerButtonState;                   
unsigned int lastCleanerButtonState = LOW;         
unsigned long lastDebounceTimeCleanerButton = 0;   
unsigned int debounceDelay = 100;  */

//--------------------------------------

// ------------------------------------------------------------------------------------------
// DEFINIÇÔES DAS STRUCTS
// ------------------------------------------------------------------------------------------

//WWWWWWWWWW*********************************************************************************
// Funcao   : Patamar
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

struct Patamar {
  int temperaturaMax = DEFAULT_TEMP_PATAMAR;
  int tempo = DEFAULT_MINUTO_PATAMAR;
  int tempoDecorrido;
  boolean concluido = false;
};

Patamar patamar[MAX_PATAMAR];

//WWWWWWWWWW*********************************************************************************
// Funcao   : Adicao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

struct Adicao {
  char nome[MAX_NOME] = "";
  int tempo = DEFAULT_MINUTO_ADICAO;
  unsigned int qtde = DEFAULT_ADICAO;
};

//WWWWWWWWWW*********************************************************************************
// Funcao   : Whirlpool
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

struct Whirlpool {
  char nome[MAX_NOME] = "";
  int tempo = DEFAULT_MINUTO_WHIRLPOOL;
  int qtde = DEFAULT_WHIRLPOOL;
};

//WWWWWWWWWW*********************************************************************************
// Funcao   : Descanso
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

struct Descanso {
  char nome[MAX_NOME] = "";
  int tempo = DEFAULT_MINUTO_DESCANSO;
  int qtde = DEFAULT_DESCANSO;
};

//WWWWWWWWWW*********************************************************************************
// Funcao   : Receita
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

struct Receita {

  uint8_t tempFervura = DEFAULT_TEMP_FERVURA;
  uint8_t statusBrassagem = RESET; //0=Parada, 1=Em produção, 2=Fervura, 3=Whirlpool e 4=Descanso

  byte qtdePatamar = DEFAULT_QT_PATAMAR;
  byte qtdeAdicao = DEFAULT_ADICAO;
  byte qtdeWhirlpool = DEFAULT_WHIRLPOOL;
  byte qtdeDescanso = DEFAULT_DESCANSO;

  char nome[MAX_NOME] = "";

  unsigned int qtdeMinDescanso = DEFAULT_MINUTO_DESCANSO;
  unsigned int qtdeMinFervura = DEFAULT_MINUTO_FERVURA;
  int codigo = RESET;
  int qtdeMinWhirlpool = DEFAULT_MINUTO_WHIRLPOOL;

  Adicao adicao[MAX_ADICAO];
  Patamar patamar[MAX_PATAMAR];
  Whirlpool whirlpool[MAX_WHIRLPOOL];
  Descanso descanso[MAX_DESCANSO];
};

//WWWWWWWWWW*********************************************************************************
// Funcao   : ConfigGeral
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

struct ConfigGeral {
  byte qtdeMaxPatamar = DEFAULT_MAX_PATAMAR;
  byte pwmSetpoint = DEFAULT_PWM;
  uint8_t tempoEnvioBrassagem = 1; // 1 Segundo
  float grauPorMinuto = DEFAULT_GRAU_MINUTO;

  char idModule[10];
};

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES DAS VARIÁVEIS - GLOBAIS II
// ------------------------------------------------------------------------------------------

ConfigGeral configGeral;

Receita regReceita[MAX_RECEITA + 1];
Receita receitaSel;
Receita prReceita;
String menuReceita[MAX_RECEITA + 1];

Adicao adicaoFervura[MAX_ADICAO];
byte qtdeAdicao;
unsigned int qtdeMinFervura;
uint8_t tempFervura;
byte numAdicao;

Whirlpool whirlpoolFervura[MAX_WHIRLPOOL];
byte qtdeWhirlpool;
int qtdeMinWhirlpool;
byte numWhirlpool;

Descanso descansoFervura[MAX_DESCANSO];
byte qtdeDescanso;
int qtdeMinDescanso;
byte numDescanso;

uint8_t tamanhoMenu = 4;
int8_t posicaoMenu = 0;

boolean _analisaOpcMenu = false;

const char* const alfabeto[] PROGMEM = {"A", "B", "C", "D", "E", "F",
							        	"G", "H", "I", "J","K", "L", 
							        	"M", "N", "O", "P", "K", "R",
							        	"S", "T", "U", "V", "W", "X", 
							        	"Y", "Z", "0", "1", "2", "3",
							        	"4", "5", "6", "7", "8", "9"};

const char* const numeros[] PROGMEM = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

const char stVoltar[] PROGMEM = "Voltar"; 

const char stEscolheReceita[] PROGMEM = "Escolhe receita";  
const char stCriarReceita[] PROGMEM = "Criar receita";
const char stAdicionarAgua[] PROGMEM = "Adicionar agua";
const char stConfiguracao[] PROGMEM = "Configuracao";
const char* const menu[] PROGMEM = {stEscolheReceita, stCriarReceita, stAdicionarAgua, stConfiguracao};

const char stBrassagem[] PROGMEM = "Brassagem";
const char stEditar[] PROGMEM = "Editar";
const char stApagar[] PROGMEM = "Apagar";
const char* const menuReceitaSel[] PROGMEM = {stVoltar, stBrassagem, stEditar, stApagar};

const char stMaxPatamar[] PROGMEM = "MAX patamares";
const char stGrauPorMinuto[] PROGMEM = "Grau p/ minuto";
const char stPwmSetpoint[] PROGMEM = "PWM p/ setpoint";
const char stAjustarDataHora[] PROGMEM = "Data e hora";
const char stResetDefault[] PROGMEM = "Reset Default";
const char* const menuConfig[] PROGMEM = {stVoltar, stMaxPatamar, stGrauPorMinuto, stPwmSetpoint, stAjustarDataHora, stResetDefault};

const char stIniciarFervura[] PROGMEM = "Iniciar fervura";  
const char* const menuFervura[] PROGMEM = {stIniciarFervura};

const char stEditarReceita[] PROGMEM = "Editar receita";
const char stMostrarMais[] PROGMEM = "Mostar Mais";
const char stPwm[] PROGMEM = "PWM";
const char* const menuBrassagemPwm[] PROGMEM = {stVoltar, stPwm, stEditarReceita, stMostrarMais};
const char* const menuBrassagem[] PROGMEM = {stVoltar, stEditarReceita, stMostrarMais};

const char* const menuDataHora[] PROGMEM = {stVoltar,"Hora", "Minutos", "Segundos", "Dia da semana", "Dia do mes", "Mes", "Ano"};

#else
#warning PROGMEM is not supported on this platform
#endif

// ------------------------------------------------------------------------------------------
// DEFINIÇÔES DAS FUNÇÕES
// ------------------------------------------------------------------------------------------

//WWWWWWWWWW*********************************************************************************
// Funcao   : loopProducao
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void loopProducao() { //Logica do patamar

  calculaGrauPorMinuto(mediaTemperature); //Mede velocidade
  setStatusFase(mediaTemperature);

  tempoRestante = (patamar[numPatamar].tempo * 60) + (valorTempoManual * 60) - tempoInicial;
  if (tempoInicial > 0) {
    if (tempoRestante <= 0) {
      receitaSel.patamar[numPatamar].concluido = true; //Salva patamar como concluido
      salvaReceita(receitaSel);
      numPatamar++;
      _temperaturaMaxEntrou = false;
      if (numPatamar >= qtdePatamar) {
        desligaAquecimento();
        if (passoMaquina == mLOOP_FERVURA) {
          receitaSel.statusBrassagem = 3; //Whirlpool
          salvaReceita(receitaSel);
          bipOK();
          printDisplay(F(" Fim da FERVURA "), F("Concl.c/sucesso"), TEMPO_ALARME);
          passoMaquina = mEXECUTA_WHIRLPOOL;
        }
        else if (passoMaquina == mLOOP_WHIRLPOOL) {
          receitaSel.statusBrassagem = 4; //Descanso
          salvaReceita(receitaSel);
          bipOK();
          printDisplay(F("Fim do WHIRLPOOL"), F("Concl.c/sucesso"), TEMPO_ALARME);
          passoMaquina = mEXECUTA_DESCANSO;
        }
        else if (passoMaquina == mLOOP_DESCANSO) {
          _derrubaMalte = false;
          receitaSel.statusBrassagem = RESET;
          qtdePatamar = receitaSel.qtdePatamar;
          for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
            receitaSel.patamar[ind].concluido = false;
          salvaReceita(receitaSel);
          bipInicial();
          printDisplay(F("Fim do PROCESSO"), F("Concl.c/sucesso"), TEMPO_ALARME);
          executaBoasVindas();
          passoMaquina = mSTANDBY;
        }
        else {
          bipOK();
          printDisplay(F(" Fim PATAMARES "), F("Concl.c/sucesso"), TEMPO_ALARME);
          passoMaquina = mMENU_FERVURA;
        }
      }
      else {
        bipOK();
        printDisplay(F(" Inicio de novo "), " PATAMAR N: " + String(numPatamar + 1), TEMPO_ALARME);
        valorTempoManual = 0;
        _loadTimerPwm = false;
        _pwmFull = false;
      }
      desligaLeds();
      valorTempoManual = 0;
      _contaTempo = false; //Quando "false" paraliza o "tempoDecorrido" durante chegada no patamar
      tempoInicial = 0;
      tempoProximo = 0;
      wdtDadosBrassagem = 0;
      return;
    }
  }

  char tempoAtual[17];
  char tempoIncremental[17];
  char minutoAtual[17];
  char segundoAtual[17];

  char tempoAdicao[17];
  char tempoWhirlpool[17];
  char tempoDescanso[17];

  int tempoRestanteAdicao;
  int tempoRestanteWhirlpool;
  int tempoRestanteDescanso;

  sprintf(tempoAtual, "%02u:%02u", tempoRestante / 60, tempoRestante % 60);
  sprintf(tempoIncremental, "%02u:%02u", tempoProximo / 60, tempoProximo % 60);
  sprintf(minutoAtual, "%02u", tempoRestante / 60);
  sprintf(segundoAtual, "%02u", tempoRestante % 60);

  if (passoMaquina == mLOOP_FERVURA) { //Adicao fervura
    if (numAdicao == 0)
      tempoRestanteAdicao = ((receitaSel.qtdeMinFervura * 60) - (adicaoFervura[numAdicao].tempo * 60)) - tempoProximo;
    else if (numAdicao < qtdeAdicao)
      tempoRestanteAdicao = ((adicaoFervura[numAdicao - 1].tempo * 60) - (adicaoFervura[numAdicao].tempo * 60)) - tempoProximo;
    else
      tempoRestanteAdicao = (adicaoFervura[numAdicao - 1].tempo * 60) - tempoProximo;
    if (tempoRestanteAdicao <= 0)
      tempoRestanteAdicao = 0;

    sprintf(tempoAdicao, "%02u:%02u", tempoRestanteAdicao / 60, tempoRestanteAdicao % 60);
    processaFervura(tempoAtual);

    if (_printDebug/* && wdtDadosBrassagem > 1*/)
    {
      _printDebug = false;

      if (DEBUG_SERIAL) { //FF03
        const size_t bufferSize = JSON_OBJECT_SIZE(7);
        DynamicJsonDocument jsonAdicaoBuffer(bufferSize);
        JsonObject dataAdicao = jsonAdicaoBuffer.to<JsonObject>();

        dataAdicao[F("header")] = F("4E");
        dataAdicao[F("product")] = F("0002");
        dataAdicao[F("type")] = F("D002");
        dataAdicao[F("id")] = String(configGeral.idModule);
        dataAdicao[F("typeCmd")] = F("FF");
        dataAdicao[F("cmd")] = 2;

        dataAdicao[F("restante")] = String(tempoRestanteAdicao);
        dataAdicao[F("proximo")] = String(tempoAdicao);
        dataAdicao[F("nAdicao")] = String(numAdicao);
        dataAdicao[F("tempo")] = String(tempoProximo);

        code = F("A1");

        JsonArray error = dataAdicao.createNestedArray(F("code"));
        error.add(code);

        serializeJsonPretty(dataAdicao, Serial1);
        dataAdicao.clear();
      }
    }
  }

  else if (passoMaquina == mLOOP_WHIRLPOOL) { //Whirlpool

    if (numWhirlpool == 0)
      tempoRestanteWhirlpool = (receitaSel.qtdeMinWhirlpool * 60);
    else
      tempoRestanteWhirlpool = (whirlpoolFervura[numWhirlpool - 1].tempo * 60) - tempoProximo;
    if (tempoRestanteWhirlpool <= 0)
      tempoRestanteWhirlpool = 0;

    sprintf(tempoWhirlpool, "%02u:%02u", tempoRestanteWhirlpool / 60, tempoRestanteWhirlpool % 60);
    processaWhirlpool(tempoIncremental); // Esse é o tempo referência para o ALARME (Compara com a EEPROM)

    if ((tempoRestanteWhirlpool / 60) <= 0 && (tempoRestanteWhirlpool % 60) <= 0 && (numWhirlpool < qtdeWhirlpool && contandoTempo()))
    {
      tempoProximo = 0;
      bipAtencao();
      printDisplay(F("Start WHIRLPOOL:"), String(whirlpoolFervura[numWhirlpool].nome), TEMPO_ALARME);
      numWhirlpool = numWhirlpool + 1;
      desligaLeds();
    }

    if (_printDebug/* && wdtDadosBrassagem > 1*/)
    {
      _printDebug = false;

      if (DEBUG_SERIAL) { //FF04
        const size_t bufferSize = JSON_OBJECT_SIZE(7);
        DynamicJsonDocument jsonWhirlpoolBuffer(bufferSize);
        JsonObject dataWhirpool = jsonWhirlpoolBuffer.to<JsonObject>();

        dataWhirpool[F("header")] = F("4E");
        dataWhirpool[F("product")] = F("0002");
        dataWhirpool[F("type")] = F("D002");
        dataWhirpool[F("id")] = String(configGeral.idModule);
        dataWhirpool[F("typeCmd")] = F("FF");
        dataWhirpool[F("cmd")] = 2;

        dataWhirpool[F("restante")] = String(tempoRestanteWhirlpool);
        dataWhirpool[F("proximo")] = String(tempoWhirlpool);
        dataWhirpool[F("nWhirlpool")] = String(numWhirlpool);
        dataWhirpool[F("tempo")] = String(tempoProximo);

        code = F("A1");

        JsonArray error = dataWhirpool.createNestedArray(F("code"));
        error.add(code);

        serializeJsonPretty(dataWhirpool, Serial1);
        dataWhirpool.clear();
      }
    }
  }

  else if (passoMaquina == mLOOP_DESCANSO) { //Descanso
    if (numDescanso == 0)
      tempoRestanteDescanso = (receitaSel.qtdeMinDescanso * 60);
    else
      tempoRestanteDescanso = (descansoFervura[numDescanso - 1].tempo * 60) - tempoProximo;
    if (tempoRestanteDescanso <= 0)
      tempoRestanteDescanso = 0;

    sprintf(tempoDescanso, "%02u:%02u", tempoRestanteDescanso / 60, tempoRestanteDescanso % 60);
    processaDescanso(tempoIncremental); // Esse é o tempo referência para o ALARME (Compara com a EEPROM)

    if (_printDebug/* && wdtDadosBrassagem > 1*/)
    {
      _printDebug = false;

      if (DEBUG_SERIAL) { //FF05
        const size_t bufferSize = JSON_OBJECT_SIZE(7);
        DynamicJsonDocument jsonDescansoBuffer(bufferSize);
        JsonObject dataDescanso = jsonDescansoBuffer.to<JsonObject>();

        dataDescanso[F("header")] = F("4E");
        dataDescanso[F("product")] = F("0002");
        dataDescanso[F("type")] = F("D002");
        dataDescanso[F("id")] = String(configGeral.idModule);
        dataDescanso[F("typeCmd")] = F("FF");
        dataDescanso[F("cmd")] = 2;

        dataDescanso[F("restante")] = String(tempoRestanteDescanso);
        dataDescanso[F("proximo")] = String(tempoDescanso);
        dataDescanso[F("nDescanso")] = String(numDescanso);
        dataDescanso[F("tempo")] = String(tempoProximo);

        code = F("A1");

        JsonArray error = dataDescanso.createNestedArray(F("code"));
        error.add(code);

        serializeJsonPretty(dataDescanso, Serial1);
        dataDescanso.clear();
      }
    }

    if (tempoRestanteDescanso == 0)
    {
      _derrubaMalte = false;
      receitaSel.statusBrassagem = RESET;
      qtdePatamar = receitaSel.qtdePatamar;
      for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
        receitaSel.patamar[ind].concluido = false;
      salvaReceita(receitaSel);
      bipInicial();
      printDisplay(F("Fim do PROCESSO"), F("Concl.c/sucesso"), TEMPO_ALARME);
      executaBoasVindas();
      passoMaquina = mSTANDBY;
    }
  }

  else
  {
    if (_printDebug/* && wdtDadosBrassagem > 1*/)
    {
      _printDebug = false;
      if (DEBUG_SERIAL) { //FF02
        const size_t bufferSize = JSON_OBJECT_SIZE(7);
        DynamicJsonDocument jsonBrassagemBuffer(bufferSize);
        JsonObject dataBrassagem = jsonBrassagemBuffer.to<JsonObject>();

        dataBrassagem[F("header")] = F("4E");
        dataBrassagem[F("product")] = F("0002");
        dataBrassagem[F("type")] = F("D002");
        dataBrassagem[F("id")] = String(configGeral.idModule);
        dataBrassagem[F("typeCmd")] = F("FF");
        dataBrassagem[F("cmd")] = 2;

        dataBrassagem[F("temp")] = String((float)(mediaTemperature), 2);
        dataBrassagem[F("status")] = String(statusFase);
        dataBrassagem[F("pwm")] = String(valorPwm);
        dataBrassagem[F("speed")] = String((float)(speedAquecimento), 2);
        dataBrassagem[F("patamar")] = String(numPatamar + 1);
        dataBrassagem[F("tempo")] = String(tempoRestante);

        code = F("A1");

        JsonArray error = dataBrassagem.createNestedArray(F("code"));
        error.add(code);

        serializeJsonPretty(dataBrassagem, Serial1);
        dataBrassagem.clear();
      }
    }
  }

  
  if (passoMaquina == mLOOP_FERVURA) 
  {
  	oledRestart();
  	if(wdtTimesDisplay == 0)
  	{
  		wdtTimesDisplay = 5000;
  		_rewriteDisplay = true;

	    oled.clear();
	    oled.setCursor(0, LINHA_1);
	    oled.println(F("--EM BRASSAGEM--"));
	    oled.println(getStatusFase() + "/PWM:" + String(valorPwm) + "%");
  	}

  	if(_flag1000ms || _rewriteDisplay)
  	{
  		_rewriteDisplay = false;
  		_flag1000ms = false;

  		oled.setCursor(0, LINHA_3);
	    oled.println("Tempo: " + String(tempoAdicao) + "/" + String(minutoAtual));
	    oled.print("T: " + getValor1Dec(mediaTemperature) + "/" + String(patamar[numPatamar].temperaturaMax) + "C ");
  	}
  }
  else if (passoMaquina == mLOOP_WHIRLPOOL) 
  {
  	oledRestart();
  	if(wdtTimesDisplay == 0)
  	{
  		wdtTimesDisplay = 5000;
  		_rewriteDisplay = true;

	    oled.clear();
	    oled.setCursor(0, LINHA_1);
	    oled.println(F("--EM BRASSAGEM--"));
	    oled.println(getStatusFase() + ": " + String(numWhirlpool) + "/" + String(qtdeWhirlpool)/*+ String(minutoAtual)*/);
  	}
  	if(_flag1000ms || _rewriteDisplay)
  	{
  		_rewriteDisplay = false;
  		_flag1000ms = false;

  		oled.setCursor(0, LINHA_3);
  		oled.println("Tempo: " + String(tempoWhirlpool) + "/" + String(minutoAtual+1));
    	oled.print("T: " + getValor1Dec(mediaTemperature) + "/" + String(patamar[numPatamar].temperaturaMax) + "C "); 
 	}
  }
  else if (passoMaquina == mLOOP_DESCANSO) 
  {
  	oledRestart();
  	if(wdtTimesDisplay == 0)
  	{
  		wdtTimesDisplay = 5000;
  		_rewriteDisplay = true;

	    oled.clear();
	    oled.setCursor(0, LINHA_1);
	    oled.println(F("--EM BRASSAGEM--"));
	    oled.println(getStatusFase() /*+ "/" + String(minutoAtual)*/);
  	}
  	if(_flag1000ms || _rewriteDisplay)
  	{
  		_rewriteDisplay = false;
  		_flag1000ms = false;

  		oled.setCursor(0, LINHA_3);
	    oled.println("Tempo: " + String(tempoDescanso));
	    oled.print("T: " + getValor1Dec(mediaTemperature) + "/" + String(patamar[numPatamar].temperaturaMax) + "C ");
	}
  }
  else 
  {
  	oledRestart();
  	if(wdtTimesDisplay == 0)
  	{
  		wdtTimesDisplay = 5000;
  		_rewriteDisplay = true;

    	oled.clear();
    	oled.setCursor(0, LINHA_1);
    	oled.println(F("--EM BRASSAGEM--"));
    	oled.print(getStatusFase() + ": " + String(numPatamar + 1) + "/" + String(qtdePatamar));	
  	}
  	if(_flag1000ms || _rewriteDisplay)
  	{
  		_rewriteDisplay = false;
  		_flag1000ms = false;
  		
  		oled.setCursor(0, LINHA_3);
  		if (String(tempoAtual) == F("00:00"))
      		sprintf(tempoAtual, "%02u:%02u", patamar[numPatamar].tempo, 0);
    	
    	oled.println("Tempo: " + String(tempoAtual));
    	oled.print("T: " + getValor1Dec(mediaTemperature) + "/" + String(patamar[numPatamar].temperaturaMax) + "C");
  	}
  }
}


//WWWWWWWWWW*********************************************************************************
// Funcao   : stateMachine
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void stateMachine()
{
  static boolean auxKeyEsc = false;

  if (!_atendeMaquina)
    return;
  _atendeMaquina = false;

  switch (passoMaquina) 
  {
    case mSTANDBY:
      executaBoasVindas();
      break;

    case mMENU:
      executaMenuInicial();
      break;

    case mRECEITAS:
      executaMenuReceitas();
      break;

    case mRECEITASEL:
      executaMenuReceitaSel();
      break;

    case mAPAGA_RECEITA:
      apagaReceita(idReceitaSel);
      break;

    case mCONTINUA_PRODUCAO:
      if (receitaSel.statusBrassagem != RESET) { //Brassagem já iniciada
        perguntaSimNao(F("Continuar?"));
        if (_endString) {
          _endString = false;
          if (stResp == F("N")) {
            receitaSel.statusBrassagem = RESET; //Reseta o "statusBrassagem"
            for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
              receitaSel.patamar[ind].concluido = false;
            salvaReceita(receitaSel);
          }
          passoMaquina = mEXECUTA_PRODUCAO;
          break;
        }
      }
      else {
        passoMaquina = mEXECUTA_PRODUCAO;
        break;
      }
      break;

    case mEXECUTA_PRODUCAO:
      executaProducao();
      break;

    case mLOOP_BRASSAGEM:
      {
        if (_keyPress)
        {
          if (_KEY_RIGHT)
          {
            numPatamar++;
            _contaTempo = false;
            _pwmFull = false;
            _loadTimerPwm = false;
            tempoInicial = 0;
            tempoProximo = 0;
            wdtDadosBrassagem = 0;
            if (numPatamar >= qtdePatamar)
            {
              numPatamar = qtdePatamar - 1;
              passoMaquina = mMENU_FERVURA;
              break;
            }
          }
          else if (_KEY_LEFT)
          {
            numPatamar--;
            _contaTempo = false;
            _pwmFull = false;
            _loadTimerPwm = false;
            tempoInicial = 0;
            tempoProximo = 0;
            wdtDadosBrassagem = 0;
            if (numPatamar == 0) {
              _temperaturaMaxEntrou = false;
              numPatamar = 0;
              tempoInicial = 0;
              tempoProximo = 0;
              wdtDadosBrassagem = 0;
              _contaTempo = false;
              qtdePatamar = receitaSel.qtdePatamar;

              for (register int ind = 0; ind < qtdePatamar; ind++)
              {
                patamar[ind] = receitaSel.patamar[ind];
                if (receitaSel.statusBrassagem == RESET) //Parada
                  patamar[ind].concluido = false;
              }
              salvaReceita(receitaSel);
            }
            if (numPatamar == 255) { //Atenção: "numPatamar": declarado como: "byte"
              numPatamar = 0;
              tempoInicial = 0;
              _derrubaMalte = false;
              wdtDadosBrassagem = 0;
              receitaSel.statusBrassagem = RESET;
              qtdePatamar = receitaSel.qtdePatamar;
              for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
                receitaSel.patamar[ind].concluido = false;
              salvaReceita(receitaSel);
              desligaAquecimento();
              passoMaquina = mMENU;
              break;
            }
          }
          else if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) {
            entradaTempoManual(F("Tempo"), valorTempoManual, -500, 500);
            valorTempoManual = valorDigitado;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }

        }

        loopProducao();
      }
      break;

    case mDERRUBAR_MALTE:
      {
        if (_keyPress)
        {
          if (_KEY_SELECT)
          {
            desligaLeds();
            _derrubaMalte = true;
            _contaTempo = true;
            _printDebug = true;
            wdtDadosBrassagem = 0;
            passoMaquina = mLOOP_BRASSAGEM;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
         }
        if (_flag3000ms)
          bipAlarme();
      }
      break;

    case mMENU_FERVURA:
      {
        if (_keyPress)
        {
          if (_KEY_SELECT)
          {
            passoMaquina = mEXECUTA_FERVURA;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        if (_flag3000ms)
          bipAlarme();
      }
      break;

    case mEXECUTA_FERVURA:
      {
        executaFervura();
        if (_keyPress)
        {
          if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
      }
      break;

    case mLOOP_FERVURA: {
        if (_keyPress)
        {
          if (_KEY_SELECT)
          {
            if (mediaTemperature >= DEFAULT_FERVURA)
            {
              bipAtencao();
              _contaTempo = true;
              _printDebug = true;
              wdtDadosBrassagem = 0;
            }
            break;
          }
          else if (_KEY_RIGHT)
          {
            if (receitaSel.qtdeWhirlpool == 0)
            {
              passoMaquina = mMENU_DESCANSO;
              break;
            }
            else
            {
              passoMaquina = mMENU_WHIRLPOOL;
              break;
            }
          }
          else if (_KEY_LEFT) {
            _contaTempo = false;
            tempoInicial = 0;
            tempoProximo = 0;
            wdtDadosBrassagem = 0;
            receitaSel = regReceita[idReceitaSel];
            receitaSel.statusBrassagem = 1;
            qtdePatamar = receitaSel.qtdePatamar;
            numPatamar = qtdePatamar - 1;
            passoMaquina = mLOOP_BRASSAGEM;
            break;
          }
          else if (_KEY_UP || _KEY_DOWN) {
            entradaPwmManual(F("% PWM"), valorPwmManual, 0, MAX_PWM);
            valorPwmManual = valorDigitado;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        loopProducao();
      }
      break;

    case mADICAO_FERVURA:
      {
        if (_keyPress)
        {
          if (_KEY_SELECT)
          {
            desligaLeds();
            numAdicao = numAdicao + 1;
            passoMaquina = mLOOP_FERVURA;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }

        if (_flag3000ms)
          bipAlarme();
        if (_flag1000ms) {
          _flag1000ms = false;
          tempoInicial++;
          tempoProximo++;
        }
      }
      break;

    case mMENU_WHIRLPOOL:
      {
        if (_keyPress)
        {
          if (_KEY_SELECT) {
            passoMaquina = mEXECUTA_WHIRLPOOL;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        if (_flag3000ms)
          bipAlarme();
      }
      break;

    case mEXECUTA_WHIRLPOOL:
      {
        if (_keyPress)
        {
          if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        executaWhirlpool();
      }
      break;

    case mLOOP_WHIRLPOOL:
      {
        if (_keyPress)
        {
          if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) //teste
          {
            entradaTempoManual(F("Tempo"), valorTempoManual, -500, 500);
            valorTempoManual = valorDigitado;

            if (_KEY_UP)
              tempoProximo = tempoProximo - 60;
            else if (_KEY_DOWN)
              tempoProximo = tempoProximo + 60;
            break;
          }//teste

          else if (_KEY_RIGHT)
          {
            tempoInicial = 0;
            wdtDadosBrassagem = 0;
            passoMaquina = mEXECUTA_DESCANSO;
            break;
          }
          else if (_KEY_LEFT) {
            receitaSel.statusBrassagem = 2;
            _contaTempo = false;
            tempoInicial = 0;
            tempoProximo = 0;
            wdtDadosBrassagem = 0;
            passoMaquina = mEXECUTA_FERVURA;
            break;
          }
          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        loopProducao();
      }
      break;

    case mMENU_DESCANSO:
      {
        if (_keyPress)
        {
          if (_KEY_SELECT)
          {
            passoMaquina = mEXECUTA_DESCANSO;
            break;
          }

          else if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        if (_flag3000ms)
          bipAlarme();
      }
      break;

    case mEXECUTA_DESCANSO:
      {
        if (_keyPress)
        {
          if (_KEY_ESC)
          {
            auxKeyEsc = true;
            passoMaquina = mLOOP_DESCANSO;
            break;
          }
        }
        executaDescanso();
      }
      break;

    case mLOOP_DESCANSO:
      {
        boolean auxFimProcesso = false;
        if (_keyPress || auxKeyEsc)
        {
          if (_KEY_RIGHT || auxKeyEsc)
          {
            _KEY_RIGHT = false;
            _KEY_ESC = true;
            auxFimProcesso = true;
          }
          else if (_KEY_LEFT)
          {
            _contaTempo = false;
            tempoInicial = 0;
            tempoProximo = 0;
            wdtDadosBrassagem = 0;

            if (receitaSel.qtdeWhirlpool == 0)
            {
              receitaSel.statusBrassagem = 2;
              passoMaquina = mMENU_FERVURA;
              break;
            }
            else
            {
              receitaSel.statusBrassagem = 3;
              passoMaquina = mEXECUTA_WHIRLPOOL;
              break;
            }
          }
          else if ((_KEY_UP || _KEY_DOWN) && contandoTempo()) //teste
          {
            if (_KEY_UP)
              tempoProximo = tempoProximo - 60;

            else if (_KEY_DOWN)
              tempoProximo = tempoProximo + 60;

            break;
          }
          if (_KEY_ESC || auxKeyEsc)
          {
            tempoInicial = 0;
            _derrubaMalte = false;
            receitaSel.statusBrassagem = RESET;
            qtdePatamar = receitaSel.qtdePatamar;
            for (register int ind = 0; ind < qtdePatamar; ind++) //Reseta todos os Patamares
              receitaSel.patamar[ind].concluido = false;
            salvaReceita(receitaSel);
            bipInicial();

            if (auxKeyEsc || _KEY_ESC)
              printDisplay(F(" Etapa Abortada "), F("  Saida Remota  "), TEMPO_ALARME);
            else if (auxFimProcesso)
              printDisplay(F("Fim do Processo"), F("Etapas Completas"), TEMPO_ALARME);
            executaBoasVindas();
            passoMaquina = mSTANDBY;
            auxKeyEsc = false;
            break;
          }
        }
        loopProducao();

      }
      break;

    case mCRIAR_RECEITA:
      criaReceita();
      break;

    case mNOME_RECEITA:
      {
        if (_keyPress && (stDigitado == F("") && stChar == F("_")))
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            stChar = F("_");
            numLetra = -1;
            _endString = false;
            passoMaquina = mMENU;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endString = true;
            _digitarValue = false;
          }
        }
        if(!_endString)
        	entradaString(stringDisplay, prReceita.nome);
        if (_endString)
        {
          _endString = false;
          nomeReceita = stDigitado;
          nomeReceita.toCharArray(prReceita.nome, MAX_NOME+1);
          stChar = F("_");
          
          boolean nomeExistente = false;
          for (register int ind = 0; ind < MAX_RECEITA; ind++) 
    	  {
    	  	Receita receita;
    		int ender = (ind * sizeof(receita));
    		EEPROM.get(ender, receita);
    
    		if (receita.codigo < 0)
      			break;

     		if(strcmp(prReceita.nome, receita.nome) == 0)
     		{
     			nomeExistente = true;
     			break;	
     		}
    	  }

    	  if(nomeExistente)
    	  {
    	  	bipAtencao();
    	  	printDisplay(F("Nome ja existe!"), F("Digite novamente"), TEMPO_CONF);
    	  	nomeReceita = F("");
          	nomeReceita.toCharArray(prReceita.nome, MAX_NOME+1);
    	  	break;
    	  }

          passoMaquina = mQT_PATAMAR;
          break;
        }
      }
      break;

    case mQT_PATAMAR:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            ind = 0;
            _digitarValue = false;
            _endValue = true;
          }
        }

        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.qtdePatamar,
                          MIN_PATAMAR, configGeral.qtdeMaxPatamar, false);
        if (_endValue)
        {
          _endValue = false;
          qtdePatamar = valorDigitado;
          prReceita.qtdePatamar = valorDigitado;
          ind = 0;
          passoMaquina = mTEMPMAX_PATAMAR;
          break;
        }
      }
      break;

    case mTEMPMAX_PATAMAR:
      {
        if (ind >= qtdePatamar)
        {
          ind = 0;
          _endValue = false;
          _digitarValue = false;
          passoMaquina = mTEMPMAX_FERVURA;
          break;
        }
        if (_keyPress)
        {
          resetDisplay();	
          if (_KEY_LEFT)
          {
            ind--;
            if (ind >= 0)
              passoMaquina++;
            else
            {
              _endValue = false;
              _digitarValue = false;
              ind = 0;
              passoMaquina = mQT_PATAMAR;
            }
            break;
          }

          else if (_KEY_RIGHT)
          {
            _digitarValue = false;
            _endValue = true;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.patamar[ind].temperaturaMax,
                          MIN_TEMPERATURA_PATAMAR, MAX_TEMPERATURA_PATAMAR, false);
        if (_endValue)
        {
          _endValue = false;
          prReceita.patamar[ind].temperaturaMax = valorDigitado;
          passoMaquina++;
          break;
        }
      }
      break;

    case mMINUTO_PATAMAR:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina = mTEMPMAX_PATAMAR;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.patamar[ind].tempo,
                          MIN_MINUTO_PATAMAR, MAX_MINUTO_PATAMAR, false);
        if (_endValue)
        {
          _endValue = false;
          prReceita.patamar[ind].tempo = valorDigitado;
          ind = ind + 1;
          passoMaquina--;
        }
      }
      break;

    case mTEMPMAX_FERVURA:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            ind = qtdePatamar - 1;
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
            //passoMaquina = mMAXMIN_FERVURA;
            //break;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.tempFervura,
                          MIN_TEMPERATURA_FERVURA, MAX_TEMPERATURA_FERVURA, false);
        if (_endValue) 
        {
          _endValue = false;
          tempFervura = valorDigitado;
          prReceita.tempFervura = valorDigitado;
          passoMaquina = mMAXMIN_FERVURA;
          break;
        }
      }
      break;

    case mMAXMIN_FERVURA:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.qtdeMinFervura,
                          MIN_MINUTO_FERVURA, MAX_MINUTO_FERVURA, false);
        if (_endValue) 
        {
          _endValue = false;
          qtdeMinFervura = valorDigitado;
          prReceita.qtdeMinFervura = valorDigitado;
          passoMaquina = mQT_ADICAO;
          break;
        }
      }
      break;

    case mQT_ADICAO:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            ind = 0;
            _endValue = true;
            _digitarValue = false;
            stChar = F("_");
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.qtdeAdicao,
                          MIN_ADICAO, MAX_ADICAO, false);
        if (_endValue) 
        {
          _endValue = false;
          qtdeAdicao = valorDigitado;
          prReceita.qtdeAdicao = valorDigitado;
          passoMaquina = mNOME_ADICAO;
          for(uint8_t i = 0; i < qtdeAdicao; i++)
          	prReceita.adicao[ind].qtde = DEFAULT_GRAMAS_ADICAO;
          qtdeMin = DEFAULT_MINUTO_ADICAO;
          break;
        }
      }
      break;

    case mNOME_ADICAO:
      {
        if (ind >= qtdeAdicao)
        {
          passoMaquina = mQT_WHIRLPOOL;
          _endValue = false;
          _digitarValue = false;
          ind = 0;
          break;
        }

        if (_keyPress && (stDigitado == F("") && stChar == F("_")))
        {
          if (_KEY_LEFT && stDigitado == F(""))
          {
            passoMaquina--;
            _endValue = false;
            _digitarValue = false;
            stChar = F("_");
            numLetra = -1;
            _endString = false;

            ind--;
            if (ind == -1)
            {
              ind = 0;
              passoMaquina = mQT_ADICAO;
              break;
            }
            else
            {
              passoMaquina = mMINUTOS_ADICAO;
              break;
            }
          }
          else if (_KEY_RIGHT)
          {
            _endString = true;
            _digitarValue = false;
          }

        }
        if(!_endString)
        	entradaString(stringDisplay, prReceita.adicao[ind].nome);
        if (_endString)
        {
          _endString = false;
          nomeAdicao = stDigitado;
          stChar = F("_");
          nomeAdicao.toCharArray(prReceita.adicao[ind].nome, MAX_NOME);
          passoMaquina++;
          break;
        }
      }
      break;

    case mGRAMAS_ADICAO:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            passoMaquina--;
            break;
          }
          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
          }
        }

        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.adicao[ind].qtde,
                          MIN_GRAMAS_ADICAO, MAX_GRAMAS_ADICAO, false);
        if (_endValue) {
          _endValue = false;
          prReceita.adicao[ind].qtde = valorDigitado;
          passoMaquina++;
          break;
        }
      }
      break;

    case mMINUTOS_ADICAO:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
            stChar = F("_");
          }
        }
        if(!_endValue)
        {
	        if (prOper == F("E"))
	          entradaValorGeral(stringDisplay, prReceita.adicao[ind].tempo,
	                            MIN_MINUTOS_ADICAO, qtdeMin, false);
	        else 
	        {
	          if (ind == 0)
	            entradaValorGeral(stringDisplay, qtdeMin,
	                              MIN_MINUTOS_ADICAO, prReceita.qtdeMinFervura, false);
	          else
	            entradaValorGeral(stringDisplay, qtdeMin,
	                              MIN_MINUTOS_ADICAO, prReceita.adicao[ind - 1].tempo, false);
	        }
    	}
        if (_endValue)
        {
          _endValue = false;
          prReceita.adicao[ind].tempo = valorDigitado;
          qtdeMin = prReceita.adicao[ind].tempo - 1;
          ind = ind + 1;
          passoMaquina = mNOME_ADICAO;
        }
      }
      break;

    case mQT_WHIRLPOOL:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            qtdeMin = DEFAULT_MINUTO_ADICAO;
            if(prReceita.qtdeAdicao == 0)
            	passoMaquina = mQT_ADICAO;
            else
            	passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
            stChar = F("_");
            numLetra = -1;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.qtdeWhirlpool,
                          MIN_WHIRLPOOL, MAX_WHIRLPOOL, false);
        //qtdeMinWhirlpool = 0;
        if (_endValue) 
        {   
          _endString = false;
          _endValue = false;
          qtdeWhirlpool = valorDigitado;
          prReceita.qtdeWhirlpool = valorDigitado;
          passoMaquina = mNOME_WHIRLPOOL;
          ind = 0;
          break;
        }
      }
      break;

    case mNOME_WHIRLPOOL:
      {
        if (ind >= qtdeWhirlpool)
        {
          passoMaquina = mQT_DESCANSO;
          ind = 0;
          break;
        }
        if (_keyPress && (stDigitado == F("") && stChar == F("_")))
        {
          if (_KEY_LEFT)
          {
            if (ind == 0)
              passoMaquina = mQT_WHIRLPOOL;
            else
            {
              ind--;

              if (ind == 0)
                passoMaquina++;
            }
            break;
          }

          else if (_KEY_RIGHT)
          {
            numLetra = -1;
            _endString = true;
          }
        }

        if (prOper == F("E"))
          	nomeWhirlpool = prReceita.whirlpool[ind].nome;
	    
	    else if(prOper == F("I"))
	    {
	        if (ind == 0)
	          nomeWhirlpool = F("LENTO");
	        else if (ind == 1)
	          nomeWhirlpool = F("RAPIDO");
	    }

        if(!_endString)
        	entradaString(stringDisplay, nomeWhirlpool);
    	
        if (_endString) 
        {
          _endString = false;
          nomeWhirlpool = stDigitado;
          stChar = F("_");
          nomeWhirlpool.toCharArray(prReceita.whirlpool[ind].nome, MAX_NOME);
          passoMaquina++;
          break;
        }
      }
      break;

    case mMINUTO_WHIRLPOOL:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.whirlpool[ind].tempo,
                          MIN_MINUTOS_WHIRLPOOL, MAX_MINUTO_WHIRLPOOL, false);
        if (_endValue) 
        {
          _endValue = false;
          prReceita.whirlpool[ind].tempo = valorDigitado;
          qtdeMinWhirlpool += prReceita.whirlpool[ind].tempo;
          stDigitado = F("");
          ind = ind + 1;
          passoMaquina--;
        }
      }
      break;

    case mQT_DESCANSO:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            ind = qtdeWhirlpool - 1;
            _endValue = false;
            _digitarValue = false;
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.qtdeDescanso,
                          MIN_DESCANSO, MAX_DESCANSO, false);
        //qtdeMinDescanso = 0;
        if (_endValue) {
          _endValue = false;
          qtdeDescanso = valorDigitado;
          prReceita.qtdeDescanso = valorDigitado;
          passoMaquina = mNOME_DESCANSO;
          ind = 0;
          break;
        }
      }
      break;

    case mNOME_DESCANSO:
      {
        if (ind >= qtdeDescanso)
        {
          passoMaquina = mSALVA_RECEITA;
          ind = 0;
          break;
        }
        if (_keyPress && (stDigitado == F("") && stChar == F("_")))
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            passoMaquina--;
            break;
          }

          else if (_KEY_RIGHT)
          {
            _endValue = false;
            _digitarValue = false;
            stChar = F("_");
            numLetra = -1;
            _endString = true;
          }
        }

    	
    	if (prOper == F("E"))
	        nomeDescanso = prReceita.descanso[ind].nome;
	    else if(prOper == F("I"))
	    {
	        if (ind == 0)
	        nomeDescanso = F("DESCANSO");
	    }

        if(!_endString) 
	    	entradaString(stringDisplay, nomeDescanso);

        if (_endString) 
        {
          _endString = false;
          nomeDescanso = stDigitado;
          stChar = F("_");
          nomeDescanso.toCharArray(prReceita.descanso[ind].nome, MAX_NOME);
          passoMaquina++;
          break;
        }
      }
      break;

    case mMINUTO_DESCANSO:
      {
        if (_keyPress)
        {
          resetDisplay();
          if (_KEY_LEFT)
          {
            passoMaquina--;
            break;
          }
          else if (_KEY_RIGHT)
          {
            _endValue = true;
            _digitarValue = false;
          }
        }
        if(!_endValue)
        entradaValorGeral(stringDisplay, prReceita.descanso[ind].tempo,
                          MIN_MINUTOS_DESCANSO, MAX_MINUTO_DESCANSO, false);
        if (_endValue) {
          _endValue = false;
          prReceita.descanso[ind].tempo = valorDigitado;
          qtdeMinDescanso += prReceita.descanso[ind].tempo;
          stDigitado = F("");
          ind = ind + 1;
          passoMaquina--;
        }
      }
      break;

    case mSALVA_RECEITA:
      {
        nomeReceita.toCharArray(prReceita.nome, MAX_NOME+1);
        prReceita.qtdePatamar = qtdePatamar;
        prReceita.qtdeAdicao = qtdeAdicao;
        prReceita.qtdeMinFervura = qtdeMinFervura;
        prReceita.tempFervura = tempFervura;
        prReceita.qtdeWhirlpool = qtdeWhirlpool;
        prReceita.qtdeMinWhirlpool = qtdeMinWhirlpool;
        if (prOper == F("I"))
        {
          prReceita.codigo = qtdeReceita;
          qtdeReceita = qtdeReceita + 1;
          totalDeReceitas++;
        }
        regReceita[prReceita.codigo] = prReceita;
        salvaReceita(prReceita);
        bipAcerto();
        printDisplay(F(" Receita Salva "), F("  Com Sucesso!  "), TEMPO_CONF);
        
        wdt3000ms = 0;
		_msgConvite = true;
		_msgEzbrew = false;

        if (code = F("A1")) passoMaquina = mSTANDBY;
        else passoMaquina = mMENU;
        break;
      }
      break;

    case mADICIONAR_AGUA:
      adicionarAgua();
      break;

    case mCALCULA_FLUXO:
      calculaFluxo(volumeDigitado);
      break;

    case mSENHA_CONFIGURACAO: 
    {
        stringDisplay = F("--MENU CONFIG.--\n Digite a SENHA \np/ o menu config");
        entradaString(stringDisplay, "");
        if (_endString) {
          _endString = false;
          stringSenha = stDigitado;
          stChar = F("_");
          if (stringSenha == String(SENHA)) {
            bipAcerto();
            passoMaquina = mMENU_CONFIGURACAO;
            break;
          }
          else {
            stringSenha = F("");
            bipAtencao();
            printDisplay(F("     ERRO     "), F(" Senha Invalida "), TEMPO_ALARME);
            passoMaquina = mMENU;
            break;
          }
        }
      }
      break;

    case mMENU_CONFIGURACAO: 
    {
        uint8_t opcMenu = executaMenu(6);
        
        if (_analisaOpcMenu) {
          _analisaOpcMenu = false;
          if (opcMenu == 0) {
            passoMaquina = mMENU;
            break;
          }
          else if (opcMenu == 1) {
            passoMaquina = mSET_MAX_PATAMAR;
            break;
          }
          else if (opcMenu == 2) {
            passoMaquina = mSET_GRAU_MINUTO;
            break;
          }
          else if (opcMenu == 3) {
            passoMaquina = mSET_PWM_SETPOINT;
            break;
          }
          else if (opcMenu == 4) {
            passoMaquina = mSET_DATA_HORA;
            break;
          }
          else if (opcMenu == 5) {
            passoMaquina = mLIMPA_EEPROM;
            break;
          }
        }
      }
      break;

    case mSET_MAX_PATAMAR:
      {
        if (_keyPress)
        {
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina = mMENU_CONFIGURACAO;
            break;
          }
        }
        entradaValorGeral(F("Defina o maximo\n  de patamares"), configGeral.qtdeMaxPatamar, 1, MAX_PATAMAR, false);
        if (_endValue) {
          _endValue = false;
          configGeral.qtdeMaxPatamar = valorDigitado;
          passoMaquina = mMENU_CONFIGURACAO;
          salvaConfigGeral();
          break;
        }
      }
      break;

    case mSET_GRAU_MINUTO:
      {
        if (_keyPress)
        {
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina = mMENU_CONFIGURACAO;
            break;
          }
        }

        entradaValorGeral(F("Escolha valor de\ngraus por minuto"), configGeral.grauPorMinuto, 0, 2, true);
        if (_endValue) {
          _endValue = false;
          configGeral.grauPorMinuto = valorDigitado;
          passoMaquina = mMENU_CONFIGURACAO;
          salvaConfigGeral();
          break;
        }
      }
      break;

    case mSET_PWM_SETPOINT:
      {
        if (_keyPress)
        {
          if (_KEY_LEFT)
          {
            _endValue = false;
            _digitarValue = false;
            passoMaquina = mMENU_CONFIGURACAO;
            break;
          }
        }
        entradaValorGeral(F("Escolha o valor\nde PWM SetPoint"), configGeral.pwmSetpoint, 10, 100, false);
        if (_endValue) {
          _endValue = false;
          configGeral.pwmSetpoint = valorDigitado;
          passoMaquina = mMENU_CONFIGURACAO;
          salvaConfigGeral();
          break;
        }
      }
      break;

    case mSET_DATA_HORA:
    {
		if(!_flagAjusteDataHora)
    		opcMenuDataHora = executaMenu(8);

    	if (_keyPress && !_flagAjusteDataHora)
      	{
	        if (_KEY_LEFT)
	        {
	          passoMaquina = mMENU_CONFIGURACAO;
	          break;
	        }

	        if (_analisaOpcMenu) 
	        {
	          _analisaOpcMenu = false;
	          _flagAjusteDataHora = true;
	          printConfigDataHora();
	        }
      	}

        else if (_flagAjusteDataHora)
      	{
      		if (_keyPress)
        	{
                if (_KEY_LEFT)
                {
                    _endValue = false;
	          		_digitarValue = false;
	          		opcMenuDataHora = 0;
	          		_flagAjusteDataHora = false;
	          		break;
                }
            }
            
	        if(opcMenuDataHora == 0)
	        {
	            _endValue = false;
	          	_digitarValue = false;
	          	opcMenuDataHora = 0;
	          	_flagAjusteDataHora = false;
	          	passoMaquina = mMENU_CONFIGURACAO;
	            break;
	        }

	        else if (opcMenuDataHora == 1) // Ajuste da HORA
	        {
	          byte auxHoraAtual = getClock(F("hrs"));
	          entradaValorGeral(stringDisplay, auxHoraAtual, 0, 23, false);
	        }

	        else if (opcMenuDataHora == 2) // Ajuste do MINUTO
	        {
	          byte auxMinAtual = getClock(F("min"));
	          entradaValorGeral(stringDisplay, auxMinAtual, 0, 59, false);
	        }

	        else if (opcMenuDataHora == 3) // Ajuste do SEGUNDO
	        {
	          byte auxSecAtual = getClock(F("sec"));
	          entradaValorGeral(stringDisplay, auxSecAtual, 0, 59, false);
	        }

	        else if (opcMenuDataHora == 4) // Ajuste do dia da semana
	        {
	          int auxDiaDaSemanaAtual = getClock(F("diaSem"));
	          entradaValorGeral(stringDisplay, auxDiaDaSemanaAtual, 1, 7, false);
	        }

	        else if (opcMenuDataHora == 5) // Ajuste do dias do mês
	        {
	          int auxDiaDoMesAtual = getClock(F("diaMes"));
	          entradaValorGeral(stringDisplay, auxDiaDoMesAtual, 1, 31, false);
	        }

	        else if (opcMenuDataHora == 6) // Ajuste do mes
	        {
	          int auxMesAtual = getClock(F("mes"));
	          entradaValorGeral(stringDisplay, auxMesAtual, 1, 12, false);
	        }

	        else if (opcMenuDataHora == 7) // Ajuste do ano
	        {
	          int auxAnoAtual = getClock(F("ano"));
	          entradaValorGeral(stringDisplay, auxAnoAtual, 0, 99, false);
	        }

	        if (_endValue)
	        {
	          _endValue = false;

	          if (opcMenuDataHora == 1)
	          {
	            byte auxHora = valorDigitado;
	            setClock(F("hrs"), auxHora);
	          }

	          else if (opcMenuDataHora == 2)
	          {
	            byte auxMinuto = valorDigitado;
	            setClock(F("min"), auxMinuto);
	          }

	          else if (opcMenuDataHora == 3)
	          {
	            byte auxSegundo = valorDigitado;
	            setClock(F("sec"), auxSegundo);
	          }

	          else if (opcMenuDataHora == 4)
	          {
	            byte auxDiaDaSemana = valorDigitado;
	            setClock(F("diaSem"), auxDiaDaSemana);
	          }

	          else if (opcMenuDataHora == 5)
	          {
	            byte auxDiaDoMes = valorDigitado;
	            setClock(F("diaMes"), auxDiaDoMes);
	          }

	          else if (opcMenuDataHora == 6)
	          {
	            byte auxMes = valorDigitado;
	            setClock(F("mes"), auxMes);
	          }

	          else if (opcMenuDataHora == 7)
	          {
	            byte auxAno = valorDigitado;
	            setClock(F("ano"), auxAno);
	          }

	          _flagAjusteDataHora = false;
	          opcMenuDataHora = 0;
	          bipOK();
  			  printDisplay(F("Configuracao"), F("Salva c/ sucesso"), TEMPO_CONF);
          	  break;
	      	}
	    }
      }
      break;

    case mLIMPA_EEPROM:
      limpaEeprom();
      break;

    default:
      break;

  }
  clearKeyPad();

  if(passoAnterior != passoMaquina)
  	enviaTela();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : machineDisplay
// Data     : 15/11/2007 09:55
//WWWWWWWWWW*********************************************************************************

void machineDisplay() 
{
  if (!_atendeDisplay)
    return;
  _atendeDisplay = false;
  
  passoDisplay = passoMaquina;

  switch (passoDisplay) {

    case mVERSAO:
      printVersao();
      break;

    case mSTANDBY:
      printBoasVindas();
      break;

    case mMENU:
      printMenuGlobal(4);
      break;

    case mCALCULA_FLUXO:
      printFluxo();
      break;

    case mRECEITAS:
      printMenuGlobal(qtdeReceita + 1);
      break;

    case mRECEITASEL:
      printMenuGlobal(4);
      break;

    case mMENU_CONFIGURACAO:
      printMenuGlobal(6);
      break;

    case mSET_DATA_HORA:
      if(!_flagAjusteDataHora)
      	printMenuGlobal(8);
      break;

    case mNOME_RECEITA:
      printNomeReceita();
      break;

    case mQT_PATAMAR:
      printQtPatamar();
      break;

    case mTEMPMAX_PATAMAR:
      printTempPatamar();
      break;

    case mMINUTO_PATAMAR:
      printMinutoPatamar();
      break;

    case mTEMPMAX_FERVURA:
      printTempFervura();
      break;

    case mMAXMIN_FERVURA:
      printMinutoFervura();
      break;

    case mQT_ADICAO:
      printQtAdicao();
      break;

    case mNOME_ADICAO:
      printNomeAdicao();
      break;

    case mGRAMAS_ADICAO:
      printGramasAdicao();
      break;

    case mMINUTOS_ADICAO:
      printMinutoAdicao();
      break;

    case mQT_WHIRLPOOL:
      printQtWhirlpool();
      break;

    case mNOME_WHIRLPOOL:
      printNomeWhirlpool();
      break;

    case mMINUTO_WHIRLPOOL:
      printMinutoWhirlpool();
      break;

    case mQT_DESCANSO:
      printQtDescanso();
      break;

    case mNOME_DESCANSO:
      printNomeDescanso();
      break;

    case mMINUTO_DESCANSO:
      printMinutoDescanso();
      break;

    case mDERRUBAR_MALTE:
      printMenuMalte();
      break;

    case mMENU_FERVURA:
      printMenuFervura();
      break;

    case mADICAO_FERVURA:
      printAdicao();
      break;

    case mMENU_WHIRLPOOL:
      printMenuWhirlpool();
      break;

    case mMENU_DESCANSO:
      printMenuDescanso();
      break;

    default:
      break;
  }
  
  passoAnterior = passoMaquina;
 
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : setup
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setup() 
{
  startSerial();
  
  startEEPROM();
  clearEeprom();

  startPorts();
  startI2c();
  startOled();
  startAds1115();

  carregaReceitas();

  leConfigGeral();

  startTimers();
  bipInicial();
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : loop
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void loop() 
{
  keyPadInterruption();
  //getTemperatura();
  stateMachine();
  machineDisplay();
  
  //serialReceiver();
  //analyzesDataSerial();
  
  /*interruptButton();
  interruptTireButton();
  interruptCleanerButton();*/
}

/*
  UPDATES EZBREW À REALIZAR:
  1) Deixar DEFAULT: "Patamar 1", "Patamar 2". Porém, os demais para colocar o nome que quiser: 13 caracteres;
  2) Adicionar o resto do protocolo "machineBluetooth";
  3) Conferir o modo: "deseja continuar?" (S/N);
*/

// =================================================================================( * FIM * )
// =======================================================================( * FIM * )
// =============================================================( * FIM * )
// ===================================================( * FIM * )
// =========================================( * FIM * )
// ===============================( * FIM * )
// =====================( * FIM * )
// ===========( * FIM * )
// =( * FIM * )

// =================================================================================( * FIM * )
// =======================================================================( * FIM * )
// =============================================================( * FIM * )
// ===================================================( * FIM * )
// =========================================( * FIM * )
// ===============================( * FIM * )
// =====================( * FIM * )
// ===========( * FIM * )
// =( * FIM * )
