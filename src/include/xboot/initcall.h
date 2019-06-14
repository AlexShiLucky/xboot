#ifndef __INITCALL_H__
#define __INITCALL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#define __init __attribute__ ((__section__ (".init.text")))
#define __exit __attribute__ ((__section__ (".exit.text")))

#define __define_initcall(fn, level) \
    static const initcall_t __initcall_##fn##level \
    __attribute__((__used__, __section__(".initcall_" #level ".text"))) = fn

#define __define_exitcall(fn, level) \
    static const exitcall_t __exitcall_##fn##level \
    __attribute__((__used__, __section__(".exitcall_" #level ".text"))) = fn

#define initcall_pure(fn)       __define_initcall(fn, 0)   // 第0级初始化调用段
#define initcall_machine(fn)    __define_initcall(fn, 1)   // 第1级初始化调用段
#define initcall_core(fn)       __define_initcall(fn, 2)   // 第2级初始化调用段
#define initcall_postcore(fn)   __define_initcall(fn, 3)   // 第3级初始化调用段
#define initcall_driver(fn)     __define_initcall(fn, 4)   // 第4级初始化调用段
#define initcall_subsys(fn)     __define_initcall(fn, 5)   // 第5级初始化调用段
#define initcall_command(fn)    __define_initcall(fn, 6)   // 第6级初始化调用段
#define initcall_server(fn)     __define_initcall(fn, 7)   // 第7级初始化调用段
#define initcall_reserver(fn)   __define_initcall(fn, 8)   // 第8级初始化调用段
#define initcall_late(fn)       __define_initcall(fn, 9)   // 第9级初始化调用段

#define pure_initcall(fn)       initcall_pure(fn)
#define machine_initcall(fn)    initcall_machine(fn)
#define core_initcall(fn)       initcall_core(fn)
#define postcore_initcall(fn)   initcall_postcore(fn)
#define driver_initcall(fn)     initcall_driver(fn)
#define subsys_initcall(fn)     initcall_subsys(fn)
#define command_initcall(fn)    initcall_command(fn)
#define server_initcall(fn)     initcall_server(fn)
#define reserver_initcall(fn)   initcall_reserver(fn)
#define late_initcall(fn)       initcall_late(fn)

#define exitcall_pure(fn)       __define_exitcall(fn, 0)   // 第0级退出调用段
#define exitcall_machine(fn)    __define_exitcall(fn, 1)   // 第1级退出调用段
#define exitcall_core(fn)       __define_exitcall(fn, 2)   // 第2级退出调用段
#define exitcall_postcore(fn)   __define_exitcall(fn, 3)   // 第3级退出调用段
#define exitcall_driver(fn)     __define_exitcall(fn, 4)   // 第4级退出调用段
#define exitcall_subsys(fn)     __define_exitcall(fn, 5)   // 第5级退出调用段
#define exitcall_command(fn)    __define_exitcall(fn, 6)   // 第6级退出调用段
#define exitcall_server(fn)     __define_exitcall(fn, 7)   // 第7级退出调用段
#define exitcall_reserver(fn)   __define_exitcall(fn, 8)   // 第8级退出调用段
#define exitcall_late(fn)       __define_exitcall(fn, 9)   // 第9级退出调用段

#define pure_exitcall(fn)       exitcall_pure(fn)
#define machine_exitcall(fn)    exitcall_machine(fn)
#define core_exitcall(fn)       exitcall_core(fn)
#define postcore_exitcall(fn)   exitcall_postcore(fn)
#define driver_exitcall(fn)     exitcall_driver(fn)
#define subsys_exitcall(fn)     exitcall_subsys(fn)
#define command_exitcall(fn)    exitcall_command(fn)
#define server_exitcall(fn)     exitcall_server(fn)
#define reserver_exitcall(fn)   exitcall_reserver(fn)
#define late_exitcall(fn)       exitcall_late(fn)


void do_initcalls(void);
void do_exitcalls(void);

#ifdef __cplusplus
}
#endif

#endif /* __INITCALL_H__ */
