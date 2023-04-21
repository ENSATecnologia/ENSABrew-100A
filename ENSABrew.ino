//*******************************************************************************************
// Arquivo C/C++ do Projeto Controlador de Patamares -- ENSABrewControl
//-------------------------------------------------------------------------------------------
//
// Arquivo: ENSABrewMain.ino
//
//
// Autor  : Jânio Anselmo, Eng. Me.
// Autor  : Thiago Anselmo, Tecgº
// Autor  : Gabriel da Silva Caetano
//
// CPU    : ATMega2561 - MegaCore
// Clock  : 16MHz
//
//*******************************************************************************************

#include "ENSABrewLibraries/Wire/Wire.cpp"
#include "ENSABrewLibraries/EEPROM/EEPROM.h"
#include "ENSABrewLibraries/TimerOne/TimerOne.cpp"
#include "ENSABrewLibraries/Arduino_PID/PID_v1.cpp"
#include "ENSABrewLibraries/TimerFour/TimerFour.cpp"
#include "ENSABrewLibraries/ArduinoJson/ArduinoJson.h"
#include "ENSABrewLibraries/TimerThree/TimerThree.cpp"
#include "ENSABrewLibraries/Thermistor/Thermistor.cpp"

// ------------------------------------------------------------------------------------------
// VERSÃO DO FIRMWARE DO SISTEMA - ENSABrewControl
// ------------------------------------------------------------------------------------------

#define SISTEMA F(" ENSABrewControl")
#define VERSAO  F("  ver 1.0.00    ")

#define ERASE_EEPROM_USER        0        // 1 = Clear ALL EEPROM - USER
#define ERASE_EEPROM_CONFIG      0        // 1 = Clear ALL EEPROM - CONFIG

#define DEBUG_SERIAL             1        // 1 = Habilita o DEBUG pela Serial0
#define DEBUG_SOCKET             0        // 1 = Habilita o DEBUG pelo Socket (ETH0)

#define SENHA_MASTER              F("4E") // Definição da senha de configuração - MASTER
#define SENHA_USER                F("1A") // Definição da senha de configuração - USER

#define RESET                    0        // definição de RESET

// ------------------------------------------------------------------------------------------
// DEFINIÇÕESS DOS PINOS DO MICROCONTROLADOR - ATMEGA 2561
// ------------------------------------------------------------------------------------------

#define PINO_TERMISTOR           1        // Pino onde o Termistor NTC 10k deve ser conectado - PINOS 2 e 5 (cabo ENSABrew)
#define PINO_TECLADO             0        // Pino onde o KeyPad estão conectado - AD(0)
#define PINO_SSR                 5        // Pino de acionamento aquecedor (SSR) - HIGH -> ACENDE, LOW -> DESLIGA
#define PINO_RELAY1              22       // Pino Relé - LOW -> DESLIGA, HIGH -> ACENDE
#define PINO_SOLENOIDE1          34       // Pino Solenóide 1 - LOW -> DESLIGA, HIGH -> ACENDE
#define PINO_SOLENOIDE2          35       // Pino Solenóide 2 - LOW -> DESLIGA, HIGH -> ACENDE
#define PINO_NIVEL1              6        // Pino Sensor de Nível 1 - LOW -> DESLIGA, HIGH -> ACENDE
#define PINO_NIVEL2              7        // Pino Sensor de Nível 2 - LOW -> DESLIGA, HIGH -> ACENDE
#define PINO_FLUXO               19       // Pino Sensor Fluxo - INTERRUPÇÂO/INT1
#define PINO_AC                  18       // Pino de detecção de rede (AC) - INTERRUPÇÂO/INT0
#define PINO_BUZZER              4        // Pino do BUZZER/BEEP
#define LED_AMARELO              23       // Pino do LED AMARELO - HIGH -> ACENDE, LOW -> DESLIGA
#define LED_VERMELHO1            24       // Pino do LED VERMELHO 1 - HIGH -> ACENDE, LOW -> DESLIGA
#define LED_VERMELHO2            25       // Pino do LED VERMELHO 2 - HIGH -> ACENDE, LOW -> DESLIGA

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES GERAIS DO MICROCONTROLADOR - ATMEGA 2561
// ------------------------------------------------------------------------------------------

// Se esta macro estiver definida, as funções do relógio estarão habilitadas
// #define FUNCTIONS_CLOCK   

#define AD_DEBOUNCE              5        // AD Debounce do teclado

#define BUZZER_GERAL             2        // Número de vezes que soará o BIP na INTRODUCAO do sistema
#define BUZZER_SALVAR            2        // Número de vezes que soará o BIP ao SALVAR
#define BUZZER_ERRO              2        // Número de vezes que soará o BIP no ERRO do sistema
#define BUZZER_ALARME            1        // Número de vezes que soará o BIP no ALARME do sistema
#define BUZZER_PATAMAR           1        // Número de vezes que soará o BIP na chegada do PATAMAR
#define BUZZER_MOSTURA           4        // Número de vezes que soará o BIP ao final da MOSTURA
#define BUZZER_ADICOES           2        // Número de vezes que soará o BIP nas ADIÇÕES
#define BUZZER_WHIRLPOOL         2        // Número de vezes que soará o BIP nos WHIRLPOOL
#define BUZZER_PERIODO           10       // Período do BUZZER da "mBUZZER"

#define DELAY_DEBOUNCE           5        // Delay Debounce do teclado
#define DELAY_EEPROM             5        // Delay Write/Read da EEPROM; Máximo de 3.3ms - DataSheet
#define DELAY_SALVAR             250      // Delay para SALVAR na EEPROM 
#define DELAY_GERAL              50       // Delay para tela GERAL
#define DELAY_ERRO               500      // Delay para tela de ERRO
#define DELAY_ALARME             500      // Delay para tela de ALARME

#define TIMER1                   1000     // TIMER1 (us) - 1ms - "globalInterruption();"
#define TIMER3                   3333     // TIMER3 (us) - 3,3ms/300Hz - "PMW - SSR". Em "ENSABrewENSAv10.4" o TIMER3 = 1000000
#define TIMER4                   10000    // TIMER4 (us) - 10ms - "machineBuzzer();"

