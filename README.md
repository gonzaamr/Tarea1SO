# Tarea 1 sistemas operativos

## Descripción

El sistema implementa un chat multiusuario en C mediante **FIFOs**.  
Consta de tres programas principales:

- **Servidor:** gestiona las conexiones, crea canales exclusivos para cada cliente y reenvía los mensajes entre usuarios.
- **Usuario:** permite conectarse al chat, enviar mensajes y ejecutar comandos especiales .
- **Report:** administra las denuncias de usuarios y finaliza procesos cuando superan un número límite de reportes.

## Funcionamiento

El servidor centraliza la comunicación: cada usuario se conecta enviando un mensaje de inicio, tras lo cual recibe un FIFO propio para la recepción de mensajes. Los textos enviados se reenvían a todos los demás clientes conectados. El módulo de reportes recibe las denuncias y controla la moderación de usuarios.
<p align="center">
  <img src="diagrama" alt="Texto alternativo" width="400">
</p>

## Casos borde y soluciones

- **Desconexión de usuarios:** se elimina el FIFO correspondiente para evitar errores.  
- **Reportes a usuarios inexistentes:** el sistema informa la inexistencia del PID en lugar de fallar.  
- **Lecturas vacías en FIFOs:** se manejan con pausas (`usleep`) para prevenir bloqueos.  

De esta manera, el chat se mantiene estable incluso en situaciones inesperadas.


## Uso e instalación 

### **Paso 1: Clonar el repositorio**  
```sh  
git clone https://github.com/gonzaamr/Tarea1SO
cd Tarea1SO 
```

### **Paso 2: Compila el codigo**  
Usa `gcc` para compilar:  
```sh  
gcc servidor.c -o servidor
gcc report.c -o report
gcc usuario.c -o usuario  
```

### **Paso 3: Ejecuta los archivos**  

#### **En una terminal inicia el servidor**  
```sh  
./servidor  
```

#### **En otra terminal ejecuta el report**  
```sh  
./report  
```

#### **Por ultimo inicia la cantidad de usuarios que desees iniciar**
```sh  
./usuario  
```
Ahora puedes enviar chats entre usuarios.
Es necesario que los archivos se ejecuten en ese orden.
## Funciones

### Reportar usuarios

#### Para reportar usuarios ingresa el siguiente comando
```sh  
/report  
```
#### Para crear una copia ingresa el siguiente comando
```sh  
/clonar  
```
#### Para cerrar el chat ingresa el siguiente comando
```sh  
/exit
```

##  Requerimientos  
- Linux   
- GCC Compiler (`sudo apt install gcc` para Ubuntu/WSL)  
