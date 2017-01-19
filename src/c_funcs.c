/******************************************************************************
 *  FILE NAME    : c_funcs.c
 *
 *  DESCRIPTION  : contains the functions for the client side
 *
 *  DATE            NAME        REFERENCE          REASON
 *  6-Dec-2010    Mohd Sadique   PRISM PROJECT      Initial Creation
 *  
 *
 *  
 *
 ******************************************************************************/


/******************************************************************************
 *      HEADER FILES
 ******************************************************************************/

#include <c_header.h>

/******************************************************************************
 *
 * FUNCTION NAME: client_validate_port
 *
 * DESCRIPTION: check if the port number is valid or not
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

static return_value client_validate_port(
                                   S32_INT port /*port number*/
                                         )
{
    /*check if port number is within range*/
    if(port>MINPORT && port < MAXPORT)
        return SUCCESS;
    else    
        return FAILURE;
	/*end of if*/

}/*End of Functions*/


/******************************************************************************
 *
 * FUNCTION NAME: client_validate_ip
 *
 * DESCRIPTION: check if the ip address is valid
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

static return_value client_validate_ip(
                        SCHAR *p_ip_address /*IP address*/
                                )
{
    S32_INT dot_count = 0,  /*number of dots count*/
 	        int_count = 0,  /*number of numeral count*/
     	    temp_value =0;  /*store value of each dot seperated number*/

    /*check if first character of IP is valid*/
    if( '0'<= *p_ip_address &&  '9'>= *p_ip_address)
    {
        temp_value = (int)(*p_ip_address) - 48;
        p_ip_address++;
    }
    else
        return FAILURE;
    /*end of if*/
	
    /*count number of characters and numerals and check numeral value*/
    while( '\0' != *p_ip_address)
    {
        /*dot found*/
        if( '.' == *p_ip_address)
        {
            /*increament dot count*/
            dot_count++;
            /*check if dot seperated number value is valid*/
            if( 0 <= temp_value && 255 >= temp_value)
                temp_value = 0;
            else
                return FAILURE;
        }
        /*numeral found*/
        else if( '0'<= *p_ip_address && '9'>= *p_ip_address)
        {
            temp_value = temp_value * 10 + (int)(*p_ip_address) - 48;
            int_count++;
        }
        /*unrecognized character*/
        else
        {
            A_TRACE(DETAILED_TRACE,
                    "---> UNRECOGNISED CHARACTER IN THE IP ADDRESS\n");
            return FAILURE;
        }

        /*consecutive dots found*/
        if( '.' == *(p_ip_address) && '.' == *(p_ip_address+1))
            return FAILURE;
        p_ip_address++;
    }/*End of WHILE*/

    /*check dot count and numeral count*/
    if(( 4<=int_count && 12>=int_count) && 3==dot_count)
        return SUCCESS;
    else
        return FAILURE;

}/*End of function*/

 
 /***************************************************************
  *
  *  FUNCTION NAME   : client_send
  *
  *  DESCRIPTION     : send quit to server thread to terminate
  *
  *  RETURN VALUES   : SUCCESS OR FAILURE
  *
  **************************************************************/

return_value client_send( SCHAR *p_req , S32_INT connfd)
{

    S32_INT     pointer = 0,write_count = 0, 
            len = (S32_INT)strlen(p_req);

    A_TRACE(DETAILED_TRACE,"-----ENTERING SEND_RECEIVE FUNCTION-----\n");

    A_TRACE(BRIEF_TRACE,"-----ENTERING LOOP FOR WRITING ON SOCKET-----\n");

    
    while( pointer < len)
    {
        write_count = (S32_INT)write(connfd,
                                 (p_req + pointer),
                                 (size_t)(len-pointer)
                                );

    
        if(FAILURE == write_count)
        {
            A_ERROR(MAJOR,ERROR_WRITE_SOCKET);
            return FAILURE;
        }
        pointer  = pointer + write_count;
    }
	/*End of WHILE*/
	
    A_TRACE(BRIEF_TRACE,"-----EXITING LOOP FOR WRITING-----\n");  
	return SUCCESS;
}