#define AMOSTRAS_TECLADO         4        // Número de amostras do AD do teclado
#define MAX_AD_TECLADO           1010     // Valor máximo do AD para considerar "tecla solta"
#define BOUNCE_AD                5        // Número de bounce do AD do teclado
#define BOUNCE_UP                30       // Número de bounce do teclado (antes da leitura)
#define F_TONE                   3500     // Frequencia do BUZZER
#define TEMPO_ALARME             2000     // Tempo (ms) de impressão das mensagens de alarme - LCD
#define TEMPO_CONF               1000     // Tempo (ms) de impressão das mensagens de configuração - LCD
#define TEMPO_PWM                180      // Tempo (ms) para aguardar o PWM atingir o PATAMAR

#define RAMPA                    1        // RAMPA "statusFase" - pré-fervura
#define PATAMAR                  2        // PATAMAR "statusFase" - pré-fervura
#define FIM_MOSTURA              3        // Passo final da mostura "statusFase" - antes da FERVURA
#define FERVURA                  4        // Fervura "statusFase" - pós-fervura
#define WHIRLPOOL                5        // WHIRLPOOL "statusFase" - pós-fervura
#define DESCANSO                 6        // DESCANSO "statusFase" - pós-fervura
#define FINAL                    7        // FINAL do processo "statusFase" - pós-fervura

#define KEY_RIGHT                0        // Tecla pra DIREITA
#define KEY_UP                   1        // Tecla pra CIMA
#define KEY_DOWN                 2        // Tecla pra BAIXO
#define KEY_LEFT                 3        // Tecla pra ESQUERDA
#define KEY_SELECT               4        // Tecla ENTER para confirmação
#define KEY_ESC                  5        // Tecla para SAIR
#define KEY_NONE                 6        // Sem tecla Pressionada

#define V_KEY_UP                 F("UP")     // Tecla VIRTUAL pra CIMA
#define V_KEY_DOWN               F("DOWN")   // Tecla VIRTUAL pra BAIXO
#define V_KEY_RIGHT              F("RIGHT")  // Tecla VIRTUAL pra DIREITA
#define V_KEY_LEFT               F("LEFT")   // Tecla VIRTUAL pra ESQUERDA
#define V_KEY_SELECT             F("ENTER")  // Tecla VIRTUAL ENTER para confirmação
#define V_KEY_ESC                F("ESC")    // Tecla VIRTUAL para SAIR

#define MIN_PATAMAR              1        // Mínimo número de PATAMARES desejado
#define MIN_TEMPERATURA_PATAMAR  1        // Temperatura máxima desejada no PATAMAR
#define MIN_TEMPERATURA_FERVURA  25       // Temperatura máxima desejada na FERVURA
#define MIN_MINUTO_PATAMAR       1        // Tempo mínimo desejado no PATAMAR
#define MIN_MINUTO_FERVURA       5        // Tempo mínimo desejado na FERVURA
#define MIN_GRAMAS_ADICAO        1        // Gramas mínimas de adição de levedura

#define DEFAULT_MINUTO_PATAMAR   5        // DEFAULT MINUTO PATAMAR
#define DEFAULT_QT_PATAMAR       2        // DEFAULT QT PATAMAR
#define DEFAULT_TEMP_PATAMAR     25       // DEFAULT TEMPERATURA PATAMAR
#define DEFAULT_ADICAO           1        // DEFAULT número de ADIÇÕES
#define DEFAULT_MINUTO_ADICAO    15       // DEFAULT MINUTO de ADICAO
#define DEFAULT_GRAMAS_ADICAO    10       // DEFAULT GRAMAS de ADIÇÕES
#define DEFAULT_MINUTO_FERVURA   60       // DEFAULT MINUTO de FERVURA
#define DEFAULT_TEMP_FERVURA     100      // DEFAULT TEMPERATURA de FERVURA
#define DEFAULT_WHIRLPOOL        2        // DEFAULT número de WHIRLPOOL
#define DEFAULT_MINUTO_WHIRLPOOL 10       // DEFAULT MINUTO de WHIRLPOOL
#define DEFAULT_DESCANSO         1        // DEFAULT número de DESCANSO
#define DEFAULT_MINUTO_DESCANSO  10       // DEFAULT MINUTO de DESCANSO

#define MAX_PATAMAR              10       // máximo número de PATAMARES desejado
#define MAX_TEMPERATURA_PATAMAR  100      // Temperatura máxima desejada no PATAMAR
#define MAX_TEMPERATURA_FERVURA  200      // Temperatura máxima desejada na FERVURA
#define MAX_MINUTO_PATAMAR       300      // Tempo máximo desejado no PATAMAR
#define MAX_MINUTO_FERVURA       500      // Tempo máximo desejado na FERVURA
#define MAX_GRAMAS_ADICAO        500      // Gramas máximas de adição de levedura
#define MAX_MINUTOS_WHIRLPOOL     60      // Tempo máximo desejado no WHIRLPOOL
#define MAX_MINUTOS_DESCANSO      60      // Tempo máximo desejado no DESCANSO

#define MAX_PWM                  100      // PWM máximo (em percentual)
#define MAX_SETPOINT             120      // SetPoint máximo
#define MIN_SETPOINT             -120     // SetPoint mínimo

#define MAX_LITROS				 999	  // Quantidade máxima de litros
#define MAX_CAL_FACTOR			 10	      // Valor máxim do fator de calibração

#define DEFAULT_MAX_PATAMAR      7        // máximo número default de PATAMARES desejado
#define DEFAULT_GRAU_MINUTO      1        // grau por minuto desejado
#define DEFAULT_PWM              95       // PWM default em percentual
#define DEFAULT_FERVURA          95       // Temperatura limite para iniciar tempo fervura

#define MIN_ADICAO               0        // MÍNIMO numeros de ADIÇÕES
#define MIN_MINUTOS_ADICAO       1        // MÍNIMO minutos de ADIÇÕES
#define MIN_WHIRLPOOL            0        // MÍNIMO numeros de WHIRLPOOL
#define MIN_MINUTOS_WHIRLPOOL    1        // MÍNIMO minutos de WHIRLPOOL
#define MIN_DESCANSO             0        // MÍNIMO numeros de DESCANSO
#define MIN_MINUTOS_DESCANSO     1        // MÍNIMO minutos de DESCANSO

