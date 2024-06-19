# Manual Técnico: Módulo del Kernel de Linux y Plataforma de Monitoreo

## Descripción del Proyecto
Este proyecto tiene como objetivo implementar un sistema de monitoreo de recursos y gestión de procesos en una máquina virtual Ubuntu Server 22.04 utilizando KVM. El sistema incluye módulos del kernel para obtener información del sistema, una API en Go para manejar datos y un frontend para la visualización.

## Arquitectura Empleada
### Componentes Principales

1. **Módulos del Kernel**
   - **Módulo de RAM**: Este módulo se encarga de monitorear el uso de la memoria RAM y se aloja en el directorio `/proc/ram_so1_jun2024`. Utiliza la librería `<linux/mm.h>` para obtener información del sistema.
   - **Módulo de CPU**: Este módulo monitorea el uso del CPU y se aloja en el directorio `/proc/cpu_so1_1s2024`. Utiliza la librería `<linux/sched.h>` para obtener información del sistema.

2. **API en Go**
   - Desarrollada usando el framework Gin.
   - Funciones principales:
     - **StartProcess**: Inicia un proceso que ejecuta `sleep infinity` y devuelve el PID.
     - **KillProcess**: Termina un proceso dado su PID.
   - Conexión a una base de datos MongoDB para almacenar datos de monitoreo.
   - Manejo de CORS para permitir solicitudes desde el frontend.

3. **Base de Datos**
   - Utiliza MongoDB dentro de un contenedor Docker.
   - Almacena datos sobre procesos, uso de memoria RAM y CPU.
   - Persistencia garantizada mediante un volumen Docker.

4. **Frontend**
   - Desarrollado en un framework de elección del estudiante.
   - Características:
     - **Monitoreo en Tiempo Real**: Gráficas de uso de memoria RAM y CPU.
     - **Tabla de Procesos**: Lista de procesos y sus hijos, con botones para crear y eliminar procesos.

5. **Docker Compose**
   - Utilizado para orquestar los contenedores de la plataforma de monitoreo.
   - Facilita la gestión y el despliegue de múltiples contenedores.

## Módulos de Kernel
### Módulo de RAM
- **Descripción**: Monitorea el uso de la memoria RAM.
- **Ubicación**: `/proc/ram_so1_jun2024`
- **Características**:
  - Utiliza `<linux/mm.h>` para obtener información de memoria.
  - Proporciona datos sobre memoria total, libre y usada.

### Módulo de CPU
- **Descripción**: Monitorea el uso del CPU.
- **Ubicación**: `/proc/cpu_so1_1s2024`
- **Características**:
  - Utiliza `<linux/sched.h>` para obtener información del uso del CPU.
  - Proporciona datos sobre la carga promedio del CPU.

### Ejemplo de Código del Módulo de RAM
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

const long megabyte = 1024 * 1024;

struct sysinfo si;

static void init_meminfo(void) {
    si_meminfo(&si);
}

