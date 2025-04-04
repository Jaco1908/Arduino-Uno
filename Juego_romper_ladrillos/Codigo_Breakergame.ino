#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definición de la pantalla OLED
#define SCREEN_WIDTH 128 // Ancho en píxeles de la pantalla OLED
#define SCREEN_HEIGHT 64 // Alto en píxeles de la pantalla OLED
#define OLED_RESET 4     // Pin de reset para Arduino UNO (puedes usar 4 o cualquier pin digital)
#define SCREEN_ADDRESS 0x3C // Dirección I2C (típicamente 0x3C o 0x3D)

// Crear instancia de la pantalla
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definición de pines
#define JOY_X A0  // Pin analógico para el eje X del joystick
#define JOY_Y A1  // Pin analógico para el eje Y del joystick
#define BUZZER_PIN 3  // Pin para el buzzer

// Configuración de la paleta
#define PADDLE_HEIGHT 2
#define PADDLE_Y (SCREEN_HEIGHT - 7)
int PADDLE_WIDTH = 20; // Ahora es variable
int paddleX;
uint8_t paddleSpeed = 3;
uint8_t paddleInitialWidth = 20; // Ancho inicial de la paleta

// Configuración de la pelota
#define BALL_SIZE 2
int8_t ballX, ballY;
int8_t ballDX = 1, ballDY = -1;
float ballSpeedMultiplier = 1.0;

// Configuración de los ladrillos
#define BRICK_WIDTH 10
#define BRICK_HEIGHT 3
#define BRICK_COLS 10
#define BRICK_ROWS 3
#define BRICK_PADDING 2
#define BRICK_TOP_MARGIN 15
boolean bricks[BRICK_ROWS][BRICK_COLS];
uint8_t bricksDurability[BRICK_ROWS][BRICK_COLS];  // Durabilidad de los ladrillos
boolean specialBricks[BRICK_ROWS][BRICK_COLS];  // Ladrillos especiales que aplican efectos

// Estado del juego
uint16_t score = 0;
uint8_t lives = 3;
uint8_t level = 1;
uint8_t bricksRemaining = 0;
boolean gameStarted = false;
unsigned long lastFrameTime = 0;
unsigned long lastEffectTime = 0;
#define FRAME_DELAY 50  // 20 FPS
#define EFFECT_DURATION 5000  // 5 segundos para efectos temporales

// Función para reproducir sonidos
void playSound(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
}

// Inicializar los ladrillos
void initBricks() {
  bricksRemaining = 0;
  for (uint8_t row = 0; row < BRICK_ROWS; row++) {
    for (uint8_t col = 0; col < BRICK_COLS; col++) {
      bricks[row][col] = true;
      bricksRemaining++;
      
      // A partir del nivel 2, algunos ladrillos requieren más golpes
      if (level >= 2 && random(100) < level * 10) {
        bricksDurability[row][col] = min(3, level - 1); // Máximo 3 golpes
      } else {
        bricksDurability[row][col] = 1; // Un golpe
      }
      
      // A partir del nivel 3, algunos ladrillos son especiales
      specialBricks[row][col] = (level >= 3 && random(100) < level * 5);
    }
  }
}

// Dibujar los ladrillos
void drawBricks() {
  for (uint8_t row = 0; row < BRICK_ROWS; row++) {
    for (uint8_t col = 0; col < BRICK_COLS; col++) {
      if (bricks[row][col]) {
        int x = col * (BRICK_WIDTH + BRICK_PADDING);
        int y = BRICK_TOP_MARGIN + row * (BRICK_HEIGHT + BRICK_PADDING);
        
        // Dibujar el ladrillo según su durabilidad
        if (bricksDurability[row][col] > 2) {
          // Ladrillo muy resistente - relleno con patrón
          display.fillRect(x, y, BRICK_WIDTH, BRICK_HEIGHT, SSD1306_WHITE);
          display.drawLine(x, y, x+BRICK_WIDTH, y+BRICK_HEIGHT, SSD1306_BLACK);
          display.drawLine(x, y+BRICK_HEIGHT, x+BRICK_WIDTH, y, SSD1306_BLACK);
        } else if (bricksDurability[row][col] > 1) {
          // Ladrillo resistente - relleno normal
          display.fillRect(x, y, BRICK_WIDTH, BRICK_HEIGHT, SSD1306_WHITE);
        } else {
          // Ladrillo normal - contorno
          display.drawRect(x, y, BRICK_WIDTH, BRICK_HEIGHT, SSD1306_WHITE);
        }
        
        // Marcar ladrillos especiales
        if (specialBricks[row][col]) {
          display.drawPixel(x + BRICK_WIDTH/2, y + BRICK_HEIGHT/2, SSD1306_WHITE);
        }
      }
    }
  }
}