#define MAX_RECEITA              5        // MAXIMA RECEITA
#define MAX_NOME                 12       // MAXIMO NOMES (receita = 12)
#define MAX_ALFABETO             36       // MAXIMA letras do alfabeto
#define MAX_NUMEROS              10       // MAXIMA números
#define MAX_ADICAO               5        // MAXIMA numeros de ADIÇÕES
#define MAX_WHIRLPOOL            2        // MAXIMA numeros de WHIRLPOOL
#define MAX_DESCANSO             1        // MAXIMA numeros de DESCANSO
#define END_CONFIG               3800     // Endereço máximo da memória

#define _CRIAR_RECEITA           0		  // Sinaliza criação de receita
#define _EDITAR_RECEITA			 1		  // Sinaliza edição de receita

#define ST_RESP_NAO				 F("Nao") // Resposta: NÃO
#define ST_RESP_SIM				 F("Sim") // Resposta: SIM

#define ADDRESS_DS1307           0x68     // Endereço do RTC
#define NETEEPROM_END            115      // END address EEPROM configuration Bootloader - 
										  // 114 bytes USED (AVR Etherboot Open Escape Control v1.4)

#define SERVER_PORT              5000     // Porta de conexão TCP/IP - Server ENSA Tecnologia
#define ETH_CS                   10       // Select the Ethernet Module.
#define SD_CS                    4        // De-Select the internal SD Card
#define TIMEOUT_SOCKET           3        // Timeout connection socketENSA 
#define SERIAL_BAUD_RATE         115200   // Baud Rate Serial (DEBUG) e Serial1 (Bluetooth)
#define DATA_UTIL_SERIAL         512      // Tamanho máximo do array "DATA_UTIL_SERIAL"

#define MAX_SAMPLE_TEMPERATURE   30       // Máxima amostra de temperatura - 30
#define DEFAULT_SAMPLE_TEMP		 15		  // Quantidade default de amostras

#define TIME_READ_TEMPERATURE    10		  // (*50ms)
#define TIME_SEND_TEMPERATURE 	 15       // (*1sec), se definir como 0, nunca enviará

#define INIT_TIME_INCREMENT 	 25 //15  // *50ms

#define INTERNAL                 false    // Comandos internos do dispositivo
#define REMOTE                   true     // Comandos remotos

#define _AUTO_ADJUSTMENT 		 0		  // Sinaliza ajuste automático do fator de calibração
#define _MANUAL_ADJUSTMENT  	 1		  // Sinaliza ajuste manual do fator de calibração

#define ARDUINOJSON_ENABLE_PROGMEM    1   // Enable "ARDUINOJSON_ENABLE_PROGMEM"

#define DEFAULT_ID_MODULE		 F("1020ABC01") // Número de série/id padrão

#define _CONFIG_MENU			 0 // Sinaliza configuração por MENU
#define _CONFIG_SOCKET			 1 // Sinaliza configuração por SOCKET

#define _DISABLE				 0 // Sinaliza algo desabilitado
#define _ENABLE				 	 1 // Sinaliza algo habilitado

// Passos da BRASSAGEM:
enum passosBrassagem
{
	_STOP = 0,		// 0 = Parada
	_PRODUCAO,		// 1 = Em produção	 	 
	_FERVURA,		// 2 = Fervura			 
	_WHIRLPOOL,		// 3 = Whirlpool		 	 
	_DESCANSO, 		// 4 = Descanso 		 	 	
};

// ------------------------------------------------------------------------------------------
// PASSOS DA MAQUINA DE ESTADOS - FUNÇÕES E DISPLAY
// ------------------------------------------------------------------------------------------

enum passosMaquina
{
	mVERSAO = 0        ,	  // Passo inicial: mostra VERSAO do SISTEMA
	mSTANDBY           ,      // Passo TELA BOAS VINDAS da máquina de estado
	
	mMENU              ,      // Passo MENU da máquina de estado
	
	mRECEITAS          ,      // Passo MENU DE RECEITAS da máquina de estado
	mRECEITASEL        ,      // Passo RECEITA SELECIONADA da máquina de estado
	mAPAGA_RECEITA     ,      // Passo APAGA RECEITA SELECIONADA da máquina de estado
	mCONTINUA_PRODUCAO ,      // Passo CONTINUA PRODUCAO da máquina de estado
	mVISUALIZA_RECEITA ,	  // Passo VISUALIZA RECEITA da máquina de estado
	
	mEXECUTA_PRODUCAO  ,      // Passo EXECUTA PRODUCAO da máquina de estado
	mDERRUBAR_MALTE    ,      // Passo DERRUBAR MALTE da máquina de estado
	mLOOP_BRASSAGEM    ,      // Passo FASE BRASSAGEM da máquina de estado
	mMENU_FERVURA      ,      // Passo MENU FERVURA da máquina de estado
	mEXECUTA_FERVURA   ,      // Passo EXECUTA FERVURA da máquina de estado
	mLOOP_FERVURA      ,      // Passo FASE FERVURA da máquina de estado
	mADICAO_FERVURA    ,      // Passo FASE FERVURA da máquina de estado
	mMENU_WHIRLPOOL    ,      // Passo MENU WHIRLPOOL da máquina de estado
	mEXECUTA_WHIRLPOOL ,      // Passo EXECUTA WHIRLPOOL da máquina de estado
	mLOOP_WHIRLPOOL    ,      // Passo FASE WHIRLPOOL da máquina de estado
	mMENU_DESCANSO     ,      // Passo FASE DESCANSO da máquina de estado
	mEXECUTA_DESCANSO  ,      // Passo EXECUTA DESCANSO da máquina de estado
	mLOOP_DESCANSO     ,      // Passo FASE DESCANSO da máquina de estado
	
