//simondice

#include <16f84a.h>
#include <stdlib.h>

#define NO_KEY		255
#define FinREBOTES	255
#define ROJO		1
#define VERDE		2
#define AZUL		3
#define AMARILLO	4
#define INICIO		5
#define CANTIDAD_DE_REBOTES		100

#use fast_io(A)
#use fast_io(B)
#FUSES XT, NOWDT, NOPUT  
#use delay (clock = 4000000)
#BYTE PORTA = 0X05
#BYTE PORTB = 0X06
#bit  RA0 = 0x05.0
#bit  RA1 = 0x05.1
#bit  RA2 = 0x05.2
#bit  RA3 = 0x05.3
#bit  RA4 = 0x05.4
#byte   INTCON = 0x0B

void Board( void );
void PedirTecla( void );

int key=NO_KEY;
int aleatorio, nivel, tam, i,color_leido, color_pulsado, dir_lectura,paso,leido;
short mal, gano;
// Board
int CodigoActual;
int CodigoAnterior;
int EstadosEstables;


void retardo(int latencia){
    switch(latencia){
	case 0: delay_ms(600);       // Correspondiente al nivel 1
	    break;
	case 1: delay_ms(400);       // Nivel 2
	    break;
	case 2: delay_ms(200);        // Nivel 3
	    break;
	case 3: delay_ms(150);        // Nivel 4
	    break;
	default:
	    break;
    }
}

void tono (int i){
    int j;
    switch(i){
	case 1:
	    for(j=0;j<40;j++){
		bit_set(portb,4);delay_us(500);bit_clear(portb,4);delay_us(500);
	    }
	break;
	case 2:
	    for(j=0;j<20;j++){
	    bit_set(portb,4);delay_ms(1);bit_clear(portb,4);delay_ms(1);}
	    break;
	case 3:
	    for(j=0;j<10;j++){
	    bit_set(portb,4);delay_ms(3);bit_clear(portb,4);delay_ms(3);}
	    break;
	case 4:
	    for(j=0;j<8;j++){
	    bit_set(portb,4);delay_ms(5);bit_clear(portb,4);delay_ms(5);}
	    break;
	case 5:
	    for(j=0;j<60;j++){
	    bit_set(portb,4);delay_ms(9);bit_clear(portb,4);delay_ms(9);}
	    break;
   }

}

void enciende_led(int color)  // Enciende el led correspondiente
{
    switch(color){
	case ROJO: output_b(1);     // Led rojo
	    break;
	case VERDE: output_b(2);     // Led verde
	    break;
	case AMARILLO: output_b(4);     // Led amarillo
	    break;
	case AZUL: output_b(8);     // Led azul
	    break;
	default: PORTB = 15;             // Los 4 leds
	    break;
    }
}


void genera_aleatorio(){
    aleatorio ++;
    aleatorio %= 4;
}

void guardaSec(){
   write_eeprom(paso,aleatorio);  // Guardamos el color generado y apuntamos a
   paso++;                        //  la siguiente dirección para una próxima
}                                       //  escritura
 

void mostrarSec(void){
    dir_lectura = 0;
    for(dir_lectura = 0; dir_lectura< paso; dir_lectura++)
    {
	color_leido = read_eeprom(dir_lectura);	 	
	while(color_leido==15){portb=0x80;}
	tono(color_leido);
	enciende_led(color_leido);		
	retardo(nivel);                           // Retardo según nivel de dificultad
	PORTB = 0;                                // Apaga led
	retardo(nivel);                           // Retardo según nivel de dificultad
		
    }
}


void comprueba(){
    leido = read_eeprom(dir_lectura);  // Leemos la dirección eeprom correspondiente.
    if(leido != color_pulsado)         // Si la pulsación no ha sido correcta,acaba el
    {                                 //  juego y volvemos al principio del programa
	mal = true;
    }
}

void ingr_datos(void){
    short sal;
    dir_lectura = 0;
    //aleatorio = 1; //PAra qu hacemos esto si ya tiene un valor aleatorio
    
    /* Recogemos las pulsaciones y se va comprobando si son correctas hasta que
    //  alguna no lo sea o hasta que hayamos acertado todos los colores guardados
    //  hasta el momento.
    // dir_escritura contiene la dirección eeprom siguiente al último color guardado
    //  y dir_lectura la usamos para ir consultando cada posición de memoria y comprobar
    //  si la pulsación ha sido correcta.En el momento en que fallemos alguna,fin_juego toma
    //  el valor TRUE.
    // Durante la ejecución del bucle,aleatorio irá cambiando de valor,hasta que pulsemos el
    //  último color,momento en el cual salimos del bucle y guardamos aleatorio en memoria.*/
    while((dir_lectura < paso) && (!mal)){		
	sal = false;
	//output_b(0x10);delay_ms(1000);output_b(0x20);delay_ms(1000);output_b(0x40);delay_ms(1000);output_b(0x80);delay_ms(1000);
	while(!sal)  // Mientras no haya pulsación nos mantenemos dentro del bucle
	{
	    genera_aleatorio();           // Para conseguir aleatoriedad en los colores guardados
	    Board();
	    if(key == ROJO){             // Se ha pulsado el rojo,salimos del bucle
		color_pulsado = ROJO;
		sal = true;
	    }
	    else if(key == VERDE) {    // Se ha pulsado el verde,salimos del bucle
		color_pulsado = VERDE;
		sal = true;
	    }
	
	    else if(key == AMARILLO) {    // Se ha pulsado el amarillo,salimos del bucle
		color_pulsado = AMARILLO;
		sal = true;
	    }
	    else if(key == AZUL) {    // Se ha pulsado el azul,salimos del bucle
		color_pulsado = AZUL;
		sal = true;
	    }
	}		
	comprueba();   // Algoritmo que comprueba si la pulsación ha sido correcta
	enciende_led(color_pulsado);  // Enciende el led del color que hemos pulsado
	tono(color_pulsado);       // Genera el tono del color que hemos pulsado
	PORTB = 0;                    // Apagamos led
	dir_lectura++;                // Para comprobar la siguiente dirección eeprom
    }
}