// Nueva función para efectos especiales
void applySpecialEffect(uint8_t col, uint8_t row) {
  // Elegir un efecto aleatorio
  int effect = random(level); // Más efectos en niveles superiores
  
  switch (effect % 4) {
    case 0: // Reducir tamaño de la paleta
      if (PADDLE_WIDTH > 10) {
        PADDLE_WIDTH -= 2;
        paddleX = constrain(paddleX, 0, SCREEN_WIDTH - PADDLE_WIDTH);
      }
      break;
    case 1: // Aumentar velocidad de la pelota
      ballSpeedMultiplier += 0.1;
      break;
    case 2: // Efecto positivo: Aumentar tamaño de la paleta
      PADDLE_WIDTH += 2;
      PADDLE_WIDTH = min(PADDLE_WIDTH, 30); // Máximo tamaño
      paddleX = constrain(paddleX, 0, SCREEN_WIDTH - PADDLE_WIDTH);
      break;
    case 3: // Invertir controles temporalmente
      paddleSpeed = -paddleSpeed;
      lastEffectTime = millis(); // Iniciar temporizador para este efecto
      break;
  }
  
  // Mostrar efecto visual
  display.invertDisplay(true);
  delay(50);
  display.invertDisplay(false);
}

// Dibujar la paleta
void drawPaddle() {
  display.fillRect(paddleX, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, SSD1306_WHITE);
}

// Dibujar la pelota
void drawBall() {
  display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, SSD1306_WHITE);
}

// Dibujar la información del juego (puntuación, vidas)
void drawGameInfo() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Puntuación
  display.setCursor(0, 0);
  display.print("P:");
  display.print(score);
  
  // Nivel
  display.setCursor(50, 0);
  display.print("N:");
  display.print(level);
  
  // Vidas
  display.setCursor(100, 0);
  display.print("V:");
  display.print(lives);
}

// Función para reiniciar el nivel
void resetLevel() {
  // Posición inicial de la paleta
  paddleX = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
  
  // Posición inicial de la pelota (encima de la paleta)
  ballX = paddleX + (PADDLE_WIDTH / 2) - (BALL_SIZE / 2);
  ballY = PADDLE_Y - BALL_SIZE - 1;
  
  // Dirección inicial aleatoria
  ballDX = random(0, 2) == 0 ? -1 : 1;
  ballDY = -1;
  
  // Inicializar los ladrillos
  initBricks();
  
  // Reiniciar efectos temporales al inicio del nivel
  lastEffectTime = millis();
  
  // Asegurar que paddleSpeed sea positivo al inicio
  paddleSpeed = abs(paddleSpeed);
  
  // Redibujamos todo
  display.clearDisplay();
  drawBricks();
  drawPaddle();
  drawBall();
  drawGameInfo();
  display.display();
  
  // El juego aún no comienza hasta que el jugador mueva el joystick
  gameStarted = false;
}

// Pantalla de inicio
void showStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(25, 20);
  display.println("BREAKOUT");
  
  display.setCursor(15, 35);
  display.print("Nivel: ");
  display.println(level);
  
  display.setCursor(5, 50);
  display.println("Mueve joystick arriba");
  
  display.display();
  delay(1000);
}

// Pantalla de nivel completado
void showLevelComplete() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(15, 20);
  display.println("NIVEL COMPLETADO");
  
  display.setCursor(15, 35);
  display.print("Puntos: ");
  display.println(score);
  
  display.display();
  
  // Tocar melodía
  playSound(523, 100); // DO
  delay(100);
  playSound(659, 100); // MI
  delay(100);
  playSound(784, 300); // SOL
  
  delay(2000);
  
  // Pasar al siguiente nivel
  level++;
  ballSpeedMultiplier += 0.2;
  
  // Reducir tamaño de la paleta cada nivel
  if (level > 2) {
    PADDLE_WIDTH = paddleInitialWidth - (level - 2);
    PADDLE_WIDTH = max(PADDLE_WIDTH, 10); // Mínimo 10 píxeles
  }
  
  // Aumentar velocidad de la paleta
  paddleSpeed = 3 + (level / 2);
  
  // En niveles superiores, añadir aceleración progresiva
  if (level >= 3) {
    ballDX = (ballDX > 0) ? min(ballDX + 0.2, 2) : max(ballDX - 0.2, -2);
  }
  
  resetLevel();
}

