#include "WeMakeSoftware.h"
struct IEE802_3Exchange{
     struct Frame*Frame;
     uint16_t Size;
     char*Buffer;
}
extern int WeMakeSoftwareIEE802_3Exchange(struct IEE802_3Exchange iEE802_3Exchange); 