/******************************************************************************
 *
 * FUNCTION NAME: client_send_receive
 *
 * DESCRIPTION: send the query to server and receive the contents
 *             of the query requested and display it.
 *
 * RETURNS: SUCCESS or FAILURE
 *
 ******************************************************************************/

static return_value client_send_receive(
                SCHAR *p_temp_request,/*stores string to be sent to server*/ 
                S32_INT connfd /*stores connection socket connection descriptor */
                                        )
{
    /*local declarations*/
    S32_INT     read_count = 0,     /*receives return value of read();*/
 	            pointer = 0,        /*keeps track of temp_request read pointer*/    
                write_count = 0,    /*receives return value of write();*/
                len = (S32_INT)strlen(p_temp_request), /*length of p_temp_request*/
                counter = 0;        /*mentain number of read counts*/
    SCHAR       buff[RD_WR_SIZE],   /*buffer to store read characters*/
                *p_buff=buff;       /*pointer to buffer*/

    A_TRACE(DETAILED_TRACE,"-----ENTERING CLIENT_SEND_RECEIVE FUNCTION-----\n");

    A_TRACE(BRIEF_TRACE,"-----ENTERING LOOP FOR WRITING ON SOCKET-----\n");

    /*write on socket*/ 
    while( pointer < len)
    {
        write_count = (S32_INT)write(connfd,
                                 (p_temp_request + pointer),
                                 (size_t)(len-pointer)
                                );

        /*write failed */
        if(FAILURE == write_count)
        {
            A_ERROR(MAJOR,ERROR_WRITE_SOCKET);
            return FAILURE;
        }
        pointer  = pointer + write_count;
    }/*End of WHILE*/
    
	A_TRACE(BRIEF_TRACE,"-----EXITING LOOP FOR WRITING-----\n");  

    /*wait for response from server*/
    memset(buff, (S32_INT)'\0', sizeof(buff));  

    A_TRACE(BRIEF_TRACE,"-----ENTERING LOOP FOR READING FROM SOCKET-----\n");

    /*read from socket*/
    while((read_count = (S32_INT)read(connfd,buff, (size_t)(RD_WR_SIZE-1))) > 0)
    {
        counter++;
        if( NULL !=(p_buff= strstr(buff, "error!@#$")) )
        {
            counter=-1000;
            *p_buff='\0';
        }

        A_TRACE(BRIEF_TRACE,"\n\t\t\t\t\t\t---> <end of string>\n");   
        /*termination signal recieved*/
        if( NULL != (p_buff=strstr(buff,"quit!@#$") ) )
        {
            *p_buff='\0';
            printf("%s",buff);
            break;
        }

        printf("%s",buff);
        memset(buff,(S32_INT)'\0', RD_WR_SIZE);             
    }/*End of WHILE*/

    if(counter<0)
    {
        A_TRACE(BRIEF_TRACE,
                "\n-----EXITING CLIENT_SEND_RECEIVE FUNCTION WITH STATUS NO FILE FOUND-----\n");
        return FAILURE; 
    }

    /*ERROR in reading from socket*/
    if(0 > read_count)
    {
        A_ERROR(MAJOR,ERROR_READ_SOCKET);
        return FAILURE;
    }
    A_TRACE(DETAILED_TRACE,
            "\n-----EXITING CLIENT_SEND_RECEIVE FUNCTION SUCCESSFULLY-----\n");
    return SUCCESS;

}/*End of Function*/


/******************************************************************************
 *
 * FUNCTION NAME: client_get_file_content
 *
 * DESCRIPTION: get a filename from user.
 *
 * RETURNS: SUCCESS or FAILURE
 *
 ******************************************************************************/ 

