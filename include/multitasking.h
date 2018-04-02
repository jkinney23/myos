
#ifndef __MYOS__MULTITASKING_H
#define __MYOS__MULTITASKING_H

#include <common/types.h>
#include <gdt.h>

namespace myos
{
    struct CPUState
    {
        common::uint32_t eax;   // accumulator register
        common::uint32_t ebx;   // base register
        common::uint32_t ecx;   // counting register
        common::uint32_t edx;   // data register

        common::uint32_t esi;   // stack index
        common::uint32_t edi;   // data index
        common::uint32_t ebp;   // stack base pointer

        common::uint32_t gs;   // ? segment
        common::uint32_t fs;   // ? segment
        common::uint32_t es;   // ? segment
        common::uint32_t ds;   // data segment

        common::uint32_t error; // error code

        common::uint32_t eip;   // instruction pointer
        common::uint32_t cs;    // code segment
        common::uint32_t eflags;   // flags
        common::uint32_t esp;   // stack pointer
        common::uint32_t ss;    // stack segment


    } __attribute__((packed));

    class Task
    {
    friend class Taskmanager;
    private:
        common::uint8_t stack[4096]; // 4 KiB
        CPUState *cpustate;
    public:
        Task(GlobalDescriptorTable *gdt, void entrypoint());
        ~Task();
    };


    class TaskManager
    {
    private:
        Task* tasks[256];
        int numTasks;
        int currentTask;

    public:
        TaskManager();
        ~TaskManager();
        bool AddTask(Task *task);
        CPUState* Schedule(CPUState *cpustate);
    };

}

#endif