	mCRIAR_RECEITA     ,      // Passo CRIAR RECEITA da máquina de estado
	mNOME_RECEITA      ,      // Passo MANTEM RECEITA da máquina de estado
	mQT_PATAMAR        ,      // Passo ENTRADA STRING da máquina de estado
	mTEMPMAX_PATAMAR   ,      // Passo TEMPERATURA MAXIMA PATAMAR da máquina de estado
	mMINUTO_PATAMAR    ,      // Passo MINUTO PATAMAR PATAMAR da máquina de estado
	mTEMPMAX_FERVURA   ,      // Passo TEMPERATURA TEMPERATURA DE FERVURA da máquina de estado
	mMAXMIN_FERVURA    ,      // Passo MAXIMO MINUTO DE FERVURA da máquina de estado
	mQT_ADICAO         ,      // Passo QUANTIDADE MAXIMA DE ADICAO da máquina de estado
	mNOME_ADICAO       ,      // Passo NOME DA ADICAO da máquina de estado 
	mGRAMAS_ADICAO     ,      // Passo GRAMAS DA ADICAO da máquina de estado 
	mMINUTOS_ADICAO    ,      // Passo MINUTOS DA ADICAO da máquina de estado
	mQT_WHIRLPOOL      ,      // Passo QUANTIDADE DE WHIRLPOOL da máquina de estado
	mMINUTO_WHIRLPOOL  ,      // Passo MINUTOS DE WHIRLPOOL da máquina de estado
	mQT_DESCANSO       ,      // Passo QUANTIDADE DE DESCANSO da máquina de estado
	mMINUTO_DESCANSO   ,      // Passo MINUTOS DE DESCANSO da máquina de estado
	mSALVA_RECEITA     ,      // Passo SALVA RECEITA da máquina de estado
	
	mADICIONAR_AGUA    ,      // Passo ADICIONAR AGUA da máquina de estado
	mCALCULA_FLUXO     ,      // Passo CALCULA FLUXO da máquina de estado
	
	mSENHA_CONFIGURACAO,      // Passo EXECUTA CONFIGURACAO da máquina de estado
	
	mMENU_CONFIGURACAO ,      // Passo MENU CONFIGURACAO da máquina de estado
	mMENU_USER_CONFIG  ,      // Passo MENU USER CONFIG da máquina de estado

	mSET_MAX_PATAMAR   ,      // Passo SET MAX PATAMAR da máquina de estado
	mSET_GRAU_MINUTO   ,      // Passo SET GRAU POR MINUTO da máquina de estado
	mSET_PWM_SETPOINT  ,      // Passo SET PWM SETPOINT da máquina de estado
	mSET_DATA_HORA     ,      // Passo SET DATA HORA da máquina de estado
	mSET_SAMPLE_TEMP   ,      // Passo mSET_SAMPLE_TEMP da máquina de estado
	
	mMENU_TEMPO_ENVIO  ,      // Passo mMENU_TEMPO_ENVIO da máquina de estado
	mSET_SEND_TIME_AC  ,      // Passo SET_SEND_TIME_AC da máquina de estado
	mSET_SEND_TIME_TEMP,      // Passo SET_SEND_TIME_TEMP da máquina de estado

	mMENU_CAL_FACTOR   ,      // Passo MENU_CAL_FACTOR da máquina de estado
	mAUTO_CAL_FACTOR   ,      // Passo AUTO_CAL_FACTOR da máquina de estado
	mMANUAL_CAL_FACTOR ,      // Passo MANUAL_CAL_FACTOR da máquina de estado
	
	mMENU_CONFIG_PID   ,	  // Passo mMENU_CONFIG_PID da máquina de estado
	mSTATUS_PID		   ,      // Passo mSTATUS_PID da máquina de estado
	mCONFIG_KP_PID	   ,      // Passo mCONFIG_KI_PID da máquina de estado
	mCONFIG_KI_PID	   ,      // Passo mCONFIG_KI_PID da máquina de estado
	mCONFIG_KD_PID	   ,      // Passo mCONFIG_KI_PID da máquina de estado
	
	mLIMPA_EEPROM      ,      // Passo LIMPA_EEPROM da máquina de estado
};

// ------------------------------------------------------------------------------------------
// PASSOS DA MAQUINA DE ESTADOS - BUZZER
// ------------------------------------------------------------------------------------------

#define gmBUZZER_OFF            0         // Passo BUZZER OFF da máquina de estado do buzzer
#define gmBIP_GLOBAL            1         // Passo BIP GLOBAL da máquina de estado do buzzer
#define gmBIP_ALARME            2         // Passo BIP ALARME da máquina de estado do buzzer

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES DAS VARIÁVEIS GLOBAIS
// ------------------------------------------------------------------------------------------

#ifdef PROGMEM

// ----------------------------------------------------------------------- //
// Máquinas de estado

boolean _atendeMaquina;
boolean _atendeDisplay;
boolean _atendeBuzzer;

volatile uint8_t passoMaquina;
volatile uint8_t passoDisplay;
volatile uint8_t passoBuzzer;
uint8_t passoAnterior;

uint8_t actualMenu;

// ----------------------------------------------------------------------- //
// KeyPad

uint8_t bounceUP;
uint8_t countADTeclado = 0;
uint8_t bounceADTeclado = 0;
unsigned long ADTeclado = 0;
unsigned long ADTecladoAux = 0;
unsigned long CopyADTecladoAux = 0;

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

boolean _flagCheckIncrement = false;
boolean _flagWdtIncrement = false;

uint8_t timeWdtIncrement = 0;
uint8_t wdtIncrement = 0; 

// ----------------------------------------------------------------------- //
// Buzzer

uint8_t nBips;
uint8_t timerBuzzer;
uint8_t periodoBuzzer;

// ----------------------------------------------------------------------- //
// Entrada de dados

boolean _endString = false;
boolean _endInteger = false;
boolean _endValue = false;
boolean _digitarString = false;
boolean _digitarInteger = false;
boolean _digitarValue = false;

int8_t numLetra = -1;
String stDigitado;
String stChar = "_";
float valorDigitado = 0;

// ----------------------------------------------------------------------- //
// Receitas

int ind = 0;
int qtde;
int qtdeMin;
boolean prOper;

boolean _contaTempo = false;

byte numPatamar;
byte qtdePatamar;
int tempoInicial;
int tempoProximo;

byte statusFase;

uint8_t valorPwm = MAX_PWM;
uint8_t valorPwmManual;
int valorTempoManual = 0;

int8_t valorSetpointManual = 0;
boolean alteraSetpoint = false;

