#include <stdio.h>
#include <math.h>

int main()
{
	char pt[1000],mt[10];
	int pd,bd,md,i,cin,temp;

	while(scanf("%d%s%s",&bd,pt,mt)!=-1)
	{
		if(bd != 0)
		{
			md = 0;
			for(i=(strlen(mt)-1);i>=0;i--)
			{
				md+=(mt[i]-'0')*pow(bd,(strlen(mt)-1-i));
			}
		
			cin = 0;
			for(i=0;i<strlen(pt);i++)
			{
				pd = (pt[i]-'0')+cin*bd;
				cin = pd % md;
			}
		
			temp=0;
			for(i=0;cin != 0;i++)
			{
				temp += (cin % bd)*pow(10,i);
				cin = cin / bd;
			}
			printf("%d\n",temp);
		}
	}

	return 0;
}
