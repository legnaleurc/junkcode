#include <stdio.h>

int main()
{
	int cases,times,height;
	int i,row,col;
	
	scanf("%d",&cases);
	while(cases--)
	{
		scanf("%d%d",&height,&times);
		for(i=0;i<times;i++)
		{
			for(row=1;row<height;row++)
			{
				for(col=1;col<=row;col++)
				{
					printf("%d",row);
				}
				printf("\n");
			}
			for(row=height;row>0;row--)
			{
				for(col=1;col<=row;col++)
				{
					printf("%d",row);
				}
				printf("\n");
			}
			if(i != times-1 || cases != 0)
			{
				printf("\n");
			}
		}
	}
	
	return 0;
}