// Pantalla de juego terminado
void showGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(30, 20);
  display.println("GAME OVER");
  
  display.setCursor(15, 35);
  display.print("Puntos: ");
  display.println(score);
  
  display.display();
  
  // Sonido triste
  playSound(196, 300);
  delay(200);
  playSound(147, 600);
  
  delay(3000);
  
  // Reiniciar el juego
  level = 1;
  score = 0;
  lives = 3;
  ballSpeedMultiplier = 1.0;
  PADDLE_WIDTH = paddleInitialWidth; // Restaurar tamaño original de la paleta
  
  showStartScreen();
  resetLevel();
}

// Mover la paleta según el joystick
void movePaddle() {
  int xVal = analogRead(JOY_X);
  
  // Mover la paleta según el joystick
  if (xVal < 400 && paddleX > 0) {
    paddleX -= paddleSpeed;
  } else if (xVal > 600 && paddleX < SCREEN_WIDTH - PADDLE_WIDTH) {
    paddleX += paddleSpeed;
  }
  
  // Asegurar que la paleta no salga de la pantalla
  if (paddleX < 0) paddleX = 0;
  if (paddleX > SCREEN_WIDTH - PADDLE_WIDTH) paddleX = SCREEN_WIDTH - PADDLE_WIDTH;
  
  // Si el juego no ha comenzado, la pelota sigue a la paleta
  if (!gameStarted) {
    ballX = paddleX + (PADDLE_WIDTH / 2) - (BALL_SIZE / 2);
    ballY = PADDLE_Y - BALL_SIZE - 1;
    
    // Comprobar si el jugador pulsa para empezar
    int yVal = analogRead(JOY_Y);
    if (yVal < 400) {
      gameStarted = true;
      playSound(440, 200); // Sonido de inicio
    }
  }
}

// Comprobar colisión con los ladrillos
void checkBrickCollision() {
  // Verificar si hay colisión con algún ladrillo
  int colWidth = BRICK_WIDTH + BRICK_PADDING;
  int rowHeight = BRICK_HEIGHT + BRICK_PADDING;
  
  // Calcular el rango de ladrillos con los que puede colisionar la pelota
  int startCol = max(0, ballX / colWidth);
  int endCol = min(BRICK_COLS - 1, (ballX + BALL_SIZE) / colWidth);
  int startRow = max(0, (ballY - BRICK_TOP_MARGIN) / rowHeight);
  int endRow = min(BRICK_ROWS - 1, (ballY + BALL_SIZE - BRICK_TOP_MARGIN) / rowHeight);
  
  // Verificar cada ladrillo en el rango
  for (int row = startRow; row <= endRow; row++) {
    for (int col = startCol; col <= endCol; col++) {
      if (bricks[row][col]) {
        int brickX = col * colWidth;
        int brickY = BRICK_TOP_MARGIN + row * rowHeight;
        
        // Si hay colisión
        if (ballX + BALL_SIZE > brickX && ballX < brickX + BRICK_WIDTH &&
            ballY + BALL_SIZE > brickY && ballY < brickY + BRICK_HEIGHT) {
          
          // Reducir durabilidad del ladrillo
          bricksDurability[row][col]--;
          
          // Solo eliminar el ladrillo si su durabilidad llega a cero
          if (bricksDurability[row][col] <= 0) {
            bricks[row][col] = false;
            bricksRemaining--;
            
            // Comprobar si es un ladrillo especial
            if (specialBricks[row][col]) {
              // Aplicar efecto especial
              applySpecialEffect(col, row);
            }
          }
          
          // Cambiar dirección de la pelota
          // Determinar desde qué lado se golpeó el ladrillo
          int centerBallX = ballX + BALL_SIZE / 2;
          int centerBallY = ballY + BALL_SIZE / 2;
          int centerBrickX = brickX + BRICK_WIDTH / 2;
          int centerBrickY = brickY + BRICK_HEIGHT / 2;
          
          // Calcular la diferencia entre centros
          float dx = centerBallX - centerBrickX;
          float dy = centerBallY - centerBrickY;
          
          // Si el choque es más horizontal que vertical
          if (abs(dx / BRICK_WIDTH) > abs(dy / BRICK_HEIGHT)) {
            ballDX = -ballDX; // Invertir dirección horizontal
          } else {
            ballDY = -ballDY; // Invertir dirección vertical
          }
          
          // Sonido según durabilidad
          if (bricksDurability[row][col] <= 0) {
            playSound(330, 50);  // Ladrillo destruido
          } else {
            playSound(220, 30);  // Ladrillo golpeado pero no destruido
          }
          
          // Puntaje según nivel y durabilidad
          if (bricksDurability[row][col] <= 0) {
            score += 10 * level * (specialBricks[row][col] ? 2 : 1);
          } else {
            score += 2 * level;
          }
          
          // Verificar si se han eliminado todos los ladrillos
          if (bricksRemaining <= 0) {
            showLevelComplete();
          }
          
          // Solo procesar una colisión por vez
          return;
        }
      }
    }
  }
}