int tempInicialAquecimento = -1000;
int qtdeReceita = 0;
int idReceitaSel;
float speedAquecimento = 0;

boolean _temperaturaMaxEntrou = false;
String stResp = "_";

boolean _derrubaMalte = false;
boolean _loadTimerPwm = false;

int tempoInicialVelocidade = 0;
float grauPorMinuto = 0;
float temperaturaFloatInicial = 0;

int tempoRestante = 0;

// ----------------------------------------------------------------------- //
// Temporizadores

#define TIMER_50ms		50 // (*1ms)
#define TIMER_1000ms	20 // (*50ms)
#define TIMER_3000ms	3  // (*1000ms)

uint8_t wdt50ms = TIMER_50ms;
uint8_t wdt1000ms = TIMER_1000ms;
uint8_t wdt3000ms = TIMER_3000ms;

uint8_t wdtDadosBrassagem;
uint8_t wdtTimesTemperatura = TIME_READ_TEMPERATURE;

int wdtTimesDisplay;
int wdtTimesBlink;
long wdtPwmFull;

uint8_t wdtSendTemp;

#define TIME_SEND_READ_AC		1 // (*1sec)
uint8_t wdtSendReadAC;

boolean _flag50ms;
boolean _flag1000ms;
boolean _flag3000ms;
boolean _pwmFull;
boolean _sendDataApp;

boolean _msgConvite;
boolean _msgENSABrew;
boolean _msgTemperatura;

// ----------------------------------------------------------------------- //
// Controle e gerenciamento da comunicação - APP

enum auxTypeHeader
{
	_HEADER_SET = 0,
	_HEADER_GET,
	_HEADER_EVENT,
	_HEADER_SCREEN,
};

enum auxTypeCommand
{
	_CONNECTION_APP = 1,
	_TEMPERATURE,
	_READ_AC,
	_READ_PORT,
	_COUNT_FLUX,
	
	_LOOP_FERVURA,
	_LOOP_WHIRLPOOL,
	_LOOP_DESCANSO,
	_LOOP_PATAMARES,

	_GET_ID,
	_GET_CONFIG,
	_GET_CAL_FACTOR,
	_GET_CONFIG_PID,
	_GET_SEND_TIME_AC,
	_GET_SEND_TIME_TEMP,

	_SET_RESPONSE,
};
	
// Sinaliza o status da conexão com o APP
boolean _connectedAPP = false;

// Flag de controle de envio do keep alive 
boolean _flagSendKeepAlive = false;

// Temporizador de envio do keep alive
#define TIME_SEND_KEEP_ALIVE		60 // (*1sec)
uint8_t wdtSendKeepAlive = 0;

// Flag de controle de aguardo de resposta do keep alive 
// boolean _flagWaitRespKeepAlive = false;

// Temporizador de aguardo de resposta do keep alive
#define TIME_WAIT_RESP_KEEP_ALIVE	10 // (*1sec)
uint8_t wdtWaitRespKeepAlive = 0;

enum auxGeneralCommands
{
	_LOAD = 0,
	_CLEAR,
	_SEND,
	_LOAD_WAIT,
};

// ----------------------------------------------------------------------- //
// Leitura AC

boolean _faltouAC = false;
boolean _sendDataAC = false;
volatile byte pulseAC = 0;
volatile byte Npulse = 0;

// ----------------------------------------------------------------------- //
// Contagem de volume 

float volumeDigitado;
float flow = 0.0;
float milli = 0.0;
float liters = 0.0;
float alarmNivel = 0;

uint8_t wdtResetFluxo;
boolean _flagResetFluxo;
boolean _auxAdjustCalFactor;
boolean _stopCountFlux = false;

// ----------------------------------------------------------------------- //
// Leitura da temperatura

boolean _readTemperature = false;
unsigned int countTemperature = 0;
float realTemperature = 0;
float filtroTemperatura[MAX_SAMPLE_TEMPERATURE];
double mediaTemperature = 0;

Thermistor thermistor(PINO_TERMISTOR);

// ----------------------------------------------------------------------- //
// Serial

boolean _newDataSerial = false;
char receivedSerial[DATA_UTIL_SERIAL];

static boolean _recvInProgress = false;
static uint8_t idx = 0;
volatile char dataChar;

// ----------------------------------------------------------------------- //
// Leitura dos ports - pinos 6 e 7

boolean statePort_1;
boolean lastStatePort_1;
uint16_t lastDebounceTimePort_1;

boolean statePort_2;
boolean lastStatePort_2;
uint16_t lastDebounceTimePort_2;

uint8_t debounceDelay = 100;

//boolean _auxStatePort_1;
//boolean _auxStatePort_2;

// ----------------------------------------------------------------------- //
// Acionamento solenóides

#define SOLENOID1				0
#define SOLENOID2				1

#define TIMER_IN_MILISECONDS	0
#define TIMER_IN_SECONDS 		1
#define TIMER_IN_MINUTES 		2

// Define o tipo do temporizador
uint8_t timerTypeSolenoid[2] = {TIMER_IN_MILISECONDS, TIMER_IN_MILISECONDS};
// Temporizador
uint8_t wdtActionSolenoid[2] = {0, 0};

// ----------------------------------------------------------------------- //
// Visualização das receitas

uint8_t auxVisualizaReceita = 0;

// ----------------------------------------------------------------------- //
// AREA DE TESTES E GERAIS

// Flag auxiliar de resposta
boolean auxResp = false;

String code = "";
uint8_t contadorPartesReceita = 0;
// Auxilia na saída do processo de brassagem
boolean auxKeyEsc = false;

// ------------------------------------------------------------------------------------------
// DEFINIÇÔES DAS STRUCTS
// ------------------------------------------------------------------------------------------

struct Patamar 
{
  uint8_t temperaturaMax = DEFAULT_TEMP_PATAMAR;
  uint16_t tempo = DEFAULT_MINUTO_PATAMAR;
  int tempoDecorrido;
  boolean concluido = false;
};

Patamar patamar[MAX_PATAMAR];

struct Adicao 
{
  char nome[MAX_NOME+1] = "";
  uint8_t tempo = DEFAULT_MINUTO_ADICAO;
  uint16_t qtde = DEFAULT_ADICAO;
};

