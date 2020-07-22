#ifndef C_PNP
#define C_PNP

#include "stdconst.h"
#include "modules.h"
#include "c_pnp.iom.h"

typedef struct {

} VARSPNP;

void cPnpInit(void *pHeaders);
void cPnpCtrl(void);
void cPnpExit(void);

extern const HEADER cPnp;

#endif //C_PNP
