# FoodDPet - Alimentador Automático para Mascotas
## Proposito del Proyecto
FoodDPet es un sistema automatizado diseñado para facilitar la alimentación de mascotas de manera inteligente y controlada. Este proyecto integra hardware embebido y una aplicación web para proporcionar una solución completa que permite a los dueños de mascotas programar y monitorear la alimentación de sus animales de compañía de forma remota.


### DEPENDENCIAS NECESARIAS

### Aplicación Web
- Node.js con npm
- Angular

Instalar Node.js y ejecutar el siguiente comando para instalar las dependencias

``` bash
 npm install 
 npm install -g @angular/cli
```
### Software del microcontrolador

Instalar el IDE de Arduino, e instalar la siguiente librerias: 
- Adafruit VL53L1X por Adafruit

Además de añadir el soporte para tarjetas:
- esp32 por Espressif Systems

### PASOS DE EJECUCIÓN

### Aplicación Web

Dentro de la carpeta WebApp, ejecutar el siguiente comando para abrir la aplicación en modo desarrollo

```bash 
ng serve -o
```

### Software del microcontrolador

Seleccionar desde las tarjetas disponibles en el IDE de Arduino: ESP32S3 Dev Module.

Reemplazar las variables ssid y password por las credenciales de la red a la que se va a conectar el alimentador.

Compilar y subir el código desde el IDE.

Introducir en la aplicación la IP que se imprime después de iniciar el sistema.

### LISTA DE MATERIALES CON COSTOS 
- ESP32: $15,95
- Sensor ultrasónico HC-SR05: $4.49 
- Sensor laser Tof400c: $9.95 
- Stepper Driver A4988: $5.99
- Motor Stepper: $8.30
- Placa Perforada: $3.75
- Cautín y estaño: $24.95
- Transistor NMOS: $0.25
- Pulsador de 4 pines: $0.20
- 2 Resistencias de 220 ohmios: $0.10
- Jumpers: $2.20
- Modelo 3d(Cascasa exterior): $20.00

### DIAGRAMAS DEL CIRCUITO

![Diagrama del circuito](ruta/al/diagrama.png)

### DEMOSTRACIONES DE USO
En el siguiente enlace se puede observar una demostración del funcionamiento del alimentador automático FoodDPet:
[Demostración FoodDPet](https://www.youtube.com/@fabiola8169)

### Información Adicional
Para más detalle sobre el diseño del proyecto, incluyendo el diseño de hardware, arquitectura de software, aplicación de usuario y comunicación, por favor consulte la documentación completa en el archivo Documentacion.md. Esta documentación incluye descripciones detalladas de los componentes utilizados y demás explicaciones técnicas, así como los diagramas de flujo del software y conexiones de hardware.

Se recomienda además utilizar herramientas de simulación y diseño como Tinkercad para validar los diagramas y conexiones antes de la implementación física del proyecto. Para este proyecto, puede encontrar los mismos en la documentación adjunta. 

Para los diseños 3d de la carcasa se sugiere el uso del software de fusion360 o cualquier otro software de modelado 3d de su preferencia. En la documentación se incluyen los archivos necesarios para la impresión 3d de la carcasa del robot. Se recomienda ajustar las dimensiones según las necesidades específicas del usuario. 
