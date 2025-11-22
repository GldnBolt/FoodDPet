Descripción de la aplicación web/móvil desarrollada, incluyendo arquitectura de la aplicación, interfaces de usuario, protocolos de comunicación robot-aplicación, y funcionalidades implementadas para control y monitoreo.

# Aplicación web desarrollada

La aplicación web de FoodDPet está diseñada para gestionar el sistema automático de alimentación de mascotas. Permite a los dueños de mascotas programar horarios de comida, monitorear el estado del comedero y mantener un registro histórico de las alimentaciones. La aplicación ha sido construida con tecnologías web modernas y responsivas, pensando en una experiencia de usuario intuitiva que cualquiera pueda usar sin necesidad de conocimientos técnicos.

## Arquitectura de la Aplicación

La aplicación está construida con Angular, un framework moderno y robusto para crear aplicaciones web interactivas. Se complementa con PrimeNG, una librería de componentes visuales profesionales y listos para usar, lo que facilita la creación de interfaces atractivas y funcionales. Para la lógica reactiva y manejo de datos en tiempo real, utiliza RxJS. El código está escrito en TypeScript, un lenguaje que extiende JavaScript con características de programación orientada a objetos, haciendo el código más seguro, mantenible y escalable. Las pruebas se realizan con Jasmine y Karma, herramientas estándar en el ecosistema Angular.

## Interfaces de Usuario

La aplicación presenta dos vistas principales, cada una diseñada para cumplir un propósito específico en la gestión de la alimentación de tu mascota. Además de facilitar el control y monitoreo del comedero por medio de tablas, indicadores e historiales explicados más adelante.

1. **Panel de Control (Dashboard)**

Es la primera vista que ves al abrir la aplicación. Aquí se puede ver el estado actual del comedero automático. Incluye: 
- Indicador visual del nivel de comida representado como un porcentaje de 0 a 100% que tan lleno está el comedero.
- Un estado que clasifica si el comedero está "Bajo", "Medio" o "Alto" de comida.
- Una tabla de solicitudes que muestra un histórico de las últimas veces que se le dio comida a la mascota, incluyendo la hora exacta y si la operación se completó correctamente.

La aplicación permite tambien completar o cancelar manualmente una solicitud de alimentación.

2. **Horario de Alimentación (Food Schedule)**

En esta seccion se define cuándo y cuánta comida quieres que reciba tu mascota. Funciona como un calendario de alimentación. La misma permite:
- Agregar nuevos horarios de alimentación, especificando la hora en la que se desea alimentar a la mascota.
- Ver todos los horarios de alimentación programados.
- Editar horarios de alimentación existentes.
- Activar o desactivar horarios de alimentación.
- Eliminar horarios de alimentación cuando ya no sean necesarios.

---
## Protocolos de Comunicación Robot-Aplicación