void demo( void ){  /*demostracion prenden los leds aleatoriamente*/
    Board();
    while( key != INICIO )
    {
	switch(rand()%4)  //Se muestran numeros aleatorios 1, 2, 4 y 8
	{
	    case 0: output_b(1);aleatorio=1;tono(1);break;
	    case 1: output_b(2);aleatorio=2;tono(2);break;
	    case 2: output_b(4);aleatorio=3;tono(3);break;
	    case 3: output_b(8);aleatorio=4;tono(4);break;
	}
	delay_ms(200);if(RA4){break;}delay_ms(200);if(RA4){break;}delay_ms(200);if(RA4){break;}delay_ms(200);if(RA4){break;}
	portb=0x00;
	delay_ms(200);if(RA4){break;}delay_ms(200);if(RA4){break;}delay_ms(200);if(RA4){break;}delay_ms(200);if(RA4){break;} 
	Board();
    }
}

void main (void){
    port_b_pullups (TRUE);
    set_tris_B(0);
    set_tris_A(0b00011111); 
    INTCON=0;
    enable_interrupts(INT_EEPROM); // Unica interrupción habilitada durante toda la ejecución
    enable_interrupts(GLOBAL);     // Habilitador general de interrupciones
    gano=false;
    nivel=0;

    while(TRUE){
  	paso = dir_lectura = color_leido = leido = color_pulsado = 0;   
	mal=false;		
  	if(!gano){
	    tam=6;
	}
	demo();
	guardaSec();	//Guarda en la memoria eeprom el valor de la secuencia	el valor aleatorio generado en la demo   	
   	/*inicia el modo juego*/ 
	output_b(1);delay_ms(150);output_b(2);delay_ms(150);output_b(4);delay_ms(150);output_b(8);delay_ms(150);
	portb=0x00;delay_ms(1000);
	mostrarSec();

	while( paso < tam && !mal ){   /*verifica si acierta o si erra*/
	    //output_b(0x10);delay_ms(1000);output_b(0x20);delay_ms(1000);output_b(0x40);delay_ms(1000);output_b(0x80);delay_ms(1000);
	    ingr_datos();
	    if( !mal ){      /*acierta*/
		guardaSec();//Guarda el valor aleatorio generado en ingr_datos
		if( paso ==tam ){nivel++; gano=1; break;} //Se fija si gano el nivel
		retardo(nivel);
		mostrarSec();
	    }
	    else{    /*error*/
		while(paso == 0){portb=0x80;}
 		nivel=0; gano = false;
		output_b(0x0f);
   		tono(5);		
		output_b(0);
		delay_ms(3000);
      		paso=63;
	    }
   	}
   	if( gano ){
	    switch(tam){
   		case 6:tam=9;break;
   		case 9: tam=16;break;
   		case 16: tam=21; break;
		default: break; //case 21: tam=21;
	    }
	    output_b(0x08);tono(1);delay_ms(300);output_b(0x04);tono(2);delay_ms(300);output_b(0x02);tono(3); delay_ms(300);
	    output_b(0x01);tono(4);delay_ms(100);output_b(0);               
 	}
    }  /*fin while true*/ 
}



//Intento hacer funcion anti rebote
void PedirTecla( void ){
    if(input(PIN_A0)==0)
	CodigoActual=ROJO;
    else if(input(PIN_A1)==0)
	CodigoActual=VERDE;
    else if(input(PIN_A1)==0)
	CodigoActual=AMARILLO;
    else if(input(PIN_A1)==0)
	CodigoActual=AZUL;
    else if(input(PIN_A1)==0)
	CodigoActual=INICIO;
    else
	CodigoActual=NO_KEY;
}

void Board( void )
{    
    EstadosEstables = 0;    
    PedirTecla();    
    CodigoAnterior = CodigoActual;
    key = NO_KEY;
    while (estadoRebotes != FinREBOTES){
	
	if(CodigoActual != CodigoAnterior || CodigoActual == NO_KEY ){
	    estadoRebotes = FinREBOTES;
	    CodigoActual = NO_KEY;
	}
	if( EstadosEstables >= CANTIDAD_DE_REBOTES )
	{
	    key = CodigoActual;                  // Acepto la nueva tecla
	    estadoRebotes = FinREBOTES;
	}
	EstadosEstables++;
	PedirTecla();
    }
}    
    
