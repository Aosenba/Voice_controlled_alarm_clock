
void calculate_ri(long double stable[][frame_size],long double r[][p+1]){
	for(int i=0; i<T; i++)
	{
		for(int k=0; k<=p; ++k)
		{
			double square_sum=0;
			for(int m=0; m<frame_size-k; m++)
			{
				square_sum += stable[i][m]*stable[i][m+k];
			}
			r[i][k]=square_sum/frame_size;
		}
	}
}
void calculate_ai(long double r[][p+1], long double a[][p+1]){
	for(int frame=0; frame<T; frame++)
	{
		long double e[p+1] = {0}, k[p+1] = {0},  alpha[p+1][p+1]={{0}};
		e[0] = r[frame][0];
		for(int i=1; i<=p; i++)
		{
			if(i==1)
				k[1]=r[frame][1]/r[frame][0];
			long double sum = 0;
			for(int j=1; j<=i-1; j++)
			{
			
					sum += alpha[i-1][j]*r[frame][i-j];
			}
		    long double h=r[frame][i]-sum;
			k[i] = (r[frame][i]-sum)/e[i-1];
			alpha[i][i] = k[i];
			for(int j=1; j<=i-1; j++)
			{
				alpha[i][j] = alpha[i-1][j] - k[i] * alpha[i-1][i-j];
			}
			e[i] = (1-k[i]*k[i])*e[i-1];
		}
		for(int i=1; i<=p; i++){
			a[frame][i] = (alpha[p][i]);
		}
	}
}
long double raised_sin(int m){
	return 1+(p*1.0/2)*sin(3.14*m/12);
}
void calculate_ci(long double ri[][p+1], long double ai[][p+1], long double ci[][p+1]){
	for(int f=0; f<T; f++)
	{
		long double sig = ri[f][0];
		ci[f][0] = log10(sig*sig);
		for(int m=1; m<=p; m++){
			long double sum = 0;
			for(int k=1; k<=m-1; k++){
				sum += k*ci[f][k]*ai[f][m-k]/m;
			}
			ci[f][m] = ai[f][m] + sum;
		}
	}

	//applying raised sin window
	for(int i=0; i<T; ++i){
		for(int j=1; j<=p; ++j){
			ci[i][j] *= raised_sin(j);
		}
	}
}
 
void apply_hamming_window(long double stable[][frame_size]){
	for(int i=0; i<T; i++)
	{
		for(int j=0; j<frame_size; j++)
		{
			double m=stable[i][j];
			stable[i][j]*=0.54-0.46*cos(2*3.14*m/frame_size-1);
		}
	}

}

void get_stableframes(long double sample[],long double stable_frames[][frame_size],int length)
{
	long double max_energy=0;
	double sf[200][frame_size]={{0}};
	int sampleCnt = 0, frameCnt =0, pos = 0;
	int j=0;
	long double sum=0;
	for(int i=0;i<length;i++)
	{
			    if(j>=frame_size)
				{
					j=0;
					frameCnt++; // calculating the no of frames of size 320
				}
				sf[frameCnt][j]=sample[i];
				j++;	
	}
	long double  energy[200]={0};
	int x=0;
	//calculating energy
	for(int i=0;i<frameCnt;i++)
	{
		sum=0;
		for(int j=0;j<frame_size;j++)
		{
			sum+=sf[i][j] * sf[i][j];
		}
		energy[x]=sum/frame_size; 
		x++;
	}
	//finding maximum energy
	for(int i=0;i<frameCnt;i++)
	{
		if(energy[i]>max_energy)
		{
			max_energy=energy[i];
			pos=i-1;
		}

	}

	frameCnt=0;
	for(int i=pos-13;i<pos+13;i++)
	{
	    sampleCnt=0;
		for(int j=0;j<frame_size;j++)
		{
			stable_frames[frameCnt][sampleCnt]=sf[i][j];
			sampleCnt++;
		}
		frameCnt++;
	}

   return;
} 

void normalization(long double samples[],int length)
{
 
	long double sum=0,dc_shift=0, maxfactor=0,xmax=0;
	for(int i=0;i<length;i++)
	{
		sum+=samples[i];
	}
	dc_shift = (sum/length)*5;
	//finding the maximum  frequency from the array
	for(int i = 0; i < length; i++)
    {
		if(samples[i]>xmax)
		{
			xmax=samples[i];
		}
    }
 //calculating  normalizing factor
   maxfactor=5000/xmax;
   for(int i=0;i<length;i++)
	{
			samples[i]=((samples[i]-dc_shift)*maxfactor);
		
	}
   return;
	
}
