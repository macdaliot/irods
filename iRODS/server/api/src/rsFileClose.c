/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* This is script-generated code (for the most part).  */ 
/* See fileClose.h for a description of this API call.*/

#include "fileClose.h"
#include "miscServerFunct.h"
#include "rsGlobalExtern.h"

// =-=-=-=-=-=-=-
// eirods includes
#include "eirods_log.h"
#include "eirods_file_object.h"
#include "eirods_stacktrace.h"


int
rsFileClose (rsComm_t *rsComm, fileCloseInp_t *fileCloseInp)
{
    rodsServerHost_t *rodsServerHost;
    int remoteFlag;
    int retVal;

    remoteFlag = getServerHostByFileInx (fileCloseInp->fileInx, 
                                         &rodsServerHost);

    if (remoteFlag == LOCAL_HOST) {
         FileDesc[ fileCloseInp->fileInx ].fd, rodsServerHost->hostName->name );
        retVal = _rsFileClose (rsComm, fileCloseInp);
    } else if (remoteFlag == REMOTE_HOST) {
         FileDesc[ fileCloseInp->fileInx ].fd, rodsServerHost->hostName->name );
        retVal = remoteFileClose (rsComm, fileCloseInp, rodsServerHost);
    } else {
        if (remoteFlag < 0) {
            return (remoteFlag);
        } else {
            rodsLog (LOG_NOTICE,
                     "rsFileClose: resolveHost returned unrecognized value %d",
                     remoteFlag);
            return (SYS_UNRECOGNIZED_REMOTE_FLAG);
        }
    }

    /* Manually insert call-specific code here */

    freeFileDesc (fileCloseInp->fileInx);

    return (retVal);
}

int
remoteFileClose (rsComm_t *rsComm, fileCloseInp_t *fileCloseInp,
                 rodsServerHost_t *rodsServerHost)
{    
    int status;
    fileCloseInp_t remFileCloseInp;

    if (rodsServerHost == NULL) {
        rodsLog (LOG_NOTICE,
                 "remoteFileClose: Invalid rodsServerHost");
        return SYS_INVALID_SERVER_HOST;
    }

    if ((status = svrToSvrConnect (rsComm, rodsServerHost)) < 0) {
        return status;
    }

    memset (&remFileCloseInp, 0, sizeof (remFileCloseInp));
    remFileCloseInp.fileInx = convL3descInx (fileCloseInp->fileInx);
    status = rcFileClose (rodsServerHost->conn, &remFileCloseInp);

    if (status < 0) { 
        rodsLog (LOG_NOTICE,
                 "remoteFileClose: rcFileClose failed for %d, status = %d",
                 remFileCloseInp.fileInx, status);
    }

    return status;
}

int
_rsFileClose (rsComm_t *rsComm, fileCloseInp_t *fileCloseInp)
{

    if(FileDesc[fileCloseInp->fileInx].objPath == NULL ||
       FileDesc[fileCloseInp->fileInx].objPath[0] == '\0') {

        if(true) {
            eirods::stacktrace st;
            st.trace();
            st.dump();
        }

        std::stringstream msg;
        msg << __FUNCTION__;
        msg << " - Empty logical path.";
        eirods::log(LOG_ERROR, msg.str());
        return -1;
    }
    
    // =-=-=-=-=-=-=-
    // call the resource plugin close operation 
    eirods::file_object file_obj( rsComm,
                                  FileDesc[fileCloseInp->fileInx].objPath, 
                                  FileDesc[fileCloseInp->fileInx].fileName, 
                                  FileDesc[fileCloseInp->fileInx].rescHier, 
                                  FileDesc[fileCloseInp->fileInx].fd,
                                  0, 0 );

    eirods::error close_err = fileClose( rsComm, file_obj );
    // =-=-=-=-=-=-=-
    // log an error, if any
    if( !close_err.ok() ) {
        std::stringstream msg;
        msg << "_rsFileClose: fileClose failed for ";
        msg << fileCloseInp->fileInx;
        msg << ", status = ";
        msg << close_err.code();
        eirods::error err = PASS( false, close_err.code(), msg.str(), close_err ); 
    }

    return close_err.code();

} // _rsFileClose 








