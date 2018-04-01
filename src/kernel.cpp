
#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::gui;


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


class MouseToConsole : public MouseEventHandler
{
	int8_t x, y;
public:

	MouseToConsole()
	{
		static uint16_t* VideoMemory = (uint16_t*)0xb8000;
		x = 40;
		y = 12;
		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff));
	}


	void OnMouseMove(int xoffset, int yoffset)
	{
		static uint16_t* VideoMemory = (uint16_t*)0xb8000;

		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff));

		x += xoffset;
		if (x < 0) x = 0;
		if (x >= 80) x = 79;

		y += yoffset; // !!! is it -= or +=???
		if (y < 0) y = 0;
		if (y >= 25) y = 24;

		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff));
	}

	void OnMouseUp(int i)
	{
		static uint16_t* VideoMemory = (uint16_t*)0xb8000;
		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff));
	}
	void OnMouseDown(int i)
	{
		static uint16_t* VideoMemory = (uint16_t*)0xb8000;
		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff));
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

	Desktop desktop(320, 200, 0x00, 0xa8, 0xA8);

	DriverManager drvManager;

		//PrintfKeyboardEventHandler kbhandler;
		//KeyboardDriver keyboard(&interrupts, &kbhandler);
		KeyboardDriver keyboard(&interrupts, &desktop);
		drvManager.AddDriver(&keyboard);

		//MouseToConsole mousehandler;
		//MouseDriver mouse(&interrupts, &mousehandler);
		MouseDriver mouse(&interrupts, &desktop);
		drvManager.AddDriver(&mouse);

		PeripheralComponentInterconnectController PCIController;
		PCIController.SelectDrivers(&drvManager, &interrupts);

		VideoGraphicsArray vga;


	printf("Initializing Hardware, Stage 2\n");
	drvManager.ActivateAll();

	printf("Initializing Hardware, Stage 3\n");

	vga.SetMode(320, 200, 8);
	//vga.FillRectangle(0, 0, 320, 200, 0x00, 0xa8, 0xa8);
	Window win1(&desktop, 10, 10, 20, 20, 0xa8, 0x00, 0x00);
	desktop.AddChild(&win1);
	Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xa8, 0x00);
	desktop.AddChild(&win2);
	interrupts.Activate();

	while(1)
	{
		desktop.Draw(&vga);
	}

}
