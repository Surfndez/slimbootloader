/** @file
  Basic graphics rendering support

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __GRAPHICSLIB_H__
#define __GRAPHICSLIB_H__

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Protocol/GraphicsOutput.h>

//
// Define the maximum message length that this library supports
//
#define MAX_MESSAGE_LENGTH  0x100

enum EFI_COLORS {
  BLACK,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  LIGHTGRAY,
  DARKGRAY,
  LIGHTBLUE,
  LIGHTGREEN,
  LIGHTCYAN,
  LIGHTRED,
  LIGHTMAGENTA,
  YELLOW,
  WHITE,
  EFI_COLORS_MAX
};

extern EFI_GRAPHICS_OUTPUT_BLT_PIXEL Colors[EFI_COLORS_MAX];

#define GLYPH_WIDTH  8
#define GLYPH_HEIGHT 19

typedef struct {
  CHAR16  UnicodeWeight;
  UINT8   Attributes;
  UINT8   GlyphCol1[GLYPH_HEIGHT];
} EFI_NARROW_GLYPH;

extern EFI_NARROW_GLYPH gUsStdNarrowGlyphData[];
extern UINT32 mNarrowFontSize;

typedef struct {
  EFI_PEI_GRAPHICS_INFO_HOB     *GfxInfoHob;
  CHAR8                         *TextDisplayBuf;
  CHAR8                         *TextSwapBuf;
  UINTN                         OffX;
  UINTN                         OffY;
  UINTN                         Width;
  UINTN                         Height;
  UINTN                         Rows;
  UINTN                         Cols;
  UINTN                         CursorX;
  UINTN                         CursorY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor;
} FRAME_BUFFER_CONSOLE;

/**
  Display a *.BMP graphics image to the frame buffer. If a NULL GopBlt buffer
  is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
  buffer is passed in it will be used if it is big enough.

  @param  BmpImage      Pointer to BMP file
  @param  GopBlt        Buffer for transferring BmpImage to the frame buffer.
  @param  GopBltSize    Size of GopBlt in bytes.
  @param  GfxInfoHob    Pointer to graphics info HOB.

  @retval EFI_SUCCESS           GopBlt and GopBltSize are returned.
  @retval EFI_UNSUPPORTED       BmpImage is not a valid *.BMP image
  @retval EFI_BUFFER_TOO_SMALL  The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
  @retval EFI_OUT_OF_RESOURCES  No enough buffer to allocate.

**/
EFI_STATUS
DisplayBmpToFrameBuffer (
  IN     VOID      *BmpImage,
  IN OUT VOID      **GopBlt,
  IN OUT UINTN     *GopBltSize,
  IN     EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob
  );

/**
  Copy image into frame buffer.

  @param[in] GfxInfoHob          Pointer to graphics info HOB
  @param[in] GopBlt              The source image
  @param[in] Width               Width of the source image
  @param[in] Height              Height of the source image
  @param[in] OffX                Desired X offset in frame buffer
  @param[in] OffY                Desired Y offset in frame buffer

  @retval EFI_SUCCESS            Copy was successful
  @retval EFI_INVALID_PARAMETER  Image dimensions fall outside the framebuffer

**/
EFI_STATUS
EFIAPI
BltToFrameBuffer (
  IN EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob,
  IN VOID                      *GopBlt,
  IN UINTN                     Width,
  IN UINTN                     Height,
  IN UINTN                     OffX,
  IN UINTN                     OffY
  );

/**
  Draw a glyph into the frame buffer (ASCII only).

  @param[in] GfxInfoHob          Pointer to graphics info HOB
  @param[in] Glyph               ASCII character to write
  @param[in] ForegroundColor     Foreground color to use
  @param[in] BackgroundColor     Background color to use
  @param[in] OffX                Desired X offset
  @param[in] OffY                Desired Y offset

  @retval EFI_SUCCESS            Success
  @retval EFI_INVALID_PARAMETER  Could not draw entire glyph in frame buffer

**/
EFI_STATUS
EFIAPI
BltGlyphToFrameBuffer (
  IN EFI_PEI_GRAPHICS_INFO_HOB     *GfxInfoHob,
  IN CHAR8                         Glyph,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor,
  IN UINTN                         OffX,
  IN UINTN                         OffY
  );

/**
  Initialize the frame buffer console.

  @param[in] GfxInfoHob          Pointer to graphics info HOB
  @param[in] Width               Width of the console (in pixels)
  @param[in] Height              Height of the console (in pixels)
  @param[in] OffX                Desired X offset of the console
  @param[in] OffY                Desired Y offset of the console

  @retval EFI_SUCCESS            Success
  @retval EFI_INVALID_PARAMETER  Could not fit entire console in frame buffer

**/
EFI_STATUS
EFIAPI
InitFrameBufferConsole (
  IN EFI_PEI_GRAPHICS_INFO_HOB *GfxInfoHob,
  IN     UINTN                 Width,
  IN     UINTN                 Height,
  IN     UINTN                 OffX,
  IN     UINTN                 OffY
  );

/**
  Scroll the console area of the screen up.

  @param[in] ScrollAmount Amount (in pixels) to scroll

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
FrameBufferConsoleScroll (
  IN UINTN                ScrollAmount
  );

/**
  Write data from buffer to graphics framebuffer.

  Writes NumberOfBytes data bytes from Buffer to the framebuffer device.
  The number of bytes actually written to the framebuffer is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written.
                           If this value is less than NumberOfBytes, then the write operation failed.

**/
UINTN
EFIAPI
FrameBufferWrite (
  IN UINT8     *Buffer,
  IN UINTN      NumberOfBytes
  );

#endif
