#ifndef IMAGETILE_H
#define IMAGETILE_H

#include "defs.h"

class ImageTile
{
    public:
        ImageTile();
        int intItlId   = -1;
        int intPrjRef  = -1;
        int intSesRef  = -1;
        int intSyncRef = -1;
        int intCam     = -1;
        QString qstrCamRef = TK_QSTR_NONE;
        int intSector = -1;
        int intQlbRef = -1;
        int intImgX = -1;
        int intImgY = -1;
        int intImgW = -1;
        int intImgH = -1;
        int intSeen = -1;
        int uid = -1;
        int gid = -1;
};

#endif // IMAGETILE_H
