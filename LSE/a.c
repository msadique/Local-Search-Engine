# include<stdio.h>
# include <string.h>
int main()
{
	int r=0,i=0;
	FILE *fp,*fd;
	char buffer[150],buffer1[5],buffer3[150];
	fp=fopen("g.txt","r");
	if(fp==NULL)
	{
		printf("unable to open the file");
		exit(0);
	}
	while(i<500)
	{
		r=0;
		strcpy(buffer,"file");
		sprintf(buffer1,"%d",i);
		strcat(buffer,buffer1);
        	strcat(buffer,".txt"); 
		fd=fopen(buffer,"w");
		fprintf(fd,"File name file%d.txt\n",i+1);
		while(r<4)
		{	
			fgets(buffer3,150,fp);
			fputs(buffer3,fd);
			memset(buffer3,0,150);
			r++;
		}
		memset(buffer,0,100);
		fclose(fd);
		i++;
	}

fclose(fp);
return 0;

}
