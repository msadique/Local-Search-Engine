/******************************************************************************
 *  FILE NAME    : s_funcs.c
 *
 *  DESCRIPTION  : contains the functions for the client side
 *
 *  DATE            NAME        REFERENCE          REASON
 *  6-Dec-2010    Mohd Sadique     PRISM PROJECT      Initial Creation

 *  
 *
 ******************************************************************************/


/*******************************************************************************
 *      HEADER FILES
 **************************************************************************/

#include <s_header.h>


/*******************************************************************************
 *
 * FUNCTION NAME: server_send_content
 *
 * DESCRIPTION: sends the content in buffer through socket
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

static return_value server_send_content(
		SCHAR *p_temp, /*character array*/
		S32_INT sockfd  /*connected socket descriptor*/
		)
{
	/*local declarations*/  
	S32_INT pointer = 0,             /*points to the current location in buffer*/
	write_count = 0,         /*count of number of characters written*/
	len = (S32_INT)strlen(p_temp); /*length of string*/

	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER_SEND_CONTENT FUNCTION-----\n");

	while (pointer < len)
	{
		/*write content in temp to socket*/
		write_count = (S32_INT)write(sockfd,
				(p_temp + pointer),
				(size_t)(len - pointer));
		/*writing failed*/
		if(0 > write_count)
		{
			A_ERROR(MAJOR,ERROR_WRITE_SOCKET);
			return FAILURE;/*return failure*/
		}

		/*increase by no. of character written*/
		pointer = pointer + write_count;
	}

	A_TRACE(DETAILED_TRACE,"-----EXITING SERVER_SEND_CONTENT FUNCTION-----\n");
	return SUCCESS;/*return success*/

}/*End of function*/


/*******************************************************************************
 *
 * FUNCTION NAME: server_get_filename
 *
 * DESCRIPTION: get filename from socket
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

static return_value server_get_filename(
		SCHAR *p_filename, /*filename*/
		S32_INT connfd      /*connected socket descriptor*/
		)
{
	/*local declarations*/
	S32_INT read_count = 0; /*number of characters read*/

	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER_GET_FILENAME FUNCTION-----\n");

	/*read file name from socket*/
	read_count = (S32_INT)read (connfd, p_filename, FILE_NAME_SIZE);
	/*read failed*/
	if(0 >= read_count) 
	{
		A_TRACE(DETAILED_TRACE,"-----EXITING SERVER_GET_FILENAME FUNCTION UNSUCCESSFULLY-----\n");
		return FAILURE;/**/
	}      
	/*read successful*/
	A_TRACE(DETAILED_TRACE,"-----EXITING SEREVER_GET_FILENAME FUNCTION SUCCESSFULLY-----\n");

	return SUCCESS;/*return success*/

}/*End of function*/


