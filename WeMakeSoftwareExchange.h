#pragma once
typedef int (*CloseFrameFunc)(struct Frame* frame);
typedef int (*DropFrameFunc)(struct Frame* frame);
typedef int (*DropAndCloseFrameFunc)(struct Frame* frame);
typedef struct Frame* (*CreateFrameFunc)(uint8_t id);
typedef int (*SetSizeFrameFunc)(struct Frame* frame, uint16_t Size);
typedef int (*SendFrameFunc)(struct Frame* frame);
extern void IEE802_3Setup(CloseFrameFunc closeFrame,DropFrameFunc dropFrame,DropAndCloseFrameFunc dropAndCloseFrame,CreateFrameFunc createFrame,SetSizeFrameFunc setSizeFrame,SendFrameFunc sendFrame);
extern int IEE802_3In(struct Frame* frame);