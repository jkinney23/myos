#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "driver.h"
#include "keyboard.h"
#include "mouse.h"

void printf(char * str)
{
	static uint16_t* VideoMemory = (uint16_t*)0xb8000;

	static uint8_t x = 0, y = 0;

	for(int i = 0; str[i] != '\0'; ++i)
	{
		switch(str[i])
		{
			case '\n':
				x = 0;
				y++;
				break;
			default:
				VideoMemory[80*y + x] = (VideoMemory[80*y + x] & 0xff00) | str[i];
				x++;
				break;
		}
		
		if (x >= 80)
		{
			y++;
			x = 0;
		}

		if (y >= 25)
		{
			for (y = 0; y < 25; y++)
				for (x = 0; x < 80; x++)
					VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xff00) | ' ';
			x = 0;
			y = 0;
		}
	}
}


void printfHex(uint8_t key)
{
	char* message = "00";
	char* hex = "0123456789ABCDEF";
	message[0] = hex[(key >> 4) & 0x0F];
	message[1] = hex[key & 0x0F];
	printf(message);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
	void OnKeyDown(char c)
	{
		char* foo = " ";
		foo[0] = c;
		printf(foo);
	}
};


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

	printf("Hello World! --- https://jkinney23.github.io/project0-jkinney23/\n");

	GlobalDescriptorTable gdt;
	InterruptManager interrupts(&gdt);
	
	printf("Initializing Hardware, Stage 1\n");

	DriverManager drvManager;
	
	PrintfKeyboardEventHandler kbhandler;
	KeyboardDriver keyboard(&interrupts, &kbhandler);
	drvManager.AddDriver(&keyboard);
	
	MouseDriver mouse(&interrupts);
	drvManager.AddDriver(&mouse);
	
	printf("Initializing Hardware, Stage 2\n");
	drvManager.ActivateAll();

	printf("Initializing Hardware, Stage 3\n");	
	interrupts.Activate();

	while(1);

}
