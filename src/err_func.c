/*******************************************************************************
 *  FILE NAME    : err_func.c
 *
 *  DESCRIPTION  : contains function and global variables for error handling
 *
 *  DATE            NAME           REFERENCE          REASON
 *  6-Dec        Mohd Sadique      PRISM PROJECT      Initial Creation
 *
 *
 * 
 *
 ******************************************************************************/

/*******************************************************************************
 *                           HEADER FILES
 ******************************************************************************/

#include <c_header.h>

/*******************************************************************************
 *                         GLOBAL VARIABLES
 ******************************************************************************/

S32_INT g_trace_level = DETAILED_TRACE; /*setting trace variable to DETAILED*/
S32_INT g_error_level = ERROR_ALL;        /*setting error level to ERR_ALL*/


/*array of pointers for storing error messages*/

static SCHAR *err_msg[]={
        "CONNECTION NOT ESTABLISHED",
        "CANNOT SEARCH",
        "ERROR IN CLOSING CONNECTION",
        "ERROR IN OPENING FILE",
        "ERROR IN READING IP ADDRESS FROM FILE",
        "ERROR IN READING PORT NUMBER FROM FILE",
        "ERROR IN CREATING SOCKET",
        "FAILED TO CONNECT TO SERVER",
        "ERROR IN CLOSING SOCKET",
        "MEMORY CANNOT BE ALLOCATED",
        "NOT A VALID SEARCH",
        "BUILD QUERY FAILED",
        "SEND RECEIVE FAILED",
        "CANNOT GET THE FILENAME",
        "CANNOT BIND THE IP AND PORT",
        "SERVER CANNOT GO INTO LISTEN MODE",
        "SERVER CANNOT ACCEPT CLIENT",
        "CANNOT CREATE A NEW PROCESS",
        "ERROR IN READING QUERY TYPE",
        "ERROR IN PROCESSING FILE",
        "CANNOT CLOSE THE FILE",
        "NOT A VALID CHOICE FOR TYPE",
        "CANNOT CREATE PIPE",
        "CANNOT READ FROM SOCKET",
        "CANNOT READ PATH FROM FILE",
        "CANNOT WRITE INTO SOCKET",
        "CANNOT READ FROM FILE",
        "IP ADDRESS IS NOT PROPER",
        "PORT NUMBER NOT IN RANGE",
		"ERROR IN MEMORY REALLOCATION"
};


/*******************************************************************************
 *
 * FUNCTION NAME: error_fun
 *
 * DESCRIPTION: function for error handling
 *
 * RETURNS: void
 *
 ******************************************************************************/

void error_fun(
                 S32_INT level, /*error level*/
                 S32_INT errn   /*error number*/
                )
{

    A_TRACE(BRIEF_TRACE,"-----ENTERING ERROR HANDLER FUNCTION-----\n");

    /*checking the level of error detected and
      printing the appropriate message*/
    if( (ERROR_ALL == g_error_level) ||
        (g_error_level == level)
      )
    {
        printf("ERROR NUMBER IS: %d AND ERROR IS %s\n",errn,err_msg[errn]); 
    }

    A_TRACE(BRIEF_TRACE,"-----EXITING ERROR HANDLER SUCCESSFULLY-----\n");

} /*End of function*/
