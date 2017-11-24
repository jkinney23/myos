#ifndef __KEYBOARD_H
#define __KEYBOARD_H

<<<<<<< HEAD
=======

>>>>>>> refs/remotes/origin/master
#include "types.h"
#include "interrupts.h"
#include "port.h"

<<<<<<< HEAD
class KeyboardDriver : public InterruptHandler
{
	Port8Bit dataport;
	Port8Bit commandport;
public:
	KeyboardDriver(InterruptManager* manager);
	~KeyboardDriver();
	virtual uint32_t HandleInterrupt(uint32_t esp);
};

=======

class KeyboardDriver : public InterruptHandler
{
    Port8Bit dataport;
    Port8Bit commandport;
public:
    KeyboardDriver(InterruptManager* manager);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
};


>>>>>>> refs/remotes/origin/master
#endif
