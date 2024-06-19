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

const long megabyte = 1024 * 1024;

struct sysinfo si;
struct task_struct *task;
struct task_struct *task_child;
struct list_head *list;

static void init_meminfo(void) {
    si_meminfo(&si);
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Modulo de sistema, Laboratorio sistemas operativos 1");
MODULE_AUTHOR("Dave965");

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


static int al_abrir(struct inode *inode, struct file *file) {
    return single_open(file, escribir_archivo, NULL);
}

static struct proc_ops operaciones = {
    .proc_open = al_abrir,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release
};

static int __init _insert(void) {
    proc_create("p1_202113378", 0, NULL, &operaciones);
    printk(KERN_INFO "202113378\n");
    return 0;
}

static void __exit _remove(void) {
    remove_proc_entry("p1_202113378", NULL);
    printk(KERN_INFO "Modulo de sistema, Laboratorio sistemas operativos 1\n");
}

module_init(_insert);
module_exit(_remove);
