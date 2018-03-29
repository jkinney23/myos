#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager *manager)
: InterruptHandler(0x2C, manager),
  dataport(0x60),
  commandport(0x64)
{
}

MouseDriver::~MouseDriver()
{
}

void MouseDriver::Activate()
{
	offset = 0;
	buttons = 0;
	x = 40;
	y = 12;
	
	static uint16_t* VideoMemory = (uint16_t*)0xb8000;	
	VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
						| ((VideoMemory[80*y+x] & 0x0f00) << 4)
						| ((VideoMemory[80*y+x] & 0x00ff));

	commandport.Write(0xa8);	// activate interrupts
	commandport.Write(0x20);	// get current state
	uint8_t status = dataport.Read() | 2;
	commandport.Write(0x60);	// set state
	dataport.Write(status);
	
	commandport.Write(0xd4);
	dataport.Write(0xf4);
	dataport.Read();
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
	uint8_t status = commandport.Read();
	if (!(status & 0x20))
		return esp;
		
	static int8_t x=40, y=12;
	
	buffer[offset] = dataport.Read();
	offset = (offset + 1) % 3;
	
	if (offset == 0)
	{
		static uint16_t* VideoMemory = (uint16_t*)0xb8000;
		
		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff)); 
		
		x += buffer[1];
		if (x < 0) x = 0;
		if (x >= 80) x = 79;
		
		y -= buffer[2];
		if (y < 0) y = 0;
		if (y >= 25) y = 24;
		
		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
							| ((VideoMemory[80*y+x] & 0x0f00) << 4)
							| ((VideoMemory[80*y+x] & 0x00ff));
		
		for (uint8_t i = 0; i < 3; i++)
		{
			if ((buffer[0] & (0x01 << i)) != (buttons & (0x01<<i)))
			{
				// do something on button press...
				VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >> 4)
									| ((VideoMemory[80*y+x] & 0x0f00) << 4)
									| ((VideoMemory[80*y+x] & 0x00ff));
			}
			
			
		}
		buttons = buffer[0];
	}

			
	return esp;
}
