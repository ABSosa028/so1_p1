# Manual Técnico: Módulo del Kernel de Linux

## Descripción
Este módulo del kernel de Linux se encarga de monitorear la memoria y el uso del CPU, así como de listar los procesos y sus hijos. Fue desarrollado para el laboratorio de sistemas operativos 1.

### Información del Módulo
- **Licencia:** GPL
- **Descripción:** Módulo de sistema para laboratorio de sistemas operativos 1
- **Autor:** Dave965 ABSosa028

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
#include <linux/cred.h>          // para __kuid_val()

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
    if err != nil {
        context.IndentedJSON(http.StatusOK, err)
        return
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
import TablaProcesos from './components/tabla_procesos/TablaProcesos.jsx';´´´

Se importan los estilos CSS, los componentes PieChart y TablaProcesos, y los hooks useState y useEffect de React.

##estados del componente

const [mem, setMem] = useState(null);
const [cpu, setCpu] = useState(null);
const [data, setData] = useState(null);
const [optsSim, setOptsSim] = useState([]);
const [simActual, setSimActual] = useState(-1);

##efectos
useEffect(()=>{
  const interval = setInterval(()=> get_datos(), 1000);
  return () => clearInterval(interval);
},[]);

useEffect(()=>{
  const interval = setInterval(()=> get_procesos(), 1000);
  return () => clearInterval(interval);
},[]);

Estos useEffect configuran intervalos para llamar a las funciones get_datos y get_procesos cada segundo.

##funciones principales
function get_datos() {
  fetch('/data')
    .then((res) => res.json())
    .then((d) => {
      console.log(d);
      d = JSON.parse(d);

      const nMem = {
        labels:["En uso", "Libre"],
        label: "%",
        data: [d.memoria_ocupada, 100-d.memoria_ocupada],
        color: ["#6488ea","#282c34"],
        title: "Memoria Ram"
      };

      setMem(nMem);

      let utilizacion = 100*d.cpu_utilizado/2;
      utilizacion = Math.round(utilizacion*100)/100;
      let liberada = Math.round((100-utilizacion)*100)/100;

      if(utilizacion > 100){
        utilizacion = 100;
        liberada = 0;
      }

      const nCpu = {
        labels:["En uso", "Libre"],
        label: "%",
        data: [utilizacion, liberada],
        color: ["#6488ea","#282c34"],
        title: "CPU"
      };
      setCpu(nCpu);

    }).catch(error => {console.log(error);});
}

