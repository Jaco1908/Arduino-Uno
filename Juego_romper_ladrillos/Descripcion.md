# 🎮 Breakout con Arduino y OLED

Este proyecto implementa el clásico juego **Breakout** utilizando un **Arduino Mega**, una pantalla **OLED SSD1306** y un **joystick analógico** para el control. Además, incluye un buzzer para efectos de sonido.

## 🛠️ Componentes necesarios

- **Arduino uno** 
- **Pantalla OLED SSD1306** (128x64 píxeles, comunicación I2C)
- **Joystick analógico** (Eje X y Y)
- **Buzzer pasivo** (para efectos de sonido)
- **Cables de conexión**

## 🎯 Características principales

✅ **Control con joystick**: Mueve la paleta para evitar que la bola caiga.  
✅ **Pantalla OLED**: Visualización clara del juego, incluyendo ladrillos, la paleta y la pelota.  
✅ **Ladrillos con diferentes resistencias**: Algunos requieren más golpes para romperse.  
✅ **Efectos especiales**: Ladrillos que pueden cambiar la velocidad de la pelota, reducir/aumentar el tamaño de la paleta o invertir los controles.  
✅ **Varios niveles de dificultad**: A medida que avanzas, la velocidad y la dificultad aumentan.  
✅ **Efectos de sonido**: Se utilizan tonos para eventos clave (golpes, victoria, derrota).  
✅ **Pantallas de inicio y fin de juego**: Interfaz sencilla para mejorar la experiencia de usuario.  


## 🚀 Mejoras futuras

- Soporte para pantalla OLED de mayor tamaño.
- Implementación de un sistema de puntajes altos.
- Modo multijugador con dos joysticks.
