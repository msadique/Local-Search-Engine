/*******************************************************************************
 *  FILE NAME    : server.c
 *
 *  DESCRIPTION  : contains the main routine and search,transfer of files
 *
 *  DATE            NAME        REFRENCE            REASON
 *  6-Dec-2010    Mohd Sadique    PRISM PROJECT       Initial Creation 
 *
 *  
 *
 ******************************************************************************/


/*******************************************************************************
 *          HEADER FILES
 ******************************************************************************/

#include <s_header.h>

/*******************************************************************************
 *              GLOBAL VARIABLES
 ******************************************************************************/

/*array used for storing the path of local storage area */
SCHAR g_path[PATH_SIZE];

/*store the socket id for server*/
static S32_INT server_sockfd;
pthread_mutex_t g_counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
 *              SIGNAL HANDLER FUNCTIONS
 ******************************************************************************/

/*******************************************************************************
 *  FUNCTION NAME   :   server_signal_handler_int
 *  DESCRIPTIOM     :   signal handler for SIGINT
 *  RETURNS         :   void
 *
 ******************************************************************************/

static void  server_signal_handler_int(
		S32_INT signum /*signal number*/
		)
{
	(void)close(server_sockfd);
	printf("\n RECEIVED CTRL-C SIGNAL WITH SIGNAL NUMBER %d\n",signum);
	exit(SUCCESS);

}/*End of function*/

/*******************************************************************************

 *  FUNCTION NAME   :   serve_thread
 *  DESCRIPTIOM     :   function called by the thread
 *  RETURNS         :   void
 *
 
*********************************************************************************/

static void *serve_thread(void *arg)
{
	S32_INT ret_fun,
			type=0;
	S32_INT connfd;
	
	connfd = *(S32_INT*)(arg);
	while(1 == 1)
	{
		ret_fun = server_read_request (connfd, &type);
		if (FAILURE == ret_fun)
		{
			/*getrequesttype failed */
			A_ERROR(MAJOR,ERROR_READ_QUERY);
			(S32_INT)close (connfd);
			break;
		}
		else 
		{
			if(1 == type)
			{     
				pthread_mutex_lock(&g_counter_mutex);
				/*request is for file transfer*/
		 	   ret_fun = server_send_file (connfd);
				if (FAILURE == ret_fun)
				{ 
					/*processfile failed*/
					A_ERROR(MAJOR,ERROR_PROCESSING);
					(S32_INT)close (connfd);
					pthread_mutex_unlock(&g_counter_mutex);
					break;
				}/*End of IF*/
				
				pthread_mutex_unlock(&g_counter_mutex);
				break;
			}/*End of IF*/

			else if(2 == type)
			{
				pthread_mutex_lock(&g_counter_mutex);
				/*request is for file transfer*/
				ret_fun = server_search_files (connfd);

				if (FAILURE == ret_fun)
				{ 
					/*searchfile failed */
					A_ERROR(MAJOR,ERROR_SEARCH);
					(S32_INT)close (connfd);
					pthread_mutex_unlock(&g_counter_mutex);
					break;
				}/*End of IF*/
				pthread_mutex_unlock(&g_counter_mutex);
			}/*End of ELSE*/
			else
			{ 
				printf("\n EXITING THREAD");
				(S32_INT)close(connfd);

				pthread_exit((void*)SUCCESS);
			}
		}/*End of IF-ELSE */
	}/*End of WHILE */
	A_TRACE(DETAILED_TRACE, "-----EXITING FROM SERVER THREAD-----\n");
	(S32_INT)close(connfd);

	pthread_exit((void*)SUCCESS); /*Exit CHILD */


}//end of thread


/*******************************************************************************
 *  FUNCTION NAME   :   main
 *  DESCRIPTIOM     :   main routine for Local Search 
 *                  Engine server
 *  RETURNS     :   exit code   
 *
 ******************************************************************************/
 