/*******************************************************************************
 *
 * FUNCTION NAME: server_read_file
 *
 * DESCRIPTION: reads file content and sends the content through socket
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

static return_value server_read_file(
		SCHAR *p_temp, /*character array*/
		S32_INT  confd   /*connected socket descriptor*/
		)
{
	/*local declarations*/
	S32_INT             file_fd = 0,    /*file descriptor*/
	read_count = 0; /*number of characters read*/
	SCHAR            buffer[SIZE];   /*character buffer*/
	return_value    ret_fun = 0;    /*return value form function*/

	memset (buffer, (S32_INT)'\0', SIZE);
	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER_READ_FILE FUNCTION-----\n");

	/*open file in read only mode*/
	file_fd = open (p_temp, O_RDONLY);

	/*file openning failed*/
	if (-1 == file_fd)
	{
		A_ERROR(MAJOR,ERROR_OPEN_FILE);
		strcpy(buffer,
				"\n---> ERROR OPENING FILE. FILE NOT FOUND!!!\n\n");  

		/*send error opening file through socket*/
		ret_fun = server_send_content (buffer, confd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{   
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			return FAILURE;
		}

		memset (buffer, (S32_INT)'\0', SIZE);
		strcpy(buffer,"error!@#$");

		/*send error marking character*/
		ret_fun = server_send_content (buffer, confd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			return FAILURE;
		}

		memset (buffer, (S32_INT)'\0', SIZE);
		return FAILURE;/*return failure*/
	}/*End of outer IF*/

	while (1 == 1)
	{
		A_TRACE(BRIEF_TRACE,"-----ENTERING LOOP FOR READING FROM FILE-----\n");

		/*read file content*/
		read_count = (S32_INT)read(file_fd, buffer, SIZE);

		/*read successful. send file content through socket*/
		if (0 < read_count)
		{   
			ret_fun = server_send_content (buffer, confd);
			/*sending failed*/
			if(FAILURE == ret_fun)
			{
				A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
				return FAILURE;/*return failure*/
			}
			memset (buffer, (S32_INT)'\0', SIZE);
		}/*End of outer IF*/

		/*read failed*/
		if (0 >= read_count)
		{
			break;/*break out of the loop*/
		}
	}/*End of WHILE*/

	(void)close (file_fd);/*close the file*/

	A_TRACE(DETAILED_TRACE,"-----EXITING SERVER_READ_FILE FUNCTION-----\n");
	return SUCCESS;/*return success*/

}/*End of function*/


/*******************************************************************************
 *
 * FUNCTION NAME: server_read_request
 *
 * DESCRIPTION: get the type of client request
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

return_value server_read_request(
		S32_INT connfd, /*connected socket descriptor*/
		S32_INT *p_type   /*request type*/
		)
{
	/*local declarations*/
	SCHAR temp_buff[4];  /*array to store read characters from socket*/
	S32_INT  read_count=0;  /*number of characters rad*/

	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER_READ_REQUEST FUNCTION-----\n");

	memset(temp_buff, (S32_INT)'\0', sizeof(temp_buff));

	/*read 4 char from socket*/
	read_count = (S32_INT)read (connfd, temp_buff, 4);
	/*read fails then return with error message*/
	if(0 >= read_count)
	{
		*p_type = -1;
		A_ERROR(MINOR,ERROR_INVALID_TYPE);
		A_ERROR(MAJOR,ERROR_READ_SOCKET);
		return FAILURE;
	}       

	/*4 bytes read from socket is compared for find or file.
	  correspondingly the type value is set*/
	else if (0 == strncmp (temp_buff, "quit", QUERY_SIZE))
		*p_type = 3;
	if (0 == strncmp (temp_buff, "find", QUERY_SIZE) )
		*p_type = 2;
	else if (0 == strncmp (temp_buff, "file", QUERY_SIZE))
		*p_type = 1;

	A_TRACE(DETAILED_TRACE,"-----EXIING SERVER_READ_REQUEST FUNCTION-----\n");
	return SUCCESS;

}/*End of function*/


