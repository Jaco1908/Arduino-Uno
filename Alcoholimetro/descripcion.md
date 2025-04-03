# Proyecto de Sensor de Alcohol con Alarma y Teclado

Este proyecto utiliza un **Arduino Uno** junto con un **sensor de alcohol**, una **pantalla OLED** y un **teclado matricial 4x4** para medir el nivel de alcohol en sangre y activar una alarma cuando se superan ciertos límites. El sistema también permite desactivar la alarma mediante una contraseña.

## Descripción del Proyecto

El sistema está diseñado para detectar el nivel de alcohol en sangre y mostrarlo en una pantalla OLED en tiempo real. Dependiendo del valor de alcohol medido, el sistema activa diferentes LEDs que indican el nivel de riesgo, y un **buzzer** suena cuando se supera un límite crítico. Además, se utiliza un **teclado matricial 4x4** para ingresar una contraseña que permite desactivar la alarma.

### Funciones Principales:
- **Medición del alcohol**: El sensor mide el nivel de alcohol en sangre y convierte el valor analógico en gramos por litro (g/L).
- **Alarma visual y sonora**: Se activan LEDs según el nivel de alcohol y un buzzer suena cuando el nivel es alto.
- **Pantalla OLED**: Muestra el nivel de alcohol y el estado actual del sistema.
- **Teclado para desactivar alarma**: Un teclado 4x4 permite ingresar una contraseña para desactivar la alarma.

### Niveles de Alcohol:
- **Sin riesgo**: Menos de 0.2 g/L
- **Efectos leves**: 0.2 a 0.5 g/L
- **Límite legal**: 0.5 a 0.8 g/L
- **Alteraciones**: 0.8 a 1.0 g/L
- **Deterioro severo**: 1.0 a 3.0 g/L
- **Riesgo de coma**: 3.0 a 5.0 g/L
- **Riesgo de muerte**: Más de 5.0 g/L

### Componentes Utilizados:
- **Arduino Uno**: Microcontrolador principal que gestiona todo el sistema.
- **Sensor de alcohol**: Mide el nivel de alcohol en sangre.
- **Pantalla OLED**: Muestra los datos del alcohol y el estado de la alarma.
- **Teclado 4x4**: Permite ingresar una contraseña para desactivar la alarma.
- **LEDs y buzzer**: Indicadores visuales y sonoros para alertar al usuario.

### Conexiones:
- **Sensor de alcohol**: Conectado al pin analógico A0 de Arduino.
- **Pantalla OLED**: Conectada a través de I2C (pines SDA y SCL).
- **Teclado 4x4**: Conectado a los pines digitales de Arduino.
- **LEDs y buzzer**: Conectados a los pines digitales de Arduino.

## Código:
El código utiliza las siguientes bibliotecas para funcionar correctamente:
- **Wire.h**: Comunicación I2C para la pantalla OLED.
- **Adafruit_SSD1306.h**: Control de la pantalla OLED.
- **Adafruit_GFX.h**: Funciones gráficas para la pantalla OLED.
- **Keypad.h**: Gestión del teclado 4x4.

El código lee el valor analógico del sensor de alcohol, lo convierte en g/L y muestra el resultado en la pantalla OLED. Además, controla los LEDs y el buzzer en función del nivel de alcohol y permite desactivar la alarma ingresando la contraseña correcta en el teclado.

---

## Autor:
Este proyecto fue desarrollado por Pamela Carriel.

¡Gracias por revisar este proyecto! Si tienes alguna duda o sugerencia, no dudes en contactarme.

