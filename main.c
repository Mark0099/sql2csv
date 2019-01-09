
#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[])
{
	if(argc<3)
	{
		printf("Usage: sql2csv file.sql table_name\n");
		return -1;
	}

	char file_sql[1024];
	char file_csv[1024];
	strcpy(file_sql,argv[1]);
	strcpy(file_csv,file_sql);
	char *p=strstr(file_csv,".sql");
	if(p==NULL)
	{
		printf("Input file must be a mysql dump file with .sql suffix\n");
		return -2;
	}
	*p='\0';
	strcat(file_csv,".csv");

	FILE *sql=fopen(file_sql,"r");
	if(sql==NULL)
	{
		printf("Open input file %s FAILED\n",file_sql);
		return -3;
	}
	FILE *csv=fopen(file_csv,"w");
	if(csv==NULL)
	{
		printf("Open output file %s FAILED\n",file_csv);
		fclose(sql);
		return -4;
	}
	char flag[256];
	int flag_len=snprintf(flag,sizeof(flag),"INSERT INTO `%s` VALUES ",argv[2]);
	
	char buf[512];
	int size=fread(buf,1,sizeof(buf)-1,sql);
	buf[size]='\0';

	int ls=0;
	int le=0;
	int out=0;
	char outbuf[1024];
	while(size>0)
	{
		if(out==0)
		{
			p=strstr(buf,flag);
			if(p==NULL)
			{
				memcpy(buf,&buf[size-flag_len],size-flag_len);
				size=fread(&buf[flag_len],1,sizeof(buf)-1,sql)+flag_len;
				buf[size]='\0';
			}
			else
			{
				out=1;
				strcpy(buf,p+flag_len);
				size=strlen(buf);
			}
		}
		if(out==1)
		{
			for(int i=0;i<size;i++)
			{
				if(buf[i]=='(')
				{
					ls=1;
					le=0;
				}
				else if(buf[i]==')')
				{
					if(ls==1 && le>0)
					{
						outbuf[le++]='\n';
						fwrite(outbuf,1,le,csv);
					}
					ls=0;
					le=0;
				}
				else if(buf[i]==';')
				{
					out=-1;
					break;
				}
				else if(ls==1)
					outbuf[le++]=buf[i];
			}	
			if(out==-1)
				size=0;
			else
			{
				size=fread(buf,1,sizeof(buf)-1,sql);
				buf[size]='\0';
			}
		}
	}
	fclose(sql);
	fclose(csv);

	return 0;
}