/*******************************************************************************
 *
 * FUNCTION NAME: server_search_files
 *
 * DESCRIPTION: searches for the file based on search query
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

return_value server_search_files(
		S32_INT connfd /*connected socket descriptor*/
		)
{
	/*local declarations*/
	S32_INT     fd[2] = {0,0},      /*file descriptor for pipe*/
				read_count = 0,     /*number of characters read*/
				close_ret = 0,      /*return value from close function*/
				ret_fun = 0,        /*return value from function*/
				counter = 0,        /*counter for buffer*/
				num_file = 0,       /*number of files found*/
				iterator = 0,       /*number of lines to read from searched file*/
				status=0;           /*wait status*/
	FILE    	*p_file = NULL;     /*pointer to file*/
	SCHAR    	query[LINE_SIZE],   /*search query*/
				temp[1],            /*store one character from stdin*/
				find_query[FIND_QUERY_SIZE],    /*find query*/
				line_out[LINE_SIZE];/*line out buffer*/
	struct stat buf;           /*stat buffer*/
	pid_t   	pid;                /*process ID*/

	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER_SEARCH_FILES FUNCTION-----\n");
	memset (query, (S32_INT)'\0', LINE_SIZE);
	memset (line_out, (S32_INT)'\0', LINE_SIZE);
	memset(find_query, (S32_INT)'\0', FIND_QUERY_SIZE);
	memset (temp, 0, 1);

	/*create a pipe*/
	if (FAILURE == pipe (fd))
	{   
		/*failed to create pipe*/
		A_ERROR(MAJOR,ERROR_PIPE);
		(void)close (connfd);
		exit (FAILURE);
	}

	/*read search query from socket*/
	read_count = (S32_INT)read (connfd, query, LINE_SIZE);
	/*reading from socket failed*/
	if(0 > read_count)
	{
		A_ERROR(MAJOR,ERROR_READ_SOCKET);
		return FAILURE;
	}   

	/*create a sub process*/
	pid = fork();
	/*fork failed*/
	if(FAILURE == pid)
	{
		A_ERROR(CRITICAL,ERROR_FORK);
		(void)close(connfd);
		return FAILURE;
	}

	/*child process*/    
	if (CHILD_ID == pid)
	{
		A_TRACE(DETAILED_TRACE,"-----ENTERED SEARCH CHILD-----\n");  

		/*format the query as required by find command*/
		strcpy(find_query,"find ");
		strcat(find_query,g_path);
		strcat(find_query," ");
		strncat (find_query, query, (size_t)read_count);

		(void)close (1);            /*close standard output*/
		(void)close (fd[0]);       /*close read end of pipe*/
		(void)dup (fd[1]);          /*duplicate write end of pipe*/
		(void)system (find_query);  /*execute find command*/

		A_TRACE(DETAILED_TRACE,"-----EXITING CHILD-----\n");
		exit (SUCCESS);
	}/*End of IF*/
	else
	{
		A_TRACE(DETAILED_TRACE,"---> WAITING FOR SEARCH CHILD TO TERMINATE\n");

		(void)close (0);    /*close standard input*/
		(void)dup (fd[0]);  /*duplicate read end of pipe*/
		(void)close (fd[1]);/*close write end of pipe*/

		memset (query, (S32_INT)'\0', LINE_SIZE);

		while (read (0, temp, 1) > 0)/*read a character from standard input*/
		{
			/*if new line is not recieved, store it in buffer*/
			if ('\n' != *temp)
			{
				query[counter] = *temp;
				counter++;
			}/*End of IF*/

			/*new line is recieved*/
			else
			{   
				/*statistic of file are stored in buf*/
				if (-1 == stat (query, &buf))
					break;
			//	free(&buf.st_ctim);   /*To remove splint errors */
			//	free(&buf.st_mtim);
			//	free(&buf.st_atim);

				num_file++;

				(void)snprintf (line_out,
								LINE_SIZE,
								"\n%d. FILE PATH : %s\t\tFILE SIZE : %d\n",
								num_file,
								query,
								(S32_INT)buf.st_size);

				/*send filename and filesize through socket*/
				ret_fun = server_send_content (line_out, connfd);
				/*sending failed*/  
				if(FAILURE == ret_fun )
				{ 
					A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
					break;
				}

				memset (line_out, (S32_INT)'\0', LINE_SIZE);

				p_file  = fopen (query, "r");/*open file for reading*/
				/*file opening failed*/
				if(NULL == p_file)
				{   
					A_ERROR(MAJOR,ERROR_OPEN_FILE);
					break;
				}
				/*read two line from opened file and send it through socket*/
				iterator = 0;

				while(2 > iterator)
				{
					/*read a line from file*/
					(void)fgets(line_out,LINE_SIZE,p_file);

					/*send the line read through socket*/
					ret_fun = server_send_content (line_out, connfd);
					/*sending failed*/
					if( FAILURE == ret_fun)
					{
						A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
						break;
					}         
					memset (line_out, (S32_INT)'\0', LINE_SIZE);
					iterator++;
				}/*End of inner WHILE*/

				close_ret = fclose(p_file);/*close the file*/
				/*file closing failed*/
				if(0 != close_ret)
				{
					A_ERROR(MINOR,ERROR_CLOSE_FILE);
					return FAILURE;
				}       
				p_file = NULL;
				memset (line_out, (S32_INT)'_',(LINE_SIZE-1));

				/*send file seperator through socket*/
				ret_fun = server_send_content (line_out, connfd);
				/*sending file demarkator failed*/
				if(FAILURE == ret_fun)
				{       
					A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
					break;
				}

				memset (line_out, (S32_INT)'\0', LINE_SIZE);
				memset (query, (S32_INT)'\0', LINE_SIZE);
				memset (&buf, 0, sizeof (struct stat));
				counter = 0;

			}/*End of outer ELSE*/
		}/*End of outer WHILE*/

		(void)wait (&status);/*collect the exit status of child*/

		/*No files mathced search criteria*/    
		if(0 == num_file)
		{
			A_TRACE(BRIEF_TRACE,"---> NO MATCHING FILE FOUND\n");
			memset (line_out, (S32_INT)'\0', LINE_SIZE);
			strcpy (line_out, "NO FILE FOUND MATCHING THE SEARCH CRITERIA!!!--\n");

			/*send file no matching message through socket*/
			ret_fun = server_send_content (line_out, connfd);
			/*sending failed*/
			if(FAILURE == ret_fun)  
			{
				A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
				return FAILURE;
			}       

			memset (line_out, (S32_INT)'\0', LINE_SIZE);
			strcpy (line_out, "error!@#$");

			/*send error marking character*/
			ret_fun = server_send_content (line_out, connfd);
			/*sending failed*/
			if(FAILURE == ret_fun)
			{
				A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
				return FAILURE;
			}

			memset (line_out, (S32_INT)'\0', LINE_SIZE);
			strcpy (line_out, "quit!@#$");

			/*send termination character*/
			ret_fun = server_send_content (line_out, connfd);
			/*sending failed*/
			if(FAILURE == ret_fun)
			{
				A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
				return FAILURE;
			}

			A_TRACE(BRIEF_TRACE,"---> SENT TERMINATION CHARACTER\n");
			return FAILURE;/*return failure*/

		}/*End of outer IF*/

		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy (line_out, "quit!@#$");

		/*send termination character*/
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FAILED\n");
			return FAILURE;
		}

		A_TRACE(BRIEF_TRACE,"---> SENT TERMINATION CHARACTER\n");      
		A_TRACE(DETAILED_TRACE,"-----EXITING SERVER_SEARCH_FILES SUCCESSFULLY-----\n");
		return SUCCESS;/*return success*/
	}

}/*End of function*/