struct Whirlpool 
{
  // char nome[MAX_NOME] = "";
  uint8_t tempo = DEFAULT_MINUTO_WHIRLPOOL;
  uint8_t qtde = DEFAULT_WHIRLPOOL;
};

struct Descanso 
{
  // char nome[MAX_NOME] = "";
  uint8_t tempo = DEFAULT_MINUTO_DESCANSO;
  uint8_t qtde = DEFAULT_DESCANSO;
};

struct Receita 
{
  uint8_t tempFervura = DEFAULT_TEMP_FERVURA;
  //0=Parada, 1=Em produção, 2=Fervura, 3=Whirlpool e 4=Descanso
  uint8_t statusBrassagem = _STOP; 

  byte qtdePatamar = DEFAULT_QT_PATAMAR;
  byte qtdeAdicao = DEFAULT_ADICAO;
  byte qtdeWhirlpool = DEFAULT_WHIRLPOOL;
  byte qtdeDescanso = DEFAULT_DESCANSO;

  char nome[MAX_NOME+1] = "";

  unsigned int qtdeMinDescanso = DEFAULT_MINUTO_DESCANSO;
  unsigned int qtdeMinFervura = DEFAULT_MINUTO_FERVURA;
  int codigo = RESET;
  int qtdeMinWhirlpool = DEFAULT_MINUTO_WHIRLPOOL;

  Adicao adicao[MAX_ADICAO];
  Patamar patamar[MAX_PATAMAR];
  Whirlpool whirlpool[MAX_WHIRLPOOL];
  Descanso descanso[MAX_DESCANSO];
};

struct ConfigGeral 
{
  byte qtdeMaxPatamar = DEFAULT_MAX_PATAMAR;
  byte pwmSetpoint = DEFAULT_PWM;
  uint8_t tempoEnvioBrassagem = 1; // 1 segundo
  uint8_t sampleTemperature = MAX_SAMPLE_TEMPERATURE;
  uint8_t timeSendReadAC = 0; // Desabilitado por padrão
  uint8_t timeSendTemp = TIME_SEND_TEMPERATURE;
  float grauPorMinuto = DEFAULT_GRAU_MINUTO;
  float calFactor = 4.5;

  boolean statusPID = _DISABLE;
  float kpPID = 2.0;
  float kiPID = 5.0;
  float kdPID = 1.0;

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

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES DAS VARIÁVEIS - MENUS
// ------------------------------------------------------------------------------------------

uint8_t tamanhoMenu = 4;
int8_t posicaoMenu = 0;

String entradaAtual = "Void";
String opcaoAtual;
String statusAtual = "Void";

boolean _analisaOpcMenu = false;

const char* const alfabeto[] PROGMEM = {"A", "B", "C", "D", "E", "F",
							        	"G", "H", "I", "J","K", "L", 
							        	"M", "N", "O", "P", "Q", "R",
							        	"S", "T", "U", "V", "W", "X", 
							        	"Y", "Z", "0", "1", "2", "3",
							        	"4", "5", "6", "7", "8", "9"};

const char* const numeros[] PROGMEM = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

const char stVoltar[] PROGMEM = "Voltar"; 

const char stEscolheReceita[] PROGMEM = "Escolhe Receita";  
const char stCriarReceita[] PROGMEM = "Criar Receita";
const char stAdicionarAgua[] PROGMEM = "Adicionar Agua";
const char stConfiguracao[] PROGMEM = "Configuracoes";
const char* const menu[] PROGMEM = {stEscolheReceita, stCriarReceita, stAdicionarAgua, stConfiguracao};

// Tamanho do "menu"
#define LENGTH_MENU		(sizeof(menu)/sizeof(menu[0])) 

const char stBrassagem[] PROGMEM = "Brassagem";
const char stEditar[] PROGMEM = "Editar";
const char stApagar[] PROGMEM = "Apagar";
const char stVisualizar[] PROGMEM = "Visualizar";
const char* const menuReceitaSel[] PROGMEM = {stVoltar, stVisualizar, stBrassagem, stEditar, stApagar};

// Tamanho do "menuReceitaSel"
#define LENGTH_MENU_RECEITA_SEL		(sizeof(menuReceitaSel)/sizeof(menuReceitaSel[0]))

const char stMaxPatamar[] PROGMEM = "Total Patamares";
const char stGrauPorMinuto[] PROGMEM = "Grau por Minuto";
const char stPwmSetpoint[] PROGMEM = "PWM do Setpoint";
const char stConfigPID[] PROGMEM = "Ajuste PID";
const char stSetSampleTemp[] PROGMEM = "Media do Sensor";
const char stTempoEnvio[] PROGMEM = "Tempos de Envio";
const char stCalibracao[] PROGMEM = "Calibrar Volume";
const char stResetDefault[] PROGMEM = "Reset Default";
const char stAjustarData[] PROGMEM = "Data e Hora";
const char* const menuConfig[] PROGMEM = {stVoltar, stMaxPatamar, stGrauPorMinuto, 
	                                      stPwmSetpoint, stConfigPID, stSetSampleTemp, 
	                                      stTempoEnvio, stCalibracao, stResetDefault};

// Tamanho do menu em questão
#define LENGTH_MENU_CONFIGURACAO	(sizeof(menuConfig)/sizeof(menuConfig[0]))

const char* const userConfigMenu[] PROGMEM = {stVoltar, stMaxPatamar, stGrauPorMinuto, stPwmSetpoint};

// Tamanho do menu em questão
#define LENGTH_MENU_USER_CONFIG		(sizeof(userConfigMenu)/sizeof(userConfigMenu[0]))

const char stStatusPID[] PROGMEM = "Status PID";
const char stConfigKpPID[] PROGMEM = "Ajuste Kp";
const char stConfigKiPID[] PROGMEM = "Ajuste Ki";
const char stConfigKdPID[] PROGMEM = "Ajuste Kd";

const char* const menuConfigPID[] PROGMEM = {stVoltar, stStatusPID,
											 stConfigKpPID, stConfigKiPID, 
											 stConfigKdPID};

// Tamanho do menu em questão
#define LENGTH_MENU_PID	(sizeof(menuConfigPID)/sizeof(menuConfigPID[0]))


const char stTempoAC[] PROGMEM = "Leitura AC";
const char stTempoTemperatura[] PROGMEM = "Temperatura";
const char* const menuTempoEnvio[] PROGMEM = {stVoltar, stTempoAC, stTempoTemperatura};

// Tamanho do menu em questão
#define LENGTH_MENU_TIME_CONFIG		(sizeof(menuTempoEnvio)/sizeof(menuTempoEnvio[0]))


const char stAutoAjuste[] PROGMEM = "Auto Ajuste";
const char stAjusteDireto[] PROGMEM = "Ajuste Manual";
const char* const menuCalFactor[] PROGMEM = {stVoltar, stAutoAjuste, stAjusteDireto};

// Tamanho do menu em questão
#define LENGTH_MENU_CAL_FACTOR		(sizeof(menuCalFactor)/sizeof(menuCalFactor[0]))

// ------------------------------------------------------------------------------------------
// DISPLAY
// ------------------------------------------------------------------------------------------

// Opção barramento paralelo ("normal")
#define DISPLAY_LCD_PRL       	1                 
// Opção de LCD i2C
#define DISPLAY_LCD_I2C     	2
// Opção de display OLED
#define DISPLAY_OLED  			3

// Tipo do display
#define DISPLAY_TYPE			DISPLAY_LCD_PRL
// #define DISPLAY_TYPE			DISPLAY_LCD_I2C
// #define DISPLAY_TYPE			DISPLAY_OLED

// Se estiver utilizando algum display de LCD
#if(DISPLAY_TYPE == DISPLAY_LCD_PRL || DISPLAY_TYPE == DISPLAY_LCD_I2C)
  
