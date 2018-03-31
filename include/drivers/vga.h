
#ifndef __MYOS__DRIVERS__VGA_H
#define __MYOS__DRIVERS__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>

namespace myos
{
    namespace drivers{

        class VideoGraphicsArray
        {
        protected:
            hardwarecommunication::Port8Bit miscPort;
            hardwarecommunication::Port8Bit crtcIndexPort;
            hardwarecommunication::Port8Bit crtcDataPort;
            hardwarecommunication::Port8Bit sequencerIndexPort;
            hardwarecommunication::Port8Bit sequencerDataPort;
            hardwarecommunication::Port8Bit graphicsControllerIndexPort;
            hardwarecommunication::Port8Bit graphicsControllerDataPort;
            hardwarecommunication::Port8Bit attributesControllerIndexPort;
            hardwarecommunication::Port8Bit attributesControllerReadPort;
            hardwarecommunication::Port8Bit attributesControllerWritePort;
            hardwarecommunication::Port8Bit attributesControllerResetPort;

            void WriteRegisters(common::uint8_t *registers);
            common::uint8_t* GetFrameBufferSegment();
            virtual common::uint8_t GetColorIndex(common::uint8_t r, common::uint8_t g, common::uint8_t b);


        public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();

            virtual bool SupportsMode(int width, int height, int colordepth);
            virtual bool SetMode(int width, int height, int colordepth);
            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t r, common::uint8_t g, common::uint8_t b);
            virtual void PutPixel(common::uint32_t x, common::uint32_t y, common::uint8_t colorIndex);
        };

    }
}

#endif