return_value client_get_file_content(
                            SCHAR *p_filename,/*stores filename from user*/ 
                            S32_INT connfd/*stores connection socket descriptor*/
                                        )
{
    /*local declarations*/
    /*store return value from functions*/
    S32_INT     ret_val=1;
    /*temporary buffer to store string*/
    SCHAR       temp_buff[QUERY_TYPE_SIZE+FILENAME_SIZE];
    /*read filename from user*/
    
	printf("\nEnter the path of the file whose contents are required ...\n(starting with ../LSE/ )  :-  ");
	while((int)'\n'!=getchar());
	do
	{
    	(void)fgets(p_filename,FILENAME_SIZE,stdin);
	}while(strlen(p_filename)<4);
    
	p_filename[strlen(p_filename)-1] = '\0';
     printf("FILE :- %s\n\n",p_filename);
    /*initialize memory*/
    memset(temp_buff,(S32_INT)'\0',QUERY_TYPE_SIZE+FILENAME_SIZE);

    A_TRACE(DETAILED_TRACE,"-----ENTERING CLIENT_GET_FILE_CONTENT FUNCTION-----\n");

    strcpy(temp_buff,"file");

    /*if newline is present at the end of p_filename */
    if (NULL != strchr(p_filename,'\n') )
    {
        p_filename[strlen(p_filename)-1] = '\0';
    }
	
	if(NULL!=p_filename)
    	strcat(temp_buff,p_filename);
    if(ret_val > 0)
    {   /*client_send_receive sends "file"(request to transfer
          contents), filename whose contents are to viewed and
          receives contents from server */
        ret_val = client_send_receive(temp_buff,connfd);
        p_filename = NULL;
        if(FAILURE == ret_val)
        {       /*send_receive failed */
            A_ERROR(CRITICAL,ERROR_SEND_RECEIVE);
            return FAILURE;
        }
        else
        {
            A_TRACE(DETAILED_TRACE,
                    "-----EXITING CLIENT_GET_FILE_CONTENT FUNCTION SUCCESSFULLY-----\n");
            return SUCCESS;
        }
    }/*End of OUTER IF*/
    else
    {
        p_filename = NULL;
        return FAILURE;
    }/*End of ELSE*/

}/*End of Function*/


/******************************************************************************
 *
 * FUNCTION NAME: client_connect_to_server
 *
 * DESCRIPTION: Establish connection to the server
 *
 * RETURNS: socket descriptor
 *
 ******************************************************************************/

S32_INT client_connect_to_server()
{
    /*local declarations*/
    S32_INT     	    sockfd=0;               /*connected socket descriptor*/
	
    struct sockaddr_in 	serv_addr;  /*server address structure*/
    SCHAR    			ip[IP_LENGTH+1],        /*server ip address*/
            			port[PORT_LENGTH+1];    /*server port number*/
    FILE    			*p_file_conf=NULL;      /*pointer to configuration file */
    socklen_t 			addr_len=0;           /*address length*/

    A_TRACE(DETAILED_TRACE,"-----ENTERING CLIENT_CONNECT_TO_SERVER FUNCTION-----\n");

    /*initialize variables*/
    memset(ip, (S32_INT)'\0', IP_LENGTH+1);
    memset(port, (S32_INT)'\0', PORT_LENGTH+1);
    memset(&serv_addr, 0, sizeof(serv_addr));

    /*Open configuration file*/
    A_TRACE(BRIEF_TRACE,"---> OPENING FILE client.conf\n");
    p_file_conf = fopen("../src/client.conf","r");

    /*fopen failed*/
    if(NULL == p_file_conf)
    {
        A_ERROR(CRITICAL,ERROR_OPEN_FILE);
        return FAILURE;
    }

    /*Get Server IP ADDRESS*/
    if(NULL == fgets(ip, IP_LENGTH+1, p_file_conf) )
    /*fgets failed*/
    {
        (void)fclose(p_file_conf);
        A_ERROR(MAJOR,ERROR_GET_IP);
        return FAILURE;
    }

    /*Remove newline at the end of buffer*/
    ip[strlen(ip)-1] = '\0';

    /*Get Server Port Number*/
    if(NULL == fgets(port, PORT_LENGTH+1, p_file_conf) )
    /*fgets failed*/
    {
        (void)fclose(p_file_conf);
        A_ERROR(MAJOR,ERROR_GET_PORT);
        return FAILURE;
    }

    /*check if ip address is valid*/
    if(FAILURE == client_validate_ip(ip))
    /*invalid ip address*/
    {
        A_TRACE(BRIEF_TRACE,"---> IP should be of form 1.1.1.1\n");
        A_ERROR(MAJOR,ERROR_IP);
        return FAILURE;
    }

    /*check if port number is valid*/
    if(FAILURE == client_validate_port(atoi(port)))
    /*invalid port number*/
    {
        A_TRACE(BRIEF_TRACE,
                "---> Port should be between 1024 to 65535\n");
        A_ERROR(MAJOR,ERROR_PORT);
        return FAILURE;
    }

    /*create socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*socket creation failed*/
    if(FAILURE == sockfd)
    {
        (void)fclose(p_file_conf);
        A_ERROR(CRITICAL,ERROR_CREATION_SOCKET);
        return FAILURE;
    }

    /*initialize server address*/
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( (uint16_t)atoi(port));
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    addr_len = (socklen_t)sizeof(serv_addr);

    /*connect to server*/
    if(FAILURE == connect( sockfd,
                           (struct sockaddr *)&serv_addr,
                           addr_len) )
    /*connect failed*/
    {
        (void)fclose(p_file_conf);
        A_ERROR(CRITICAL,ERROR_FAILURE_CONNECTION);
        (void)close(sockfd);
        return FAILURE;
    }

    (void)fclose(p_file_conf);

    /*connection established*/
    return sockfd;

}/*End of Functions*/


