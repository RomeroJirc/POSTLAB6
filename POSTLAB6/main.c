/*
 * LAB6.c
 *
 * Created: 19/04/2024 11:07:35 a. m.
 * Author : josei
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void initUART9600(void);
void initADC(void);
void writeUART(char Caracter);
void writeTextUART(char* Texto);
void comprobarque(char opcion, char valorP);
void menu(void);
volatile uint8_t bufferTX;
volatile uint8_t opcion;
volatile uint8_t estado;
volatile uint8_t valorP;
volatile uint8_t intC;
volatile uint8_t intD;
volatile uint8_t intU;

int main(void)
{
    initUART9600();
	initADC();
	sei();
	menu();
    while (1) 
    {
	
	//PORTB = bufferTX;
    }
}

void initADC(void){
		ADMUX = 0;
		ADMUX |= (1<<REFS0);				//REFERENCIA AVCC = 5V
		ADMUX |= (1<<ADLAR);				//JUSTIFICACIÓN A LA IZQUIERDA
		ADMUX |= ((1<<MUX2) | (1<<MUX1));	//AL COLOCAR 0110 INDICAMOS EL ADC6 COMO EL RECEPTOR DE LA SEÑAL ANALÓGICA
		
		ADCSRA = 0;
		ADCSRA |= (1<<ADIE);				//HABILITaR INTERRUPCIONES DE ADC
		ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);	//PRESCALER DE 128 ----> 125kHz
		ADCSRA |= (1<<ADEN);				//HABILITANDO EL ADC
}

void initUART9600(void){
	//Paso 0: DEFINIR SALIDAS
	
	DDRB = (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5);			//Colocar puerto B como salida
	DDRC = (1<<DDC0)|(1<<DDC1);													//COLOCAR A0 y A1 como salidas
	
	//Paso 1: RX como entrada y TX como salida 
	DDRD &= ~(1<<DDD0); //RX Como Entrada
	DDRD |= (1<<DDD1);	//TX Como Salida
	
	//Paso 2: Configurar UCSR0A
	
	UCSR0A = 0;						//Resetear Registro
	
	//Paso 3: Configurar UCSR0B Habilitando ISR de recepción, rx y tx 
	
	UCSR0B = 0;											//Resetear Registro
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);	//Habilitar Interrupciones de RX, Recepción, Transmisión
	
	//Paso 4: Configurar UCSR0C, Asincrono, Pariedad: None, 1 bit de Stop, Data bits: 8
	
	UCSR0C = 0;											//Resetear Registro
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);				//Colocar 8 bits de datos
	
	//Paso 5: Configurar velocidad de Baudrate: 9600
	
	UBRR0 = 103;
	estado = 0;

}

void writeTextUART(char* Texto){
	uint8_t i;
	for(i=0; Texto[i]!='\0'; i++){
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = Texto[i];
	}
	
}

void writeUART(char Caracter){
	while(!(UCSR0A & (1<<UDRE0)));
	
	UDR0 = Caracter;
}

void menu(void){
	writeTextUART("---------------------------------------");
	writeUART(10);
	writeUART(13);
	writeTextUART("Eliga una opcion:");
	writeUART(10);
	writeUART(13);
	writeUART(10);
	writeUART(13);
	writeTextUART("1. Leer Potenciometro");	//31 es 1 en Hexadecimal según ANSII
	writeUART(10);
	writeUART(13);
	writeUART(10);
	writeUART(13);
	writeTextUART("2. Enviar Ascii");		//32 es 2 en Hexadecimal según ANSII
	//writeUART('B');
	writeUART(10);
	writeUART(13);
	writeUART(1);
}

void comprobarque(char opcion, char valorP){
		/*switch (opcion)
		{
			case 49:
			writeTextUART("Elegiste la opcion 1");
			writeUART(10);
			writeUART(13);
			menu();
			break;
			case 50:
			writeTextUART("Elegiste la opcion 2");
			writeUART(10);
			writeUART(13);
			menu();
			break;
			writeTextUART("MAHE 1 O 2 PUTA!");
			writeUART(10);
			writeUART(13);
			menu();
			}*/
		if(estado == 1)
		{
		PORTB = opcion;
		PORTC = (opcion>>6);
		estado = 0;
		writeTextUART("LISTO");
		writeUART(10);
		writeUART(13);
		menu();
		}
		else if(estado == 0){
		if(opcion == 49)
		{
		writeUART(10);
		writeUART(13);
		writeTextUART("Elegiste la opcion 1");	
		writeUART(10);
		writeUART(13);
		writeTextUART("---------------------------------------");
		writeUART(10);
		writeUART(13);
		writeTextUART("El valor de tu potenciometro es:");
		ADCSRA |= (1<<ADSC);				//ADC START CONVERSION
		writeUART(valorP);
		//writeUART(intC);
		//writeUART(intD);
		//writeUART(intU);
		writeUART(10);
		writeUART(13);
		writeUART(10);
		writeUART(13);
		menu();
		}
		else if(opcion == 50)
		{
		writeTextUART("Elegiste la opcion 2");
		writeUART(10);
		writeUART(13);
		estado = 1;
		writeTextUART("---------------------------------------");
		writeUART(10);
		writeUART(13);
		writeTextUART("Mande un Caracter");
		writeUART(10);
		writeUART(13);
		}
		else
		{
		writeTextUART("ERROR");
		writeUART(10);
		writeUART(13);
		menu();
		}
		}
	}

ISR(USART_RX_vect){
	
	//bufferTX = UDR0;
	//while(!(UCSR0A & (1<<UDRE0)));
	//UDR0 = bufferTX;
	
	
	
	opcion = UDR0;
	comprobarque(opcion, valorP);
	writeUART(10);
	writeUART(13);
}

ISR (ADC_vect){
	
	valorP = ADCH;
	//UDR0 = valorP;
	//while(!(UCSR0A & (1<<UDRE0)));
	//UDR0 = valorP;
	//intC = ADCH/100;
	//intD = (ADCH-intC*100)/10;
	//intU = (ADC-(intC*100+intD*10));
	ADCSRA |= (1<<ADIF);
}