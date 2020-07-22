#include "c_pnp.iom.h"
#include "c_pnp.h"

static IOMAPPNP IOMapPnp;
static VARSPNP  VarsPnp;
static HEADER   **pHeaders;

const HEADER cPnp = {
    0x000D0001L,
    "Pnp",
    cPnpInit,
    cPnpCtrl,
    cPnpExit,
    &IOMapPnp,
    &VarsPnp,
    sizeof(IOMapPnp),
    sizeof(VarsPnp),
    0x0000
};

void cPnpInit(void *pHeader) {
    pHeaders = pHeader;
}

void cPnpCtrl(void) {

}

void cPnpExit(void) {

}
