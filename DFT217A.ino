  // DFTA217
  // Petit programme utilisant un MCU SEEEDUINO XIAO associé à deux LM35DZ et deux minis écrans Oled 0.91
  // Pour pouvoir driver les 2 écrans qui ont la même adresse I2C, j'ai utilisé un multiplexeur de bus ici le TCA9548A sous forme de mini-plaquette
  // C'est en fait un double afficheur de température ambiante mais pour le premier capteur il est monté classiquement en générateur de tension quant au  deuxième un
  // petit artifice de câble permet de le faire fonctionner en générateur de courant et donc on peut éloigner le capteur et le relier au microcontroleur par un câble
  // d'une grande longueur sans que le résultat en soit affecté
  // Ma chaîne Youtube : https://www.youtube.com/c/ClaudeDufourmont
  // Mon GitHub : https://github.com/claudiusmarius
  // Le lien de la vidéo concernant ce code est ci-dessous, il vous permettra de remonter jusque min GitHub
  // https://youtu.be/e5st8zisy-U
  // Précautions de câblage :
  // SEEEDUINO XIAO :       SDA A4  à brancher sur SDA écran
  //                        SCL A5  à brancher sur SCK écran 
  //
  // INCONVENIENT : Ce montage ne peut afficher que des températures en °C positives.
  // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Afin de m'encourager à continuer à PARTAGER tous mes projets, vous pouvez si vous le souhaitez LIKER, COMMENTER, PARTAGER cette vidéo et même vous ABONNER à ma chaîne
  // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
  //                                                      ENORME MERCI A TOUTES ET A TOUS
  // ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
  
  #include <Wire.h>                                                            // Bibliothèque I2C
  #include <Adafruit_GFX.h>                                                    // Bibliothèque graphique (texte, formes, etc.) 
  #include <Adafruit_SSD1306.h>                                                // Contrôle des écrans OLED SSD1306
  
  // === Définition des constantes ===
  #define SCREEN_WIDTH 128                                                     // Largeur de l'écran OLED
  #define SCREEN_HEIGHT 32                                                     // Hauteur de l'écran OLED
  #define OLED_ADDR 0x3C                                                       // Adresse I2C des écrans OLED
  #define TCA_ADDR 0x70                                                        // Adresse I2C du multiplexeur TCA9548A
  #define OLED_RESET -1

  #define LM35_PIN1 A1
  #define LM35_PIN2 A2

  float correction1 = -2.0;                                                     // Facteur de correction de zéro pour le capteur 1
  float correction2 = -2.0;                                                     // Facteur de correction de zéro pour le capteur 2

  float gain1 = 0.91;                                                           // Facteur de correction d'échelle pour le capteur 1
  float gain2 = 0.90;                                                           // Facteur de correction d'échelle pour le capteur 2

  const int bufferSize = 5;
  float buffer1[bufferSize];
  float buffer2[bufferSize];
  int index1 = 0, index2 = 0;
  bool bufferFull1 = false, bufferFull2 = false;

  // ==================================================== Création d’un objet écran unique, réutilisé pour chaque canal ================================================== 
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  // =====================================================================================================================================================================
  
  // ============================================================= Fonction pour sélectionner un canal du TCA9548A =======================================================
  void selectChannel(uint8_t ch) 
  {
  Wire.beginTransmission(TCA_ADDR);                                                // Début communication I2C avec le TCA9548A 
  Wire.write(1 << ch);                                                             // Activation du canal ch (bit shift pour sélectionner la voie)             
  Wire.endTransmission();                                                          // Fin de transmission 
  }
  // =====================================================================================================================================================================
  
  // ================================================================ Initialisation de tous les écrans ==================================================================
  void setup() 
  {
  Wire.begin();
  Serial.begin(115200);
  for (int i = 0; i < 2; i++) 
  {
  selectChannel(i);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  }
  }
  // =====================================================================================================================================================================
  
  // ============================================================ Fonction filtrage par moyenne glissante ===============================================================
  float getFilteredTemperature(float* buffer, int& index, bool& full, float raw) 
  {
  buffer[index] = raw;
  index = (index + 1) % bufferSize;
  if (index == 0) full = true;

  int size = full ? bufferSize : index;
  float sum = 0.0;
  for (int i = 0; i < size; i++) sum += buffer[i];
  return sum / size;
  }
  // =====================================================================================================================================================================
  
  // ============================================================== Fonction affichage temperature =====================================================================
  void showTemperature(uint8_t screen, float temperature) 
  {
  char buffer[10];
  dtostrf(temperature, 4, 1, buffer); // Format " XX.X"

  selectChannel(screen);
  display.clearDisplay();
  display.setTextSize(3);                                                               // Taille plus grande (max utilisable sur 128x32 pour 4 caractères)
  display.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buffer, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_WIDTH - w) / 2;
  int16_t y = (SCREEN_HEIGHT - h) / 2;

  display.setCursor(x, y);
  display.print(buffer);
  display.display();
  }
  // =====================================================================================================================================================================
  
  // =========================================== Boucle principale : Calcul de la température et demande d'affichage =====================================================
  void loop() 
  {
  int val1 = analogRead(LM35_PIN1);
  float voltage1 = val1 * (3.3 / 1023.0);
  float temp1 = voltage1 * 100.0 * gain1 + correction1;
  float tempFiltree1 = getFilteredTemperature(buffer1, index1, bufferFull1, temp1);

  int val2 = analogRead(LM35_PIN2);
  float voltage2 = val2 * (3.3 / 1023.0);
  float temp2 = voltage2 * 100.0 * gain2 + correction2;
  float tempFiltree2 = getFilteredTemperature(buffer2, index2, bufferFull2, temp2);

  showTemperature(0, tempFiltree1);
  showTemperature(1, tempFiltree2);

  delay(500);
  }