// Mover la pelota
void moveBall() {
  if (!gameStarted) return;
  
  // Calcular el movimiento con la velocidad actual
  int moveX = ballDX * ballSpeedMultiplier;
  int moveY = ballDY * ballSpeedMultiplier;
  
  // Si la velocidad es muy baja, asegurar al menos 1 píxel de movimiento
  if (moveX == 0) moveX = (ballDX > 0) ? 1 : -1;
  if (moveY == 0) moveY = (ballDY > 0) ? 1 : -1;
  
  // Mover la pelota
  ballX += moveX;
  ballY += moveY;
  
  // Comprobar colisiones con los bordes
  // Borde izquierdo y derecho
  if (ballX <= 0) {
    ballX = 0;
    ballDX = -ballDX;
  } else if (ballX >= SCREEN_WIDTH - BALL_SIZE) {
    ballX = SCREEN_WIDTH - BALL_SIZE;
    ballDX = -ballDX;
  }
  
  // Borde superior
  if (ballY <= 10) { // Dejar espacio para la información del juego
    ballY = 10;
    ballDY = -ballDY;
  }
  
  // Colisión con la paleta
  if (ballY + BALL_SIZE >= PADDLE_Y && ballY <= PADDLE_Y + PADDLE_HEIGHT) {
    if (ballX + BALL_SIZE >= paddleX && ballX <= paddleX + PADDLE_WIDTH) {
      // Cambiar dirección según dónde golpea en la paleta
      float hitPoint = (ballX + BALL_SIZE/2) - paddleX;
      float hitRatio = hitPoint / PADDLE_WIDTH;
      
      // Calcular nuevo ángulo de rebote (-1 a 1)
      float angle = 2.0 * hitRatio - 1.0;
      
      // Ajustar dirección horizontal
      ballDX = angle * 2; // Máximo ±2
      
      // Siempre rebote hacia arriba
      ballDY = -abs(ballDY);
      
      // Posición correcta para evitar pegarse a la paleta
      ballY = PADDLE_Y - BALL_SIZE;
      
      // Reproducir sonido cuando choca con la paleta
      playSound(440, 50);
    }
  }
  
  // Comprobar colisión con ladrillos
  checkBrickCollision();
  
  // Pelota cae fuera de la pantalla
  if (ballY > SCREEN_HEIGHT) {
    playSound(196, 300); // Sonido de pérdida
    
    lives--;
    
    if (lives <= 0) {
      // Game over
      showGameOver();
    } else {
      // Reiniciar posición de la pelota
      gameStarted = false;
      ballX = paddleX + (PADDLE_WIDTH / 2) - (BALL_SIZE / 2);
      ballY = PADDLE_Y - BALL_SIZE - 1;
    }
  }
}

// Actualizar la pantalla
void updateDisplay() {
  display.clearDisplay();
  
  // Dibujar elementos del juego
  drawBricks();
  drawPaddle();
  drawBall();
  drawGameInfo();
  
  // Actualizar la pantalla
  display.display();
}

void setup() {
  // Inicializar comunicación serial para depuración
  Serial.begin(9600);
  
  // Inicializar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Error al iniciar SSD1306"));
    for (;;); // No continuar si falla
  }
  
  // Configuración inicial de la pantalla
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Configurar pines
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Inicializar generador de números aleatorios
  randomSeed(analogRead(A5));
  
  // Mostrar pantalla de inicio
  showStartScreen();
  
  // Inicializar el juego
  resetLevel();
}

void loop() {
  // Control de velocidad de fotogramas
  unsigned long currentTime = millis();
  if (currentTime - lastFrameTime < FRAME_DELAY) {
    return;
  }
  lastFrameTime = currentTime;
  
  // Comprobar si hay que revertir efectos temporales
  if (paddleSpeed < 0 && millis() - lastEffectTime > EFFECT_DURATION) {
    paddleSpeed = abs(paddleSpeed);
  }
  
  // Actualizar elementos del juego
  movePaddle();
  moveBall();
  
  // Actualizar la pantalla
  updateDisplay();
}
