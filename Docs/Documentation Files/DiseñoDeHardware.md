# Diseño de Hardware
FoodDPet está construido alrededor de un microcontrolador ESP32, elegido por su capacidad de procesamiento, conectividad Wi-Fi integrada y bajo consumo energético. El ESP32 permite una comunicación eficiente con la aplicación web y maneja múltiples sensores y actuadores necesarios para el funcionamiento del robot. Su elegibilidad se basa en su capacidad de soporte de programas complejos, versatilidad y soporte para diversas interfaces de comunicación.

## Sensores y Actuadores

El robot utiliza una variedad de sensores para monitorear su entorno y asegurar un funcionamiento óptimo. Entre ellos se incluyen:

- Sensor ultrasónico HC-SR05: Utilizado para medir la cercanía del alimento a la tapa del robot para monitorear el nivel de comida.

- Sensor Tof400c : Empleado para la detección precisa de presencia del animal cerca del dispensador para activar el pedido de alimento.

- Motor Stepper: Utilizado para mover el tornillo de dispensado de alimento.


## Analisis de Consumo Energético

El consumo energético del robot ha sido cuidadosamente analizado para garantizar una operación eficiente y prolongada. El ESP32 tiene un consumo promedio de 80 mA en modo activo, mientras que los sensores ultrasónicos y el servomotor tienen consumos adicionales que varían según su uso. Se ha implementado un sistema de gestión de energía por medio de reguladores de voltaje y jacks de alimentacion para tener un consumo optimizado y velar por la duración de los componentes.

## Diagramas de Conexión
A continuación se presentan los diagramas de conexión que ilustran cómo se interconectan los diferentes componentes del robot, se utiliza la herramienta de Tinkercad para el diseño de circuitos y simulaciones. Cabe destacar que el diseño final del hardware no es complemente reflejado en Tinkercad, por temas de limitaciones de la herramienta no todos los componentes están disponibles,
pero se utilizó para validar las conexiones y el funcionamiento básico de los componentes:

![Diagrama de conexiones basico](./images/Arqui.pdf)

## Consideraciones de Diseño Mecánico

El mecanismo principal del robot consiste en un tornillo infinito, este es el mecanismo principal que permite el movimiento del dispensador de comida. El tornillo infinito es accionado por un motor paso a paso, que proporciona la precisión necesaria para dispensar cantidades exactas de alimento. Este tornillo está hecho de un material resistente y duradero para soportar el uso continuo y las condiciones ambientales a las que estará expuesto el robot. Se opto por este mecanismo debido a su capacidad para proporcionar un movimiento suave y controlado, lo que es esencial para el correcto funcionamiento del dispensador de comida. Por otro lado, el diseño mecánico también incluye un sistema de montaje para los sensores y actuadores, asegurando que estén posicionados de manera óptima para su funcionamiento. Este sistema de montaje es ajustable, permitiendo adaptarse a diferentes tamaños y tipos de alimentos. Se utilizaron materiales ligeros mediante impresion 3d lo que permite crear estructuras sólidas, resistentes, ligeras economicas y modulares lo que permite realizar ajustes y modificaciones de manera sencilla.


## Diseño de Tarjeta Perforada

El diseño de la tarjeta perforada se ha realizado teniendo en cuenta la disposición óptima de los componentes para minimizar interferencias y facilitar el ensamblaje. Se han utilizado software de diseño de PCB para crear un diseño compacto y eficiente, asegurando que todas las conexiones sean robustas y confiables. Este diseño representa la versión final del hardware del robot FoodDPet, optimizado para su funcionalidad y durabilidad:

![Diagrama de la tarjeta perforada](./images/image0.webp).

## Diseño de la Carcasa

La carcasa del robot ha sido diseñada para proteger los componentes internos y proporcionar una apariencia atractiva y funcional. Se ha utilizado software de modelado 3D para crear un diseño que sea fácil de ensamblar y mantener. La carcasa está hecha de materiales resistentes y ligeros como lo es el PLA. El diseño de la carcasa también incluye aberturas estratégicas para la ventilación y el acceso a los componentes internos, facilitando el mantenimiento y las actualizaciones futuras.

![Modelo3D](./images/image.png)


