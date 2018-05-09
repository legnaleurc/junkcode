#include <stdio.h>

int main()
{
	int n,r,i;
	double avg,sum,temp[1000];
	
	scanf("%d",&n);
	
	while(n--)
	{
		sum = 0.0;
		scanf("%d",&r);
		for(i = 0 ; i < r ; i++)
		{
			scanf("%lf",&temp[i]);
			sum += temp[i];
		}
		avg = sum/(double)r;
		sum = 0.0;
		for(i = 0 ; i < r ;i++)
		{
			if(temp[i] > avg)
				sum++;
		}
		printf("%.3lf%\n",sum/(double)r*100.0);
	}
	
	return 0;
}
