/*******************************************************************************
 *  FILE NAME    :  client.c
 *
 *  DESCRIPTION  : contains main routine for client
 *
 *  DATE            NAME             REFERENCE         REASON
 *  20-Dec-2010    Mohd Sadique    PRISM PROJECT      Initial Creation
 *

/*******************************************************************************
 *      HEADER FILES
******************************************************************************/

#include <c_header.h>

static S32_INT g_connfd = 0;			/*store connected socket descriptor */


 /***************************************************************
 *
 *  
 *
 ******************************************************************************/

 *
 *  FUNCTION NAME   : signal_handler  
 *
 *  DESCRIPTION     : signal handler for CTRL-C and send quit message to server
 *
 *  RETURN VALUES   : void
 *
 ***************************************************************/

static void signal_handler (
					S32_INT signum	/*signal number */
  				)
{
	 (void)client_send ("quit", g_connfd);
  	 printf ("\n recieved ctrl-c with signal no %d\n", signum);
  	 exit (SUCCESS);
}				/*End of function */


/*******************************************************************************
 * FUNCTION NAME: main
 *
 * DESCRIPTION: This is the main function for local file search
 *             and displays the contents of the respective file.
 *
 * RETURNS: exit status
 *
 ******************************************************************************/

S32_INT main ()
{
  /*Local Declarations */
  S32_INT 		option = 0;		/*store menu option */
  return_value  ret_val = 0;	/*store return value from function */
  SCHAR         *p_file_name;
  A_TRACE (DETAILED_TRACE, "-----ENTERING CLIENT-----\n");


  (void) signal (SIGINT, signal_handler);
  while(1 == 1)
  {
   /*display menu returns whether to find files or quit */
   option = client_display_menu ();

  /*Search file */
  if (1 == option)
    {

    	 /*Establish connection to server */
      	 g_connfd = client_connect_to_server ();
      	/*connection to server couldnt be established */
      	if (FAILURE == g_connfd)
		{
	  		A_ERROR (CRITICAL, ERROR_CONNECTION);
	  		return FAILURE;
		}
      	/*search_files displays search options and carries on task */
      	ret_val = client_search_file_req (g_connfd);
		
      	/*search_files failed */
      	if (FAILURE == ret_val)
		{
	  		A_ERROR (MAJOR, ERROR_SEARCH);
	 		return FAILURE;
		}

      	ret_val = client_close_connection (g_connfd);

      	/*close_connection failed */
      	if (FAILURE == ret_val)
		{
	  		A_ERROR (MAJOR, ERROR_CLOSE_CONNECTION);
	  		return FAILURE;
		}
    }				/*End of IF */
	
	else if (2 == option)
    {

      	/*Establish connection to server */
      	g_connfd = client_connect_to_server ();
      	/*connection to server couldnt be established */
      	if (FAILURE == g_connfd)
		{
	  		A_ERROR (CRITICAL, ERROR_CONNECTION);
	  		return FAILURE;
		}
      	p_file_name = (SCHAR *) malloc (sizeof (SCHAR) * 32);
		if(NULL == p_file_name)
		{
			A_ERROR(MAJOR,ERROR_MEMORY);
			return(FAILURE);
		}
		memset(p_file_name,0,32);
      		ret_val = client_get_file_content (p_file_name, g_connfd);
      		if (FAILURE == ret_val)
			{
	  			A_ERROR (MAJOR, ERROR_GET_FILENAME);
				free(p_file_name);
	  		    return FAILURE;
		    }
      	free (p_file_name);
      	ret_val = client_close_connection (g_connfd);

      	/*close_connection failed */
      	if (FAILURE == ret_val)
		{
	 		 A_ERROR (MAJOR, ERROR_CLOSE_CONNECTION);
	  		return FAILURE;
		}
    }
	else
		{
		    A_TRACE(DETAILED_TRACE, "-----EXITING CLIENT-----\n");
		    return SUCCESS;
		}
   }//END OF WHILE
   return (SUCCESS);

}/*End of function */