/******************************************************************************
 *
 * FUNCTION NAME: client_display_menu
 *
 * DESCRIPTION: display menu option for find and transfer of contents.
 *
 * RETURNS:     Option value selected by user
 *
 ******************************************************************************/

S32_INT client_display_menu ()
{
    /*local declarations*/
    S32_INT option=0;   /*store menu option*/

    A_TRACE(DETAILED_TRACE,"-----ENTERING CLIENT_DISPLAY_MENU FUNCTION-----\n");

    while(1 == 1)
    {
        /*Print menu*/
        A_TRACE(BRIEF_TRACE,"---> SHOWING THE MENU\n");
        printf("\n\n\t################  MENU  ################\n");
        printf("\n\t\t1 - Search File Name\n");
		printf("\t\t2 - Search File Content\n");
        printf("\t\t3 - Quit\n");
        printf("\n\t########################################\n");

        /*Get option*/
        printf("\n\t\tEnter Option : ");
        (void)scanf("%d", &option);
		printf("\n\n");

        /*valid choice entered*/
        if(1==option || 2==option || 3==option)
        {
            A_TRACE(BRIEF_TRACE,
                    "-----EXITING DISPLAY_MENU FUNCTION SUCCESSFULLY-----\n");
            break;
        }
        /*invalid choice entered*/ 
        else
        {
            A_TRACE(BRIEF_TRACE,"---> INVALID CHOICE\n");
            return FAILURE;
        }

    }/*end of WHILE*/
    return option;

}/*End of Functions*/


/******************************************************************************
 *
 * FUNCTION NAME: client_close_connection
 *
 * DESCRIPTION: closes the socket connection
 *
 * RETURNS: SUCCESS or FAILURE
 *
 ******************************************************************************/

return_value client_close_connection(
                            S32_INT sockfd /*recives socket descriptor*/
                                     )
{
    /*local declarations*/
    S32_INT ret_val=0; /*store return value from functions*/

    A_TRACE(BRIEF_TRACE,"-----ENTERING CLIENT_CLOSE_CONNECTION FUNCTION-----\n");

    ret_val = close(sockfd);

    /*close failed*/
    if(FAILURE == ret_val)
    {
        A_ERROR(CRITICAL,ERROR_CLOSE_SOCKET);
        return FAILURE;
    }
    /*close succeed*/
    else
    {
        A_TRACE(BRIEF_TRACE,"---> CONNECTION SUCCESSFULLY CLOSED\n");    
        return SUCCESS;
    }

}/*End of Functions*/

/******************************************************************************
 *
 * FUNCTION NAME: client_search_req
 *
 * DESCRIPTION: creates search query
 *
 * RETURNS: search query
 *
 ******************************************************************************/