	char telaAtual[2][16]  = {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
	char telaBuffer[2][16] = {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

	#if(DISPLAY_TYPE == DISPLAY_LCD_PRL)

		// Library - DEFAULT IDE Arduino/AVR
		#include "ENSABrewLibraries/LiquidCrystal/src/LiquidCrystal.cpp"
		// Instância do objeto
    	LiquidCrystal lcd(28, 29, 30, 31, 32, 33);

    #elif(DISPLAY_TYPE == DISPLAY_LCD_I2C)
    	
    	// https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library  
    	#include "ENSABrewLibraries/LiquidCrystal_I2C/LiquidCrystal_I2C.cpp"
    	// Endereço I2C 
    	// #define ADDRESS_DISPLAY_LCD     0x3F
    	#define ADDRESS_DISPLAY_LCD      0x27
    	// Instância do objeto
    	LiquidCrystal_I2C lcd(ADDRESS_DISPLAY_LCD, 16, 2);
    
    #endif

// Se estiver utilizando display OLED
#elif(DISPLAY_TYPE == DISPLAY_OLED)

    // https://github.com/greiman/SSD1306Ascii
    #include "ENSABrewLibraries/SSD1306Ascii/src/SSD1306AsciiWire.h"
    #include "ENSABrewLibraries/SSD1306Ascii/src/SSD1306Ascii.cpp"
    // Instância do objeto
	SSD1306AsciiWire oled;

	// Linhas do display OLED
	#define LINHA_1				  0        
	#define LINHA_2				  2        
	#define LINHA_3				  4        
	#define LINHA_4				  6  

	// Address of SSD1306 CHIP - 128x64 OLED [0x3C or 0x3D]      
	#define I2C_OLED_ADDRESS     0x3C    

	// Sinaliza reescrita no display
	boolean _rewriteDisplay = true;
	// Sinaliza atualização dos dados no display
	boolean _refreshDisplay = false;

	// Tempo para refresh do display (*1sec)
	#define TIME_REFRESH_DISPLAY		3
	// Temporizador para refresh no display
	uint8_t wdtRefreshDisplay = TIME_REFRESH_DISPLAY;

#endif    

// Temporização para troca de mensagens
#define TIME_CHANGE_MSG		20 // (*50ms)
boolean _changeMsg = false;
uint8_t wdtChangeMsg = TIME_CHANGE_MSG;       
// Contador de troca de mensagens
uint8_t countChangeMsg = 0;     

// ------------------------------------------------------------------------------------------
// Controle PID para temperatura
// ------------------------------------------------------------------------------------------

// Status temporário para configuração
boolean auxTempStatusPID = _DISABLE;

// Define Variables we'll be connecting to
double Setpoint, Input, Output;

// Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;

// Instância do objeto
PID myPID(&Input, &Output, &Setpoint, configGeral.kpPID, configGeral.kiPID, configGeral.kdPID, DIRECT);

// ------------------------------------------------------------------------------------------
// DEFINIÇÕES DAS FUNÇÕES
// ------------------------------------------------------------------------------------------

#else
#warning PROGMEM is not supported on this platform
#endif

//WWWWWWWWWW*********************************************************************************
// Funcao   : stateMachine
// Data     : 09/09/2016 09:55
// Revisão  :
//WWWWWWWWWW*********************************************************************************

void stateMachine() 
{
  if (!_atendeMaquina)
    return;
  _atendeMaquina = false;

  switch (passoMaquina) 
  {
    case mSTANDBY:
      
		clearTimers();
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
      
    	continuaProducao();

    	break;

    case mVISUALIZA_RECEITA:

    	controleVisualizaReceita();

    	break;

    case mEXECUTA_PRODUCAO:
      
    	executaProducao();
      
    	break;

    case mLOOP_BRASSAGEM: 

    	loopBrassagem();

    	break;

    case mDERRUBAR_MALTE: 
        
        derrubarMalte();
    
    	break;

    case mMENU_FERVURA: 
        
        menuExecutaFervura();

    	break;

    case mEXECUTA_FERVURA:

        executaFervura();
      
    	break;

    case mLOOP_FERVURA: 

    	loopFervura();

    	break;

    case mADICAO_FERVURA: 

    	menuAdicaoFervura();
      
    	break;

    case mMENU_WHIRLPOOL: 

        menuExecutaWhirlpool();
      
    	break;

    case mEXECUTA_WHIRLPOOL: 
       
        executaWhirlpool();
      
    	break;

    case mLOOP_WHIRLPOOL: 

    	loopWhirlpool();
      
    	break;

    case mMENU_DESCANSO: 
        
        menuExecutaDescanso();

    	break;

    case mEXECUTA_DESCANSO: 
    
    	executaDescanso();
      
    break;

    case mLOOP_DESCANSO: 
        
    	loopDescanso();
      
    	break;

    case mCRIAR_RECEITA:
      
    	criaReceita();
    
    	break;

    case mNOME_RECEITA: 
    case mQT_PATAMAR: 
    case mTEMPMAX_PATAMAR: 
    case mMINUTO_PATAMAR: 
    case mTEMPMAX_FERVURA: 
    case mMAXMIN_FERVURA: 
    case mQT_ADICAO: 
    case mNOME_ADICAO: 
    case mGRAMAS_ADICAO: 
    case mMINUTOS_ADICAO: 
    case mQT_WHIRLPOOL: 
   
    case mMINUTO_WHIRLPOOL: 
    case mQT_DESCANSO: 
    case mMINUTO_DESCANSO: 
    case mSALVA_RECEITA: 
    
    	criacaoDaReceita();
    	
    	break;

    case mADICIONAR_AGUA:
      	
      	adicionarAgua();
    	
    	break;

    case mCALCULA_FLUXO:
      
		calculaFluxo(volumeDigitado);
      
    	break;

    case mSENHA_CONFIGURACAO: 
    	
    	senhaMenuConfig();

    	break;

    case mMENU_CONFIGURACAO: 
    		
    	menuGeneralConfig();

    	break;

    case mMENU_USER_CONFIG: 
    		
    	menuUserConfig();

    	break;

    case mMENU_TEMPO_ENVIO: 
    	
    	menuTimeConfig();

    	break;

   	case mSET_SEND_TIME_AC: 
	   	
	   	configSendTimeAC();
   		
   		break;

   	case mSET_SEND_TIME_TEMP: 
   		
   		configSendTimeTemperature();

   		break;

    case mSET_MAX_PATAMAR: 
    	
    	configMaxPatamares();

    	break;

    case mSET_GRAU_MINUTO: 
    	
    	configGrauPorMinuto();

    	break;

    case mSET_PWM_SETPOINT: 
    	
    	configPWMSetPoint();

    	break;

    case mMENU_CONFIG_PID:

    	menuGeneralConfigPID();

    	break;

    case mSTATUS_PID:
    case mCONFIG_KP_PID:
	case mCONFIG_KI_PID:
	case mCONFIG_KD_PID:
    	
    	generalConfigPID();
    	
    	break;

    case mSET_SAMPLE_TEMP: 
    	
    	configSampleTemperature();

      break;

    case mMENU_CAL_FACTOR: 
    	
    	executaMenuCalFactor();

   	break;

   	case mAUTO_CAL_FACTOR: 
   		
   		autoCalFactor();

   		break; 

   	case mMANUAL_CAL_FACTOR: 
   		
   		manualCalFactor();

   		break; 

    case mLIMPA_EEPROM:
    	
    	limpaEeprom();
      	
      	break;

    default: break;
  }

  clearKeyPad();
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

  switch (passoDisplay) 
  {

    case mVERSAO:
      
      	printVersao();

		break;

    case mSTANDBY:
      
      	printBoasVindas();

		break;

    case mMENU:
    case mRECEITAS:
    case mRECEITASEL:
    case mMENU_CONFIGURACAO:
    case mMENU_USER_CONFIG:
	case mMENU_TEMPO_ENVIO:
	case mMENU_CAL_FACTOR:
	case mMENU_CONFIG_PID:
	case mSET_DATA_HORA:
      
      	printMenuGlobal();

		break;

    case mCALCULA_FLUXO:
      
      	printFluxo();

		break;

    case mSENHA_CONFIGURACAO:
    case mNOME_RECEITA:
    case mNOME_ADICAO:
      
      	printEntradaString();

		break;

    case mQT_PATAMAR:
    case mTEMPMAX_PATAMAR:
    case mMINUTO_PATAMAR:
    case mTEMPMAX_FERVURA:
    case mMAXMIN_FERVURA:
    case mQT_ADICAO:
    case mGRAMAS_ADICAO:
    case mMINUTOS_ADICAO:
    case mQT_WHIRLPOOL:
    case mMINUTO_WHIRLPOOL:
    case mQT_DESCANSO:
    case mMINUTO_DESCANSO:
    case mADICIONAR_AGUA:
    case mSET_SEND_TIME_AC:
	case mSET_SEND_TIME_TEMP:
	case mMANUAL_CAL_FACTOR:
    case mSET_MAX_PATAMAR:
	case mSET_GRAU_MINUTO:
	case mSET_PWM_SETPOINT:
	case mSET_SAMPLE_TEMP:
    case mCONFIG_KP_PID:
	case mCONFIG_KI_PID:
	case mCONFIG_KD_PID:
      
      	printEntradaValorGeral();

		break;

    case mLIMPA_EEPROM:
    case mAPAGA_RECEITA:
    case mCONTINUA_PRODUCAO:
      
      	printSimNao();

		break;

    case mSTATUS_PID:
    	
    	printConfigStatusPID();
    	
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

	case mVISUALIZA_RECEITA:

    	printVisualizaReceita();

    	break;

    default:
		break;
  }

  if(passoAnterior != passoMaquina && _connectedAPP)
  	enviaTela(true);
  passoAnterior = passoMaquina;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : setup
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void setup() 
{
  
  clearEeprom();
  startPorts();
  startSerial();
  startI2c();
  startDisplay();
  defineTimers();
  carregaReceitas();
  leConfigGeral();
  startTimers();
  loadTimers();
  bipInicial();

  passoMaquina = mSTANDBY;
  passoAnterior = passoMaquina;
}

//WWWWWWWWWW*********************************************************************************
// Funcao   : loop
// Data     : 04/04/2017 13:45
//WWWWWWWWWW*********************************************************************************

void loop() 
{
  if (_flagCheckIncrement && !_keyPress) 
  	checkIncrement();

  keyPad();

  stateMachine();
  machineDisplay();

  serialReceiver();
  analyzesDataSerial();

  controlTemperatureTasks();
  
  if(configGeral.timeSendReadAC != 0)
  	readAC();
  
  readPortTasks();

  communicationAPPTasks();
}

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