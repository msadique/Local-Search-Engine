/*************************************************************************
 *  FILE NAME    : c_header.h 
 *
 *  DESCRIPTION  : Header file for Client side
 *
 *  DATE            NAME                REFERENCE         REASON
 *  6-Dec-2010    Mohd Sadique   PRISM PROJECT      Initial Creation
 *
 *
 * 

 **************************************************************************/


/*************************************************************************
 *                           HEADER FILES
 **************************************************************************/    

#ifndef _HEADER
#define _HEADER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

/*************************************************************************
 *                               MACROS
 **************************************************************************/

#define RD_WR_SIZE          100
#define QUERY_TYPE_SIZE     4
#define FORMAT_QUERY_SIZE   100
#define FILENAME_SIZE       34
#define IP_LENGTH           15
#define PORT_LENGTH         5

#define MINPORT             1023
#define MAXPORT             65536

#define NO_TRACE            0
#define BRIEF_TRACE         1
#define DETAILED_TRACE      2

#ifdef TRACE
#define A_TRACE(level,x) if(g_trace_level >= level) printf(x)
#else
#define A_TRACE(level,x)
#endif

#define ERROR_ALL           4
#define CRITICAL            3
#define MAJOR               2
#define MINOR               1

#define ERROR_CONNECTION          0
#define ERROR_SEARCH              1
#define ERROR_CLOSE_CONNECTION    2
#define ERROR_OPEN_FILE           3
#define ERROR_GET_IP              4
#define ERROR_GET_PORT            5
#define ERROR_CREATION_SOCKET     6
#define ERROR_FAILURE_CONNECTION  7
#define ERROR_CLOSE_SOCKET        8
#define ERROR_MEMORY              9
#define ERROR_INVALID_SEARCH      10
#define ERROR_BUILD_QUERY         11
#define ERROR_SEND_RECEIVE        12
#define ERROR_GET_FILENAME        13
#define ERROR_BIND                14
#define ERROR_LISTEN              15
#define ERROR_ACCEPT              16
#define ERROR_FORK                17
#define ERROR_READ_QUERY          18
#define ERROR_PROCESSING          19
#define ERROR_CLOSE_FILE          20
#define ERROR_INVALID_TYPE        21
#define ERROR_PIPE                22
#define ERROR_READ_SOCKET         23
#define ERROR_WRITE_SOCKET        25
#define ERROR_IP                  27
#define ERROR_PORT                28
#define ERROR_REALLOCATION		  29

#define A_ERROR(level,x) error_fun(level,x)

typedef unsigned char UCHAR ;
typedef char SCHAR;

#ifdef OS_LINUX64

typedef unsigned short U32_INT;
typedef short S32_INT; 

#else

typedef unsigned int U32_INT;
typedef int S32_INT; 

#endif

/*************************************************************************
 *                       USER DEFINED DATA TYPES
 **************************************************************************/    

typedef enum {
		SUCCESS=0,
                FAILURE=-1
	     } return_value;


/*************************************************************************
 *                          GLOBAL VARIABLES
 **************************************************************************/    

extern int g_trace_level;
extern int g_error_level;


/*************************************************************************
 *                         FUNCTION PROTYPES
 **************************************************************************/    

extern int client_connect_to_server();
extern int client_show_menu();
extern return_value client_close_connection(S32_INT);
extern return_value client_search_files(S32_INT);
extern void error_fun(S32_INT, S32_INT);
extern return_value client_send( SCHAR *req,S32_INT connfd);
extern return_value client_get_file_content(SCHAR *p_filename,S32_INT connfd);
extern return_value client_search_file_req( S32_INT);
extern S32_INT client_display_menu();
#endif