/*******************************************************************************
 *
 * FUNCTION NAME: server_validate_port
 *
 * DESCRIPTION: process the file for responding to client
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

return_value server_validate_port(
		S32_INT port /*port number*/
		)
{
	/*check if port number is within range*/
	if(port>MINPORT && port < MAXPORT)
		return SUCCESS;
	else
		return FAILURE;
}/*End of function*/


/*******************************************************************************
 *
 * FUNCTION NAME: server_validate_ip
 *
 * DESCRIPTION: process the file for responding to client
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

return_value server_validate_ip(
		SCHAR *p_ip_address /*IP address*/
		)
{
	S32_INT dot_count = 0,  /*number of dots count*/
			int_count = 0,  /*number of numeral count*/
			temp_value =0;  /*store value of each dot seperated number*/

	/*check if first character of IP is valid*/ 
	if( '0'<= *p_ip_address && '9'>= *p_ip_address)
	{
		temp_value = (int)*p_ip_address - 48;
		p_ip_address++;
	}
	else
		return FAILURE;

	/*count number of characters and numerals and check numeral value*/
	while( '\0'!=*p_ip_address)
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
			temp_value = temp_value * 10 +(int) *p_ip_address - 48;
			int_count++;
		}
		/*unrecognized character*/
		else
		{
			A_TRACE(DETAILED_TRACE,
					"---> UNRECOGNISED CHARACTER ENCOUNTERED IN IP ADDRESS\n"
				   );
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


/*******************************************************************************
 *
 * FUNCTION NAME: server_send_file
 *
 * DESCRIPTION: process the file for responding to client
 *
 * RETURNS: SUCCESS or FAILURE
 ******************************************************************************/

return_value server_send_file(
		S32_INT connfd /*connected socket descriptor*/
		)
{
	/*local declarations*/
	SCHAR    *p_filename = NULL,     /*filename*/
		   	 line_out[LINE_SIZE];    /*line out*/
	S32_INT  ret_fun = 0;            /*return value from function*/

	A_TRACE(DETAILED_TRACE,"-----ENTERING SERVER_SEND_FILE FUNCTION-----\n");

	/*allocate memory*/
	p_filename = (SCHAR *) malloc (sizeof (SCHAR) * (FILE_NAME_SIZE+1));

	if(NULL != p_filename)
		memset (p_filename, (S32_INT)'\0', FILE_NAME_SIZE);

	/*memory allocation failed*/
	if(NULL == p_filename)
	{
		A_ERROR(CRITICAL,ERROR_MEMORY);
		return FAILURE;
	}

	/*get filename from socket*/
	ret_fun = server_get_filename (p_filename, connfd);
	/*server_get_filename failed*/
	if(FAILURE == ret_fun)
	{
		A_TRACE(BRIEF_TRACE,"---> READING OF FILENAME FAILED\n");
		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy (line_out, "ERROR IN READING FILENAME!!\n");

		/*send reading filename failed through socket*/
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{   
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			free(p_filename);/*free allotted memory*/
			return FAILURE;/*return failure*/
		}

		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy (line_out, "error!@#$");

		/*send error markation through socket*/
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			free(p_filename);/*free allotted memory*/
			return FAILURE;/*return failure*/
		}

		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy (line_out, "quit!@#$");

		/*send termination character through socket*/   
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FAILED\n");
			free(p_filename);/*free allotted memory*/
			return FAILURE;/*return failure*/
		}

		A_TRACE(BRIEF_TRACE,"---> SENT TERMINATION CHARACTER\n");
		free(p_filename);/*free allotted memory*/
		return FAILURE;/*return failure*/
	}/*End of outer IF*/

	/*check for proper path in file name recieved*/ 
	if(NULL == strstr(p_filename,g_path))
	{
		A_TRACE(BRIEF_TRACE,"---> PATH NOT MATCHING\n");
		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy(line_out,
				"YOU DON'T HAVE PERMISSION TO ACCESS THIS PATH!!!\n");

		/*send permission denied through socket*/
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{   
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			free(p_filename);/*free memory allotted*/   
			return FAILURE;/*return failure*/
		}

		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy (line_out, "error!@#$");

		/*send error markation through socket*/
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			free(p_filename);/*free memory allotted*/   
			return FAILURE;/*return failure*/
		}

		memset (line_out, (S32_INT)'\0', LINE_SIZE);
		strcpy (line_out, "quit!@#$");

		/*send termination character through socket*/
		ret_fun = server_send_content (line_out, connfd);
		/*sending failed*/
		if(FAILURE == ret_fun)
		{
			A_TRACE(BRIEF_TRACE,"---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
			free(p_filename);/*free allotted memory*/   
			return FAILURE;/*return error*/
		}

		A_TRACE(BRIEF_TRACE,"---> SENT TERMINATION CHARACTER\n");
		free(p_filename);/*free allotted memory*/   
		return FAILURE; /*return failure*/
	}/*End of outer IF*/

	/*read and send file content through socket*/
	ret_fun = server_read_file (p_filename, connfd);
	/*server_read_file() failed*/
	if(FAILURE == ret_fun)
	{
		A_TRACE(BRIEF_TRACE,"---> SERVER_READ_FILES FUNCTION FAILED\n");
		free (p_filename);/*free allotted memory*/
		return FAILURE;/*return failure*/
	}

	memset (line_out, (S32_INT)'\0', LINE_SIZE);
	strcpy (line_out, "quit!@#$");

	/*send termination character through socket*/
	ret_fun = server_send_content (line_out, connfd);
	/*sending failed*/
	if(FAILURE == ret_fun)
	{
		A_TRACE(BRIEF_TRACE, "---> SERVER_SEND_CONTENT FUNCTION FAILED\n");
		free (p_filename);/*free allotted memory*/
		return FAILURE;/*return failure*/
	}

	A_TRACE(BRIEF_TRACE, "---> SENT TERMINATION CHARACTER\n");
	free (p_filename);/*free allotted memory*/
	A_TRACE(DETAILED_TRACE,
			"-----EXITING SERVER_SEND_FILES FUNCTION SUCCESSFULLY-----\n");
	return SUCCESS;/*return success*/

}/*End of function*/
