


1- Inicializa las apuestas:
a. Total de dinero apostado a cada caballo = 1.0
b. Cotización de cada caballo = <total dinero apostado a todos los
caballos hasta ese momento> / <total del dinero apostado al caballo
hasta ese momento>
c. Dinero a pagar a cada apostador para cada caballo = 0
2- Inicializa tantos threads como ventanillas de gestión de apuestas
3- Recibe mensajes de apuestas en una cola
4- Los mensajes de apuestas que se van recibiendo son procesados por los
threads “ventanilla”
5- Sólo se procesan apuestas hasta el comienzo de la carrera. Está prohibido
procesar ninguna apuesta una vez comenzada la carrera.
6- Cada ventanilla:
a. Asume uno de los mensajes de apuesta
b. Comprueba el caballo de la apuesta
c. Se le asigna al apostador la cantidad que se le pagara en caso de que
el caballo gane = <dinero apostado> * <cotización del caballo>
d. Se actualiza la cotización de los caballos:
i. <Cotización de un caballo> = <total dinero apostado a todos
los caballos> / <total dinero apostado al caballo> 