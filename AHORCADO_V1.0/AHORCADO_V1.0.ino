/*
  Juego el Arhocado

  Fecha de creacion: 05/03/2016
  Autor: Sergio Martín Rubio
 */

#include <LiquidCrystal.h>

#define TAM 20  //tamaño array palabra

//pantalla LCD
int rs = 9, e = 8, d4 = 6, d5 = 5, d6 = 3, d7 = 2; //pines de conexion
LiquidCrystal miLcd (rs, e, d4, d5, d6, d7); //creamos objeto para lcd

//botones - se utilizan resistancias pull-down
char abc[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
int b_selec = 39; //pin boton para seleccionar lera
int b_sub= 40; //pin boton para subir una poscion
int b_baj = 41; //pin boton para bajar una posicion
boolean estado_b_selec = LOW;
boolean estado_b_sub = LOW;
boolean estado_b_baj = LOW;

//caricatura
char cari[]={22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38}; //pines el encendido de los leds
int intentos = 0; //contador de errores

//Funciones auxiliares
void crear_huecos(char cc[], int lp);
void limpiar(char tc[]);
boolean letra_rep(char cc[], char l, int pla);
int comprobar_letra(char ps[], char l);
void mostrar_estruc_pal(char ps[], char l, char cc[]);
void caricatura(int fallos);
void apagar();

void setup() {
  Serial.begin(9600);
  miLcd.begin(16, 2); //lc de 16 columnas y 2 filas
  miLcd.home(); //Cursor en esquina sup izq
  
  pinMode(b_selec, INPUT);
  pinMode(b_sub, INPUT);
  pinMode(b_baj, INPUT);

  for(int k=0 ; k<strlen(cari) ; k++)
    pinMode(cari[k], OUTPUT);
}

void loop() {

  char *palabras[] = {"ordenador", "teclado", "raton", "monitor", "torre", "alfombrilla", "altavoces", "microfono", NULL};
  char *pal_selec; //puntero que apunta a cada palabra del array *palabras[]
  char cad_carac[TAM]; /*Array que almacena caracteres accertados y guiones*/
  char total_carac[TAM];/*Array que almacena todos los caracteres introducidos*/
  int lon_pal_selec;
  char letra;
  int pos_letra_acum = 0;
  int letras_res;
  int num_pal = -1;
  char res;
  int pos;

  /*Calculamos numero de palabras del array de strings*/
  while(palabras[++num_pal]!= NULL){}/*buscamos el valor centinela(NULL)*/
  
  /*El programa elige la palabra que el jugador tiene que adivinar*/
  randomSeed(analogRead(0));
  //Serial.println("Generando palabra aleatoria...");
  pos = random(0, num_pal);
  pal_selec = palabras[pos];
  //Serial.println(pal_selec);

  lon_pal_selec = letras_res = strlen(pal_selec);
  /*Funcion que limpia todas las posiciones donde se guardan todos los caracteres*/
  limpiar(total_carac);
  /*Creamos las posiciones de la palabra a adivinar*/
  crear_huecos(cad_carac, lon_pal_selec);
  
  do{
    int i;
    int j = 0;
    estado_b_selec = LOW;
    
    //Serial.write(estado_b_selec);
    while(estado_b_selec == LOW){  //estamos en el bucle hasta que se presione el boton selecionar
      estado_b_selec = digitalRead(b_selec); //recoge estado del boton que selecciona
      estado_b_sub = digitalRead(b_sub);     //recoge estado del boton que sube de letra
      estado_b_baj = digitalRead(b_baj);     //recoge estado del boton que baja de letra
      
      if(estado_b_sub == HIGH){  //boton de subir letra presionado
        j++;
        if(j>=26)
          j=0;
        delay(300);
        miLcd.clear();
      }
      if(estado_b_baj == HIGH){  //boton de bajar letra presionado
        j--;
        if(j<=-1)
          j=25;
        delay(300);
        miLcd.clear();      
      }
      miLcd.setCursor(0, 0);
      miLcd.print("Letra selec: ");
      miLcd.setCursor(13, 0);
      miLcd.print(abc[j]);
      
      /*Mostramos en pantalla los huecos acertados*/
      for(i=0 ; i<strlen(pal_selec) ; i++){
        miLcd.setCursor(i, 1);
        miLcd.print(cad_carac[i]);
      }      
    }

    miLcd.clear();
    /*Acumulamos todas las letras introducidas en esta funcion*/
    total_carac[pos_letra_acum] = abc[j];
    //Serial.print("La letra introducida es: ");
    //Serial.println(abc[j]);
    
    /*Comprobamos si se ha introducido una letra repetida*/
    if(letra_rep(total_carac, abc[j], pos_letra_acum) == 0){ //Si la funcion devuelve 0 la letra es repetida
      miLcd.setCursor(0, 0);
      miLcd.print("Letra repetida!");
      delay(1000);
      miLcd.clear();
      intentos++;
    }
    else{
      /*Comprobamos que la letra este en la palabra a adivinar*/
      if(comprobar_letra(pal_selec, abc[j]) > 0){
        miLcd.setCursor(0, 0);
        miLcd.print("Letra correcta!");
        delay(1000);
        miLcd.clear();
        /*Mostramos las letras acertadas y su posicion*/
        mostrar_estruc_pal(pal_selec, abc[j], cad_carac);
        letras_res -= comprobar_letra(pal_selec, abc[j]);
      }
      else{
        miLcd.setCursor(0, 0);
        miLcd.print("Letra erronea!");
        for(int q=0, i=0 ; i<pos_letra_acum + 1 ; i++, q+=2){
          miLcd.setCursor(q, 1);
          miLcd.print(total_carac[i]);
        }
        delay(2000);
        miLcd.clear();
        intentos++;
      }
    }
    /*LEDS que representan los 6 fallos posibles*/
    caricatura(intentos);

    if(letras_res == 0){
      miLcd.clear();
      miLcd.setCursor(0, 0);
      miLcd.print("HAS GANADO!");
      delay(3000);
    }
    pos_letra_acum++;
    
  } while(intentos < 6 && letras_res > 0);

    if(intentos>=6){
      miLcd.clear();
      miLcd.setCursor(0, 0);
      miLcd.print("HAS PERDIDO!");
      delay(3000);
    }
    miLcd.clear();
    intentos=0; //reiniciamos intentos
    apagar();   //reiniciamos muneico
}

void crear_huecos(char cc[], int lp){
  int i = 0;
  while(i<TAM){
    if(i<lp)
      cc[i] = '_';
    else
      cc[i] = ' ';
    i++;
  }
}
void limpiar(char tc[]){
  int i = 0;
  while(i<TAM -1 ){
    tc[i] = ' ';
    i++;
  }
  tc[i] = '\0';/*importante!, nos dice cuando acaba el array*/
}
boolean letra_rep(char tc[], char l, int pla){
  int i;
  for(i=0 ; i<strlen(tc) ; i++){
    if(tc[i] == l && i!=pla)
      return 0;
  }
  return 1;
}
int comprobar_letra(char ps[], char l){
  int i, cont = 0;
  for(i=0 ; i<strlen(ps) ; i++){
    if(ps[i] == l)
      cont++;
  }
  return cont;
}
void mostrar_estruc_pal(char ps[], char l, char cc[]){
  int i;
  for(i=0 ; i<strlen(ps) ; i++){
    if(ps[i]==l){
      cc[i] = ps[i];
    }
  }
}
void caricatura(int fallos){

  for(int i = 22; i<=30 ; i++){
    digitalWrite(i, HIGH); 
  }
  if(fallos >= 1){
    digitalWrite(31, HIGH);
    digitalWrite(32, HIGH);
  }
  if(fallos >= 2){
    digitalWrite(33, HIGH);
    digitalWrite(34, HIGH);
  }
    
  if(fallos >= 3)
    digitalWrite(35, HIGH);
    
  if(fallos >= 4)
    digitalWrite(36, HIGH);

  if(fallos == 5)
    digitalWrite(37, HIGH);
    
  if(fallos == 6)
    digitalWrite(38, HIGH);
}
void apagar(){
  for(int i = 22; i<=38 ; i++){
    digitalWrite(i, LOW); 
  }
}