int main ()
{
	/*Local Declarations */
	struct sockaddr_in  serv_addr,    /*server address structure*/
						client_addr;  /*client address structure*/
	S32_INT connfd = 0,     /*connected socket descriptor*/
			close_ret = 0;  /*return value from close*/
	return_value ret_fun = 0;    /*return value from function*/
	SCHAR 	ip[IP_LENGTH+1],/*array for storing the ip address*/
			port[PORT_LENGTH+1],/*array for storing the port for server*/
			*p_fgets_ret = NULL,/*return value from fgets*/
			buff[100];	/*stores the ip address*/
	socklen_t addr_len = 0;       /*socket address length*/
	pthread_t ntid = 0;
	FILE      *p_file_conf = NULL;/*file pointer pointer pointing to .conf files*/
	/*Signal Handlers registered for SIGCHILD and SIGINT*/
	(void)signal(SIGINT, server_signal_handler_int);

	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER MAIN-----\n");
	memset (ip, (S32_INT)'\0', IP_LENGTH+1);
	memset (port, (S32_INT)'\0', PORT_LENGTH+1);
	memset (&serv_addr, 0, sizeof (serv_addr));
	memset (&client_addr, 0, sizeof (client_addr));

	/*Open configuration file */
	p_file_conf = fopen ("../src/server.conf", "r");

	/*fopen failed */
	if (NULL == p_file_conf)
	{
		A_ERROR(CRITICAL,ERROR_OPEN_FILE);
		return FAILURE;
	}

	/*Get Server IP ADDRESS */
	p_fgets_ret = fgets (ip, IP_LENGTH+1, p_file_conf);
	if (NULL == p_fgets_ret)
	{
		A_ERROR(CRITICAL,ERROR_GET_IP);
		return FAILURE;
	}

	/*Remove newline at the end of buffer*/
	ip[strlen(ip)-1] = '\0';

	/*getting port from server.conf file */
	p_fgets_ret = fgets(port, 7, p_file_conf); 
	if(NULL == p_fgets_ret)
	{
		A_ERROR(CRITICAL,ERROR_GET_PORT);
		return FAILURE;
	}

	/*getting path sotred in server.conf*/
	p_fgets_ret = fgets(g_path, PATH_SIZE, p_file_conf); 
	if(NULL == p_fgets_ret)
	{
		A_ERROR(MAJOR,ERROR_PATH);
		return FAILURE;
	}

	g_path[strlen(g_path)-1]='\0';  /*remove new line from g_path*/
	close_ret = fclose(p_file_conf);/*closinf file pointer*/
	/*checking error for closing file pointer*/
	if(0!=close_ret)
	{
		A_ERROR(MINOR,ERROR_CLOSE_FILE);
		return FAILURE;
	}

	if(FAILURE == server_validate_ip(ip))
	{
		A_TRACE(BRIEF_TRACE,"IP should be of form 1.1.1.1\n");
		A_ERROR(MAJOR,ERROR_IP);
		return FAILURE;
	}

	if(FAILURE == server_validate_port(atoi(port)))
	{
		A_TRACE(BRIEF_TRACE,"Port should be between 1024 to 65535\n");
		A_ERROR(MAJOR,ERROR_PORT);
		return FAILURE;
	}

	server_sockfd = socket (AF_INET, SOCK_STREAM, 0);/*create socket */
	/*socket creation failed */
	if (FAILURE == server_sockfd)
	{
		A_ERROR(CRITICAL,ERROR_CREATION_SOCKET);
		return FAILURE;
	}

	A_TRACE(BRIEF_TRACE,"---> SOCKET CREATED\n");

	/*populating server socket structure*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons ((uint16_t)atoi (port));
	serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	addr_len = (socklen_t)sizeof (serv_addr);

	/*Binding address and checking for FAILURE*/
	ret_fun = bind (server_sockfd, (struct sockaddr *) &serv_addr, addr_len);
	
	if (FAILURE == ret_fun)
	{
		A_ERROR(CRITICAL,ERROR_BIND);/*displaying error corresponding message*/ 
		(S32_INT)close (server_sockfd);
		return FAILURE;
	}

	A_TRACE(BRIEF_TRACE,"---> SOCKET BINDED TO A PORT\n");

	/*making socket into listen mode*/


	ret_fun = listen (server_sockfd, 25);
	if (FAILURE == ret_fun)
	{
		A_ERROR(CRITICAL,ERROR_LISTEN);
		(S32_INT)close (server_sockfd);
		return FAILURE;
	}

	A_TRACE(BRIEF_TRACE,"---> SOCKET LISTENING\n");
	/*getting size of client structure*/

	addr_len = (socklen_t)sizeof (client_addr);
	/*infinite loop for accepting clients*/

	while (1 == 1)
	{
		A_TRACE(DETAILED_TRACE,
				"\n---> INSIDE LOOP FOR RECEIVING CONNECTION FROM DIFFERENT CLIENTS\n\n");
		memset(&client_addr,0,(size_t)addr_len);
		connfd = accept(server_sockfd,
				(struct sockaddr *) &client_addr,
				&addr_len);

		/*Accept failed */

		if (FAILURE == connfd)
		{   
			A_ERROR(MAJOR,ERROR_ACCEPT);
			continue;
		}
		memset(buff,0,100);
		printf("Received connection from %s\n", inet_ntop(AF_INET, &client_addr.sin_addr,(SCHAR*) buff,(socklen_t) sizeof(buff)));
		
		(void) pthread_create(&ntid,NULL,serve_thread,(void *)&connfd);

	}/*End of while */

		(int) pthread_join(ntid,(void *)NULL);
	return 0;

}/*End of function*/

