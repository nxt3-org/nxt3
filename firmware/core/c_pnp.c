#include <hal_general.h>
#include "c_pnp.iom.h"
#include "c_pnp.h"
#include "hal_pnp.h"

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
    if (!Hal_Pnp_RefAdd())
        Hal_General_AbnormalExit("Cannot initialize PNP manager");
}

void cPnpCtrl(void) {
    Hal_Pnp_Tick();
}

void cPnpExit(void) {
    if (!Hal_Pnp_RefDel())
        Hal_General_AbnormalExit("Cannot initialize PNP manager");
}
