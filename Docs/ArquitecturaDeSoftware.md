## Arquitectura de Software

El programa principal que es el que controla el microcontrolador está escrito en Arduino/C++. El programa se conecta a la red WiFi para comunicarse con la aplicación web, mide constantemente los sensores para monitorear el nivel de comida y la presencia de la mascota, ejecuta los horarios programados para dispensar comida automáticamente, y guarda todos los datos importantes en memoria permanente para asegurar que no se pierdan.
El mismo está organizado para ser eficiente y responsivo, al cargar el programa este establece la conexion con los sensores, se conecta a la red WiFi, sincroniza la hora con internet, carga los datos almacenados, y luego entra en un bucle infinito donde atiende las solicitudes de la aplicación web. Este flujo se puede observar mejor de la siguiente manera:

![Diagrama de conexiones basico](./images/UntitledDiagram.jpg)


## Estrategias de Comunicación y Gestión de E/S 

Para los dispositivos de entrada y salida del esp 32 se han implementado diversas estrategias para asegurar una comunicación eficiente y confiable con los sensores y actuadores conectados. La decision fundamental fue combinar la comunicación tanto por interrupciones como por sondeo dependiendo del caso de uso. Lo anterior ya que no era factible ya que si se realizara solamente por sondeo el sistema no sabria exactamente cuando realizar la interrupcion sino que verificaria constantemente el estado de los sensores si hay proximidad lo que generaria un consumo innecesario de recursos y energia. Por otro lado, si se utilizara solamente por interrupciones, los sensor generarían una interrupción cada vez que detecte proximidad, interrumpiendo inmediatamente todo lo que el microcontrolador está haciendo. Por lo que se opto por una solución híbrida que combina ambos métodos para aprovechar las ventajas de cada uno. Más adelante se detallan cada una de las estrategias implementadas.

La primera es la utilizacion de contadores de tiempo que permiten ejecutar tareas periódicas sin bloquear el flujo principal del programa.  
Estos timers funcionan en paralelo con el resto del código, lo que significa que el microcontrolador pueda estar respondiendo a la aplicación web mientras simultáneamente está verificando horarios y escaneando sensores lo anterior permite que el sistema sea responsivo y esté sinconizado. 
Para los timers se definen dos, el timer de cada segundo y el del minuto, el inicial es para verificar en tiempo real los horarios programados y el segundo timer es para escanear el sensor ultrasónico cada minuto y detectar si la mascota está pidiendo comida.

Por otro lado, cada dispositivo tiene su propio protocolo de comunicación específico:

1. Sensor Ultrasonico: Este sensor utiliza un protocolo basado en pulsos. El microcontrolador envía un pulso de activación al sensor, que luego emite un pulso ultrasónico para por realizar las medias.

2. Sensor Láser: Este sensor utiliza el protocolo I2C para comunicarse con el microcontrolador por medio de dos cables SDA y SCL que permiten enviar y recibir datos de manera eficiente.

3. Motor: el microcontrolador activa una salida digital que enciende el motor durante el tiempo necesario.

4. Red WiFi: La comunicación con la aplicación web ocurre constantemente. El ESP32 actúa como un servidor HTTP pequeño que recibe solicitudes GET, POST, PUT y DELETE desde la aplicación, las procesa, y responde con datos en formato JSON.


## Organización de Memoria
Para el almacenamiento de datos se utiliza la memoria EEPROM del microcontrolador. Donde se guardan los horarios de alimentación programados por  medio de un json y las peticiones de alimentacion realizadas por medio de archivo de texto log. Para el manejo de estos archivos se utiliza la libreria de littlefs que permite crear un sistema de archivos en la memoria flash del microcontrolador. Se eligió esta manera de almacenamiento debido a su capacidad para manejar múltiples archivos y su facilidad de uso con el microcontrolador. 

La organización de la memoria se realiza de la siguiente manera:      
- Los primeros bytes de la memoria EEPROM se reservan para almacenar la cantidad de horarios programados. Esto permite al sistema saber cuántos horarios debe cargar al iniciar.

- A continuación, se asigna un bloque de memoria para cada horario programado. Cada bloque contiene la hora y minuto lo que facilita la lectura y escritura de horarios específicos sin afectar otros datos.

- Finalmente, se reserva un espacio adicional para almacenar el historial de alimentaciones realizadas. Cada entrada en el historial incluye la fecha y hora de la alimentación, así como el estado de la operación. Esto permite al usuario revisar el historial completo desde la aplicación web.