static  SCHAR * client_search_req(
				
			      )
{
	S32_INT choice,
			not_count=0, // stores query choice of users
			len,  // stores not statement len
			length = 0,  //stores length of query
			i; //for loop counter
	SCHAR 	*p_temp = "find ", //stores search query 
			*p_temp1,  // stores not query in serach query
		    dis_req[FORMAT_QUERY_SIZE], // stores to query in presentable display 
			*p_file_req=NULL, // stores search query to return 
			filename[FILENAME_SIZE]; // stores file name to search

	A_TRACE(DETAILED_TRACE, "-----ENTERING CLIENT_SEARCH_REQ FUNCTION-----\n");

	length =(S32_INT) strlen(p_temp);	//find the length of temp
/* checking malloc*/
	p_file_req = (SCHAR*)malloc(sizeof(SCHAR)* (length+1));
	if(NULL==p_file_req)
	{
		A_ERROR(MAJOR, ERROR_MEMORY);
		exit(FAILURE);/* exiting*/
	} 

	strcpy(p_file_req, p_temp);
     printf("\n ----file1.txt to file500.txt----\n");
	do
	{
		printf("\nEnter the file name :- ");
		(void)scanf("%s",filename);     
		if(strlen(filename) >FILENAME_SIZE)/* checking size of filename*/
		printf("\nEnter filename again :- ");
	}while(strlen(filename)>FILENAME_SIZE);/* if true then re enter file name*/

	length+= strlen(filename)+strlen("-name \"\" ");  //find length of filename and -name ""
	
	strcpy(dis_req,filename);

/*checking if realloc faileid*/
	p_file_req = (SCHAR*)realloc(p_file_req,sizeof(SCHAR) * (length+10));
	if(NULL==p_file_req)
	{
		A_ERROR(MAJOR, ERROR_REALLOCATION);
		exit(FAILURE);/* exiting*/
	}
	else
	{
		strcat(p_file_req, "-name \"");
		strcat(p_file_req, filename);
		strcat(p_file_req, "\" ");
    }
	
	do
	{
	(void)system("clear");
	/*clients options*/
	printf("\n\tThe current query = %s \n", dis_req);
	printf("\n\tPress 1 for AND operation.");
	printf("\n\tPress 2 for OR operation.");
	printf("\n\tPress 3 for NOT operation.");
	printf("\n\tPress 4 to send search request : ");
	printf("\n\n\tEnter Your Choice :- ");

	(void)scanf("%d",&choice);/* your choice saved here*/           
 	printf("\n");

	switch(choice)
	{
	case 1 : 
	     printf("\nEnter another filename :- ");
		(void) scanf("%s",filename);
		 length+= strlen(filename) + strlen("-a -name \"\" ");
		 
		/* checking if realloc failed? */	
		 if(NULL == (p_file_req = (SCHAR*)realloc(p_file_req,sizeof(SCHAR) * length)))
	{
		    A_ERROR(MAJOR, ERROR_REALLOCATION);
			exit(FAILURE);/*exiting*/
		 }
		 else
		
		/*end of IF*/
		 strcat(p_file_req, "-a -name \"");
		 strcat(p_file_req, filename);
		 strcat(p_file_req, "\" ");
         if( strlen(p_file_req)  > FORMAT_QUERY_SIZE )/*checking  total request size of client after and operation */
           {
            A_TRACE(DETAILED_TRACE,"ERROR: -EXCEEDED THE QUERY SIZE\n");

            free(p_file_req);/* free previous alloted*/
            
            p_file_req=client_search_req();/* re allot*/
             
            choice=4;/* making choice 4 to exit after recursive operation*/
            break;
           }
    
		 strcat(dis_req," AND ");
		 strcat(dis_req,filename);
         not_count=0; 
		 break;
		  
	case 2 :
	     /* OR FUNCTION */
		 do
		 {
		 
		 printf("Enter another filename :- ");/*checking length of file name*/
		 (void)scanf("%s",filename);
		 if(strlen(filename) >FILENAME_SIZE)
		   {printf("\n Enter filename again :- \n");
		    
		   }	
		 }while(strlen(filename)>FILENAME_SIZE);
		 length+= strlen(filename) + strlen("-o -name \"\" ");
		 /* checking realloc*/	
		 if(NULL == (p_file_req = (SCHAR*)realloc(p_file_req,sizeof(SCHAR) * length)))
		 {
			A_ERROR(MAJOR, ERROR_REALLOCATION);
			exit(FAILURE);/*exiting*/
		 }
		 else
		
		 /*end of IF*/
		 strcat(p_file_req, "-o -name \"");
		 strcat(p_file_req, filename);
		 strcat(p_file_req, "\" ");
         if( strlen(p_file_req)  > FORMAT_QUERY_SIZE )/* checking total request sizeof after or operation*/
           {
            A_TRACE(DETAILED_TRACE,"ERROR: -EXCEEDED THE QUERY SIZE\n");

            free(p_file_req);/* free previous alloted*/
            
            p_file_req=client_search_req();/* re allot*/
            choice=4;/* making choice 4 to exit from switch after recursive operation*/
            break;
           }
    
		 strcat(dis_req," OR ");
		 strcat(dis_req,filename);
         not_count=0;
		 break;
		  
	case 3 : 
	     /* NOT FUNCTION  */
         if(not_count==0)/* to append 'NOT' only single time in front of a file*/
         {
	     len= (S32_INT)(strlen(filename)+strlen("-name \"\" "));
		 length+= strlen("! ");
	 /* checking malloc allocation */
		 if(NULL == (p_temp1 = (SCHAR*)malloc(sizeof(SCHAR) * (length+1))))
		 {
			A_ERROR(MAJOR, ERROR_MEMORY);
			exit(FAILURE);/* exiting */
		 }
		 else
		 {
		 strncpy(p_temp1,p_file_req,(size_t)(length-(len+2)));
		 /*end of IF*/
		 free(p_file_req);
		 p_file_req=p_temp1;
		 /* Insert ! in between file name */
		 strcat(p_file_req, "! -name \"");
		 strcat(p_file_req, filename);
		 strcat(p_file_req, "\" ");
		 
		 for(i=0;i<(S32_INT)(strlen(dis_req)-strlen(filename));i++);
		 /*end of FOR*/
		 dis_req[i]='\0';
		 strcat(dis_req," NOT ");
		 strcat(dis_req,filename);
		 }
         not_count++;/* incrementing the counter of 'NOT's entered */ 
        }
         else
        {
         A_TRACE(DETAILED_TRACE,"NOT CALLED ONCE AGAIN\n");
        }
           break;
		  
	case 4 : break;
	
	default :printf("ENTER CHOICES AGAIN\n"); /* Re-enter choice*/
	         break;
	         

	}
	
	}while(4 != choice);
	/*end of WHILE*/
	
	A_TRACE(DETAILED_TRACE, "-----EXITING FROM CLIENT_SEARCH_REQ FUNCTION-----\n");
	return p_file_req;

}	

