#include "types.h"

void printf(char * str)
{
	static uint16_t* VideoMemory = (uint16_t*)0xb8000;

	for(int i = 0; str[i] != '\0'; ++i)
		VideoMemory[i] = (VideoMemory[i] & 0xff00) | str[i];
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
	for (constructor* i = &start_ctors; i != &end_ctors; i++)
		(*i)();
}


extern "C" void kernelMain(void *multiboot_structure, uint32_t magicnumber)
{

	printf("Hello World! --- https://jkinney23.github.io/project0-jkinney23/");

	while(1);

}
