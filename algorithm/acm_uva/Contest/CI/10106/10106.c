#include <stdio.h>

int main()
{
	char a[250],b[250];
	int x[250],y[250],p[500]={0},count=0;
	int i,j,temp,al,bl;
	
	while(scanf("%s%s",a,b)!=-1)
	{
		al = (int)strlen(a);
		bl = (int)strlen(b);
		for(i=al-1;i>=0;i--)
			x[al-1-i]=a[i]-48;
		for(i=bl-1;i>=0;i--)
			y[bl-1-i]=b[i]-48;
		for(i=0;i<al;i++)
			for(j=0;j<bl;j++)
			{
				temp = x[i] * y[j];
				p[i+j] += temp % 10;
				if(p[i+j]>=10)
				{
					p[i+j+1] += p[i+j] / 10;
					p[i+j] = p[i+j] % 10;
				}
				p[i+j+1] += temp / 10;
			}
		for(i=500-1;i>=0;i--)
			if(p[i]!=0)
				break;
		if(i==-1)
			putchar('0');
		else
			for(;i>=0;i--)
				printf("%d",p[i]);
		putchar('\n');
		for(i=0;i<500;i++)
			p[i] = 0;
		count = 0;
	}
	
	return 0;
}
