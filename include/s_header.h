/*************************************************************************
 *  FILE NAME    : s_header.h
 *
 *  DESCRIPTION  : Header file for Server side
 *
 *  DATE            NAME              REFERENCE         REASON
 *  6-Dec-2010    Mohd Sadique    PRISM PROJECT      Initial Creation
 *
 *
 *  
 **************************************************************************/


/*************************************************************************
 *                            HEADER FILES
 **************************************************************************/    

#ifndef _HEADER
#define _HEADER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
/*************************************************************************
 *                       USER DEFINED DATA TYPES
 **************************************************************************/    

typedef enum  { 
		SUCCESS=0,
                FAILURE=-1
	      } return_value;


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
 *                              MACROS
 **************************************************************************/    

#define SIZE            1024
#define QUERY_SIZE      4
#define LINE_SIZE       100
#define FILE_NAME_SIZE  33
#define PATH_SIZE       15
#define CHILD_ID        0
#define IP_LENGTH       15
#define PORT_LENGTH     5
#define FIND_QUERY_SIZE 150

#define MINPORT         1023
#define MAXPORT         65536

#define NO_TRACE        0
#define BRIEF_TRACE     1
#define DETAILED_TRACE  2

#ifdef TRACE
#define A_TRACE(level,x) if(g_trace_level >= level) printf(x)
#else
#define A_TRACE(level,x)
#endif

#define ERR_ALL                 4
#define CRITICAL                3
#define MAJOR                   2
#define MINOR                   1

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
#define ERROR_PATH                24
#define ERROR_WRITE_SOCKET        25
#define ERROR_READ_FILE           26
#define ERROR_IP                  27
#define ERROR_PORT                28

#define A_ERROR(level,x) error_fun(level,x)


/*************************************************************************
 *                         GLOBAL VARIABLES
 **************************************************************************/

extern char g_path[PATH_SIZE]; 
extern int  g_trace_level;
extern int  g_error_level;

/*************************************************************************
 *                        FUNCTION PROTOTYPES
 **************************************************************************/

extern return_value server_read_request (S32_INT, S32_INT *);
extern return_value server_search_files (S32_INT connfd);
extern return_value server_send_files (S32_INT connfd);
extern return_value server_validate_ip(SCHAR *);
extern return_value server_validate_port(S32_INT);
extern void error_fun(S32_INT,S32_INT);
extern return_value server_send_file( S32_INT);

#endif