static int escribir_archivo(struct seq_file *archivo, void *v) {
    init_meminfo();
    unsigned long memoria_ocupada = ((si.totalram - si.freeram) * si.mem_unit) / megabyte;
    unsigned long totalram_mb = (si.totalram * si.mem_unit) / megabyte;

    seq_printf(archivo, "{ \"memoria_ocupada\": %lu, \"memoria_total\": %lu }",
               memoria_ocupada, totalram_mb);
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modulo de RAM, Laboratorio sistemas operativos 1");
MODULE_AUTHOR("Dave965");


## Código Fuente

### Inclusión de Cabeceras
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/sched/loadavg.h>
#include <linux/sched/signal.h>  // para for_each_process()
#include <linux/cred.h>          // para __kuid_val() c´´´

##Constantes

const long megabyte = 1024 * 1024;

struct sysinfo si;
struct task_struct *task;
struct task_struct *task_child;
struct list_head *list;

##Funcion de inicializacion de informacion de memoria
static void init_meminfo(void) {
    si_meminfo(&si);
}

##Informacion del modulo

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modulo de sistema, Laboratorio sistemas operativos 1");
MODULE_AUTHOR("Dave965");

##funcion para escribir en el archivo

static int escribir_archivo(struct seq_file *archivo, void *v) {
    init_meminfo();

    unsigned long memoria_ocupada = ((si.totalram - si.freeram) * si.mem_unit) / megabyte;
    unsigned long totalram_mb = (si.totalram * si.mem_unit) / megabyte;

    seq_printf(archivo, "{ \"memoria_ocupada\": %lu, \"cpu_utilizado\": %lu.%02lu, \"procesos\": [",
               100 - (memoria_ocupada * 100 / totalram_mb), LOAD_INT(avenrun[0]), LOAD_FRAC(avenrun[0]));

    int first_process = 1;  // Variable para manejar la primera coma
    struct task_struct *task;
    for_each_process(task) {
        if (!first_process) {
            seq_printf(archivo, ",");
        } else {
            first_process = 0;
        }

        // Obtener el estado del proceso usando task_state_to_char
        char estado = task_state_to_char(task);
        char estado_str[2] = {estado, '\0'};

        // Imprimir varios atributos del objeto task
        seq_printf(archivo, "{ \"nombre\": \"%s\", \"pid\": %d, \"estado\": \"%s\", \"hijos\": [",
                   task->comm, task->pid, estado_str);

        int first_child = 1;  // Variable para manejar la primera coma en los hijos
        struct list_head *list;
        struct task_struct *task_child;

        list_for_each(list, &task->children) {
            if (!first_child) {
                seq_printf(archivo, ",");
            } else {
                first_child = 0;
            }

            task_child = list_entry(list, struct task_struct, sibling);
            seq_printf(archivo, "%d", task_child->pid);
        }
        seq_printf(archivo, "]}");
    }
    seq_printf(archivo, "]}");
    return 0;
}

##Operaciones de archivo

static struct proc_ops operaciones = {
    .proc_open = al_abrir,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release
};


#Backend
##Servicio Web en Go
##Descripción
Este servicio web está escrito en Go y utiliza el framework Gin para manejar las rutas HTTP. El servicio permite iniciar y detener procesos en el sistema operativo y proporciona una ruta para obtener datos del archivo /proc/p1_202113378. También se conecta a una base de datos MySQL y establece las cabeceras CORS necesarias.

##funcion StartProcess

Inicia un proceso que ejecuta sleep infinity y devuelve el PID del proceso iniciado.


func StartProcess(context *gin.Context) {
    cmd := exec.Command("sleep", "infinity")
    cmd.Start()
    process = cmd
    var res pidReq
    res.Pid = process.Process.Pid
    context.IndentedJSON(http.StatusOK, res)
}


##funcion killProcess

func KillProcess(context *gin.Context) {
    var req pidReq
    context.BindJSON(&req)
    cmd := exec.Command("kill", "-9", strconv.Itoa(req.Pid))
    err := cmd.Run()
    if (err != nil) {
        context.IndentedJSON(http.StatusOK, err)
        return;
    }
    context.IndentedJSON(http.StatusOK, "Proceso terminado")
}

##Funcion main
func main() {
    db, err := sql.Open("mysql", "root:123@tcp(db:3306)/historicos")
    if err != nil {
        panic(err.Error())
    }
    defer db.Close()

    router := gin.Default()

    router.Use(func(c *gin.Context) {
        c.Writer.Header().Set("Access-Control-Allow-Origin", "*")
        c.Writer.Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE")
        c.Writer.Header().Set("Access-Control-Allow-Headers", "Origin, Content-Type, Accept")
        c.Writer.Header().Set("Access-Control-Allow-Credentials", "true")

        if c.Request.Method == "OPTIONS" {
            c.AbortWithStatus(200)
            return
        }

        c.Next()
    })

    router.GET("/data", getDatos)
    router.GET("/iniciarP", StartProcess)
    router.POST("/terminarP", KillProcess)

    router.Run(":8080")
}

###Función StartProcess: Inicia un proceso que ejecuta sleep infinity, almacena el comando en process y devuelve el PID del proceso iniciado en formato JSON.
###Función KillProcess: Recibe un PID en una solicitud JSON, ejecuta el comando kill -9 para terminar el proceso correspondiente y devuelve un mensaje de confirmación.

#FrontEnd
# Explicación del Código

Este código es un componente de una aplicación React que monitorea el uso de memoria y CPU en tiempo real y muestra una tabla de procesos filtrados. La aplicación tiene dos secciones principales: una para gráficos de pastel (PieChart) y otra para la tabla de procesos (TablaProcesos).

## Importaciones

```javascript
import './App.css';
import PieChart from './components/pie_chart/pie_chart.jsx';
import {useState, useEffect} from 'react';
import TablaProcesos from './components/tabla_procesos/TablaProcesos.jsx';