/******************************************************************************
*
 * FUNCTION NAME: client_search_file_req
 *
 * DESCRIPTION: search for file based on query and
 *             display the result of search
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

return_value client_search_file_req (
                        S32_INT connfd /*stores connection socket descriptor*/
                                 )
{
    /*local declarations*/
    return_value ret_val = 0;       /*store function return value*/
    SCHAR    	 *p_format_query = NULL;  /*store formated query*/
    SCHAR 		 *p_file_name = NULL;
	
	p_format_query = client_search_req();
 	(void)system(p_format_query);
 	ret_val = client_send_receive(p_format_query,connfd);
    /*client_send_receive failed*/
    if(FAILURE == ret_val)
    {
        A_ERROR(CRITICAL,ERROR_SEND_RECEIVE);
        /*cleanup*/
        free(p_format_query);
        return FAILURE;
    }/*End of IF*/

    free(p_format_query);
    do
    {
		p_file_name=(SCHAR *)malloc(sizeof(SCHAR)*(FILENAME_SIZE+1));
    }while(p_file_name==NULL);
    
	memset(p_file_name, (S32_INT)'\0', FILENAME_SIZE);
    /*client_get_filename reads name of file whose
      contents are to be viewed */  
    ret_val = client_get_file_content(p_file_name, connfd);
	free(p_file_name);
    /*cleanup*/
    /*client_get_filename failed */
    if(FAILURE == ret_val)
    {
        A_ERROR(MAJOR,ERROR_GET_FILENAME);
        return FAILURE;
    }
    else
    { 
        A_TRACE(BRIEF_TRACE,"---> READY TO SEND FILENAME\n");
        return SUCCESS;
    }

}/*End of Functions*/


