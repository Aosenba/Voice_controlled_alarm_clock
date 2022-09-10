// hmm.cpp : Defines the entry point for the console application.
//

#include<stdio.h.>
#include "stdafx.h"
#include<cstdlib>
#include <string.h>
#include <fstream>
#include <sstream>
#include <tchar.h>
#include <math.h>
#include "clock.h"
using namespace std;
#define N 5
#define M 32
#define No_of_files 15
#define sample_length 50000
#define p 12
#define T 28
#define frame_size 320

long double t_weights[12] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};

FILE *fileA,*fileB,*file_pi,*file_obs,*file_ref,*model_file;

FILE *training_file,*normalized_inputs,*code_book_file,*obs_file;
long double input_samples[sample_length];
long double codebook[M+1][p+1];
char words[4][10] = {  
                         {'s', 't', 'o', 'p', '\0'},
                         {'s', 'n', 'o', 'o', 'z','e','\0'},
						 {'a','m','\0'},
						 {'p','m','\0'}
                     };

long double A[N+1][N+1];
long double B[N+1][M+1];
long double pi[N+1];
long double A_Bar[N+1][N+1];
long double B_Bar[N+1][M+1];
long double pi_Bar[N+1];
long double avg_A[N+1][N+1]={{0}};
long double avg_B[N+1][M+1]={{0}};
long double local_A[N+1][N+1]={{0}};
long double local_B[N+1][M+1]={{0}};
int obs[T+1];

long double alpha[T+1][N+1];
long double beta[T+1][N+1];
long double delta[T+1][N+1];
long double gamma[T+1][N+1];
long double chi[T+1][N+1][N+1];
int psy[T+1][N+1];
int obs_count=0;
long double p_star=0;
int q_star[T+1];
long double probability=0;

char predicted_word[50];
int accuracy_count=0;
float accuracy=0.0;
char word[100];
char cmd[500];
int index=1;
int predicted_digit=0;


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

void read_pi(char word[],int digit)
{
     char file_path[500];
	 if(word==NULL)
		sprintf_s(file_path,"./Models/%d/pi_%d.txt",digit,digit);
	 else
		sprintf_s(file_path,"./Models/%s/pi_%s.txt",word,word);
	 fopen_s(&model_file,file_path,"r");
	 if(!model_file)
	 {
			 printf("Error openning file\n");
	 }
	 while(!feof(model_file))
	 {
			 for(int j=1;j<N+1;j++)
			 {
				 fscanf_s(model_file,"%lf ",&pi[j]);
			 }
	}
	 fclose(model_file);
}

void read_A(char word[],int digit)
{
     char file_path[500];
	 if(word==NULL)
		sprintf_s(file_path,"./Models/%d/A_%d.txt",digit,digit);
	 else
		sprintf_s(file_path,"./Models/%s/A_%s.txt",word,word);
	 fopen_s(&model_file,file_path,"r");
	 if(!model_file)
	 {
			 printf("Error openning file\n");
	 }
	 while(!feof(model_file))
	 {
		 for(int i=1;i<N+1;i++)
		 {
			 for(int j=1;j<N+1;j++)
			 {
				 fscanf_s(model_file,"%lf ",&A[i][j]);
			 }
		 }
	}
	  fclose(model_file);
}
void read_B(char word[],int digit)
{
     char file_path[500];
	 if(word==NULL)
		sprintf_s(file_path,"./Models/%d/B_%d.txt",digit,digit);
	 else
		sprintf_s(file_path,"./Models/%s/B_%s.txt",word,word);
	 fopen_s(&model_file,file_path,"r");
	 if(!model_file)
	 {
			 printf("Error openning file\n");
	 }
	 while(!feof(model_file))
	 {
		 for(int i=1;i<N+1;i++)
		 {
			 for(int j=1;j<M+1;j++)
			 {
				 fscanf_s(model_file,"%lf ",&B[i][j]);
			 }
		 }
	}
	 fclose(model_file);
}
void initialization() // initialization of all matrix
{
	p_star=0.0;
	probability=0.0;
	for(int t=1;t<T+1;t++)
	{
		q_star[t]=0;
		for(int j=1;j<N+1;j++)
		{
			alpha[t][j] = 0.0;
			beta[t][j] = 0.0;
			gamma[t][j] = 0.0;
			delta[t][j] = 0.0;
			psy[t][j] = 0;
		}
	}
	
		for(int i=1;i<N+1;i++)
		{
			for(int j=1;j<N+1;j++)
			{
				for(int t=1;t<T+1;t++)
				{
				chi[i][j][t]=0.0;
				}
			}
		}
}

void read_codebook()
{
	fopen_s(&code_book_file,"codebook.txt","r");

	if(!code_book_file)
	{
		printf("error opening file\n");
	}
		
	while(!feof(code_book_file))
	{
		for(int i=1;i<=M;i++)
		{
			for(int j=1;j<=p;j++)
			{
				fscanf_s(code_book_file,"%lf	", &codebook[i][j]); // reading the uiversal codebook and saving in a universal array
			}
		}
	}
	fclose(code_book_file);
}
void clear_lamda()
{
	for(int i=1;i<N+1;i++)
		{
			for(int j=1;j<N+1;j++)
			{
					A[i][j] = 0;
				
			}
		}
		for(int i=1;i<N+1;i++)
		{
			for(int j=1;j<M+1;j++)
			{
				B[i][j] = 0;
				
			}
		}
}

void read_obs(char word[],int digit,int file_count)
{
	char file_path[500];
   	if(word==NULL && file_count==NULL) //condition for test utterance
	{
		sprintf_s(file_path,"./observation_sequence/test.txt");
	}
	else if(digit==NULL && word!=NULL)
	{
		sprintf_s(file_path,"./observation_sequence/words/%s_%d.txt",word,file_count);
	}
	else
	{
		sprintf_s(file_path,"./observation_sequence/%d_%d.txt",digit,file_count);
	}
	 

	fopen_s(&file_obs,file_path, "r");
	if(!file_obs)
	{
		printf("error reading file\n");
	}
	int index=1;
	while(!feof(file_obs))
	{
		fscanf_s(file_obs,"%d ",&obs[index]); // saving the observation sequence in a universal array
		index++;
	}
	fclose(file_obs);
}
void feed_forward_model()
{
	for(int i=1;i<N+1;i++)
	{
		if(i==1) //make the first state as starting state
			pi[i]=1.0;
		else
			pi[i]=0.0;
	}
				
    for(int i=1;i<N+1;i++) 
	{
		  for(int j=1;j<N+1;j++)
		  {
			  	if(i==j&&i!=N)
					A[i][j]=0.8; //probability of being in current state
				else if(i==j&&i==N)
					A[i][j]=1; //forcing to end the transition in final state
				else if(j==i+1)
					A[i][j]=0.2; //probability to transit to next immediate state
				else
					A[i][j]=0.0; 
		  }
	}
    for(int i=1;i<N+1;i++)
	{
		 for(int j=1;j<M+1;j++)
		 {
			   B[i][j]=1.0/M;
		 }
			
	}
    for(int t=1;t<T+1;t++)
	{
		obs[t]=0;
	}
}

void forward_procedure()
{
	for(int i=1;i<N+1;i++) //initialization
	{
		alpha[1][i] = pi[i] * B[i][obs[1]];
	}
	for(int t=1;t<T;t++) // induction
	{
		for(int j=1;j<N+1;j++)
		{   
			long double sum=0;
			for(int i=1;i<N+1;i++)
			{
				sum += alpha[t][i] * A[i][j];
			}
			alpha[t+1][j] = sum * B[j][obs[t+1]];
		}
	}

	for(int i = 1; i<N+1 ; i++)
	{
	 	probability+=alpha[T][i];
	}
	//printf("probability=%Le\n",probability);
}
void backward_procedure()
{
	for(int i=1;i<N+1;i++) //initialization
	{
		beta[T][i] = 1;
	}
	for(int t=T-1;t>=1;t--) // induction
	{
		for(int i=1;i<N+1;i++)
		{
			long double sum=0;
			for(int j=1;j<N+1;j++)
			{
				sum += A[i][j] * B[j][obs[t+1]] * beta[t+1][j];	
			}
			beta[t][i]= sum;
		}
	}
}
void problem_two()
{
	int arg_max=0;
	for(int i=1;i<N+1;i++) //initialization
	{
		delta[1][i] = pi[i] * B[i][obs[1]];
		psy[1][i]=-1;
	}
	for(int t=2;t<T+1;t++) //induction
	{
		for(int j=1;j<N+1;j++)
		{
			long double max=0;
			int arg_max=0;
			for(int i=1;i<N+1;i++)
			{
				long double temp=delta[t-1][i]*A[i][j];
				if(temp>delta[t-1][arg_max] * A[arg_max][j])
				{
					arg_max=i;
				}
			}
			delta[t][j]= delta[t-1][arg_max] * A[arg_max][j] * B[j][obs[t]];
			psy[t][j]=arg_max;
		}
	}
	arg_max=0;
	for(int i=1;i<N+1;i++)
	{
		if(delta[T][i]>delta[T][arg_max])
			arg_max=i;
	}
	p_star = delta[T][arg_max];

	q_star[T] = arg_max;
	for(int t=T-1;t>=1;t--)
	{
		q_star[t]=psy[t+1][q_star[t+1]];
	}
	

}

void re_estimation()
{
		double divider=0; //used as common devider
		for(int t=1;t<T;t++) //repeat this for all T-1 state transitions
		{
			divider=0;
			for(int i=1;i<N+1;i++)
			{
				for(int j=1;j<N+1;j++)
					divider+=(alpha[t][i]*A[i][j]*B[j][obs[t+1]]*beta[t+1][j]);
			}
			for(int i=1;i<N+1;i++)
			{
				for(int j=1;j<N+1;j++)
					chi[t][i][j]=(alpha[t][i]*A[i][j]*B[j][obs[t+1]]*beta[t+1][j])/divider;
			}
		}
		for(int t=1;t<T;t++)
		{
			for(int i=1;i<N+1;i++)
			{
				long double sum = 0;
				for(int j=1;j<N+1;j++)
				{
					sum+=chi[t][i][j];
				}
				gamma[t][i] = sum;
			}
		}
	for(int i=1;i<N+1;i++)
	{
		pi_Bar[i] = gamma[1][i];
	}
	
	for(int i=1;i<N+1;i++)// re-estimation of A
	{
		
		for(int j=1;j<N+1;j++)
		{
		    long double sum_chi=0;
			long double sum_gamma=0;
			for(int t=1;t<T;t++)
	        {
				sum_chi+=chi[t][i][j];
				sum_gamma += gamma[t][i];
			}	
			A_Bar[i][j] = sum_chi/sum_gamma;
		}	  
	}


	
	for(int j=1;j<N+1;j++) // re-estimation of B
	{
		for(int k=1;k<M+1;k++)
		{
			long double numerator=0;
			long double denomsum=0;
			for(int t=1;t<T+1;t++)
			{
				denomsum+=gamma[t][j];
			}
			for(int t=1;t<T+1;t++)
			{
				if(obs[t]==k)
				{
					numerator+=gamma[t][j];
				}
			}
			long double temp = (numerator/denomsum);
			if((numerator/denomsum)==0)
				temp = 1e-30;
			B_Bar[j][k]=temp;
		}

	}
	
}
void print_converged_lamda(char word[], int count)
{
	printf("\n");
	printf("\n-------------------------------------------------------------------------------------------------------------------\n");
	printf("\n           CONVERGED LAMDA FOR word %s  ,    OBSERVATION SEQUENCE %d           ----------                            \n",word,count);
	printf("\n-------------------------------------------------------------------------------------------------------------------\n");
	printf("probability=%Le\n",probability);
	printf("P* = %Le\n",p_star);
	printf("\n");
	printf("STATE SEQUENCE:\n");
	for(int i=1;i<T+1;i++)
	{
			printf("%d ",q_star[i]);
	}
	printf("\n");
	printf("PI MATRIX:\n");
	for(int i=1;i<N+1;i++)
		printf("%lf", pi_Bar[i]);
	printf("\n");
	printf("\n");
	printf("A MATRIX:\n");
	for(int i=1;i<N+1;i++) //printing A_bar
	{
		for(int j=1;j<N+1;j++)
		{
			printf("%lf   ",A_Bar[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	printf("B MATRIX:\n");
	for(int i=1;i<N+1;i++) //printing B_bar
	{
		for(int j=1;j<M+1;j++)
		{
			printf("%lf   ",B_Bar[i][j]);
		}
		printf("\n");
	}	
}

void update_lamda()
{
	for(int i=1;i <N+1;i++) //assign the given values
        pi[i]=pi_Bar[i];
    for(int i=1;i < N+1;i++) //assign the given values for transition probability distribution
        for(int j=1;j < N+1;j++)
            A[i][j]=A_Bar[i][j];
    for(int i=1;i < N+1;i++) //assign the given values for observation symbol probability distribution
        for(int j=1;j < M+1;j++)
            B[i][j]=B_Bar[i][j];
}



void observation_sequence(long double ci[][p+1],char word[],int digit, int filecount) //find the observation sequence based on shortest distance
{
	long double refdist=0,testdist=0;
	int index=0,tempcount=0;
	int obser_seq[T];
	for(int frame=0;frame<T;frame++)
	{
		refdist=0; //initialize reference distance
		for(int j=1;j<=p;j++) //assume the distance from first vector as reference distance
		{
			long double temp = (ci[frame][j] - codebook[1][j]);
			refdist += t_weights[j-1]*temp*temp; //compute distance from first code vector as reference distance
		}
		refdist/=p;//as per tokhura distance
		index=1; //assume the vector belongs to first cluster
		int cb_count=0;
		for(int i=2;i<M+1;i++)
		{
			cb_count++; // go to next codebook vector
			testdist=0;
			for(int j=1;j<=p;j++) //find the distance from other vectors
			{
				long double temp =0;
				temp = (ci[frame][j] - codebook[i][j]);
				testdist += t_weights[j-1]*temp*temp;  // compute the distance from the center of a cluster
			}
			testdist/=p; //as per tokhura distance formula
			if(testdist<refdist) //make the shortest distance as reference distance and save the correpsonding codebook vector index
			{
				refdist=testdist; //make the current lowest distance as the reference distance
				index=i; //and the vector  belong to corresponding cluster
			}

		}
		obser_seq[frame]=index;
	   }
	    char file_path[500];
		if(word==NULL && filecount==NULL)
		{
			sprintf_s(file_path, "./observation_sequence/test.txt");
		}
		else if(digit==NULL && word!=NULL)
		{
		   sprintf_s(file_path, "./observation_sequence/words/%s_%d.txt", word,filecount); //saving in the observation folder
		}
		else
		{
			 sprintf_s(file_path, "./observation_sequence/%d_%d.txt", digit,filecount);
		}
		fopen_s(&obs_file, file_path, "w");
		
		for(int i=0; i<T; ++i)
		{
				fprintf(obs_file,"%d ", obser_seq[i]);
				
		}
		fclose(obs_file);
		

}
// function for generating observation sequence of 20 utterances of 10 digits
void generate_observation_sequence(char word[],int digit, int file_count)
{
	        char file_path[200];
			long double stable_frames[T+1][frame_size]={{0}},ri[T][p+1] = {{0}}, ai[T][p+1] = {{0}}, ci[T][p+1] = {{0}};

			if(word==NULL && digit==NULL && file_count==NULL) // condition for test utterance
			{
				sprintf_s(file_path, "./training_files/test_files/test.txt");
			}
			else if(digit==NULL && word!=NULL)
			{
				sprintf_s(file_path, "./training_files/words/%s_%d.txt",word,file_count);
			}
			else
			{
				sprintf_s(file_path, "./training_files/digit_%d/%d.txt",digit,file_count);
			}
			
			fopen_s(&training_file,file_path, "r"); 
			if(!training_file)
					printf("Error openning file\n");
			int length=0;
			while(!feof(training_file))
			{
				fscanf_s(training_file,"%lf\n",&input_samples[length]);
				length++;
			}
			fclose(training_file);

			normalization(input_samples,length);

			get_stableframes(input_samples,stable_frames,length);

			apply_hamming_window(stable_frames);
			
			calculate_ri(stable_frames, ri);

			calculate_ai(ri,ai);

			calculate_ci(ri,ai,ci);

		    observation_sequence(ci,word,digit,file_count);
			if(word==NULL && digit==NULL && file_count==NULL) // condition for test utterance
			{
				sprintf_s(file_path, "./reference_files/test.txt"); 
			}
			else if(digit==NULL && word!=NULL)
			{
				sprintf_s(file_path, "./reference_files/words/%s_%d.txt",word,file_count); //saving in the reference folder
			}
			else
			{
				sprintf_s(file_path, "./reference_files/%d_%d.txt",digit,file_count);
			}
			
			fopen_s(&file_ref, file_path, "w");
			for(int i=0; i<T; ++i)
			{
				
				for(int j=1; j<=p; j++)
				{
					fprintf(file_ref, "%lf ", ci[i][j]);
					
				}
				fprintf(file_ref, "\n");
			}
			fclose(file_ref);
     
}
//Initializing average lamda to zeroes
void clear_avg_lamda()
{
	for(int i=1;i<N+1;i++)
	{
		for(int j=1;j<N+1;j++)
		{
			avg_A[i][j] =0;
		}
	}
	for(int i=1;i<N+1;i++)
	{
		for(int j=1;j<M+1;j++)
		{
			avg_B[i][j] =0;
		}
	}
}
//Initializing local lamda to zeroes
void clear_local_lamda()
{
	  for(int i=1;i<N+1;i++)
	  {
		for(int j=1;j<N+1;j++)
		{
				local_A[i][j] =0;
		}
	}
	for(int i=1;i<N+1;i++)
	{
		for(int j=1;j<M+1;j++)
		{
			local_B[i][j] =0;
		}
	}
}
//Re-initializing the lamda value using the average of 20 converged models
void read_avg_lamda()
{
		for(int i=1;i<N+1;i++)
		{
			for(int j=1;j<N+1;j++)
			{
					A[i][j] = local_A[i][j];
				
			}
		}
		for(int i=1;i<N+1;i++)
		{
			for(int j=1;j<M+1;j++)
			{
				B[i][j] = local_B[i][j];
				
			}
		}
}
char *testing(int label)
{
                    int index=0,i=0;
					int length=0;
					if(label==1)
					{
						i=0;
						length=2;
					}
					else
					{
						i=2;
						length=4;
					}

					long double max_prob=0;
				    printf("\n\n--------------------------------------");
					printf("\nPRESS HIT ENTER AFTER RECORDING STOPS\n\n");
					printf("------------------------------------------\n\n");
					char cmd[500];
					sprintf(cmd, "cd training_files && Recording_Module.exe 3 wav\\test.wav test_files\\test.txt");
					system(cmd);
					generate_observation_sequence(NULL,NULL,NULL); //passing null values so that it gets saved saved in a test file
					for(int t=1;t<T+1;t++)
					{
						obs[t]=0;
					}
					read_obs(NULL,NULL,NULL);//passing null values so that it reads the  test file observation sequence
					max_prob=0;
					index=0;
					for( i;i<length;i++)
					{	 
						initialization();
					    clear_lamda();
						read_pi(words[i],NULL);
						read_A(words[i],NULL);
						read_B(words[i],NULL);
						forward_procedure();
						if(probability>max_prob)
						{
							 max_prob = probability;
							index=i;
						}

					}	
					 return words[index];
}
int digit_recog()
{
					int digit=0;
					long double max_prob=0;
					char cmd[500];
					sprintf(cmd, "cd training_files && Recording_Module.exe 3 wav\\test.wav test_files\\test.txt");
					system(cmd);
					generate_observation_sequence(NULL,NULL,NULL); //passing null values so that it gets saved saved in a test file
					for(int t=1;t<T+1;t++)
					{
						obs[t]=0;
					}
					read_obs(NULL,NULL,NULL);//passing null values so that it reads the  test file observation sequence
					max_prob=0;
					index=0;
					for(int i=0;i<=13;i++)
					{	 
						initialization();
					    clear_lamda();
						read_pi(NULL,i);
						read_A(NULL,i);
						read_B(NULL,i);
						forward_procedure();
						if(probability>max_prob)
						{
							 max_prob = probability;
							digit=i;
						}

					}	
					 if(digit==13)
						 digit=30;
					 return digit;
}
void training(char word[],int digit)
{
	            
					int iter=0;
					int count=0;
					if(digit==NULL)
						count=15;
					else	
						count=20;
					clear_local_lamda();
					while(iter<3) // training the model again after taking the average lamda model
					{
				     	iter++;
						clear_avg_lamda();
						printf("\n\n---------------------FOR ITERTAION------------------------------ %d\n",iter);
							
						for(int file_cnt=1;file_cnt<=count;file_cnt++)
						{
							if(iter==1)
							{
								feed_forward_model(); // for the first iteration of all utterances
							}
							else
							{
								read_avg_lamda(); // for the 2nd iteration
							}
								
							for(int t=1;t<T+1;t++)
						    {
								obs[t]=0;
							}
							if(digit==NULL)
								read_obs(word,NULL,file_cnt);
							else
								read_obs(NULL,digit,file_cnt);
						
							int i=0;
							while(i<200)
							{       
									initialization();
									forward_procedure();
									backward_procedure();
									problem_two();
									re_estimation();
									update_lamda();
									i++;
							}
							for(int i=1;i<N+1;i++)
							{
								for(int j=1;j<N+1;j++)
								{
									avg_A[i][j] += A[i][j]; //accumulating each transition probabaility in a average martix
								}
							}
							for(int i=1;i<N+1;i++)
							{
								for(int j=1;j<M+1;j++)
								{
									avg_B[i][j] += B[i][j];
								}
							}
							print_converged_lamda(word,file_cnt);////accumulating each sobervation sequence probabaility in a average martix
				
						} 
						char file_p[500];
						if(digit==NULL && word!=NULL)
							sprintf_s(file_p,"./Models/%s/A_%s.txt",word,word);
						else
							sprintf_s(file_p,"./Models/%d/A_%d.txt",digit,digit);
						fopen_s(&model_file,file_p,"w");
						printf("\n\n AVERAGE LAMDA\n");
						printf("A MATRIX\n");
						for(int i=1;i<N+1;i++)
						{
							for(int j=1;j<N+1;j++)
							{
								local_A[i][j]=avg_A[i][j]/count;
								printf("%lf ",local_A[i][j]);
								fprintf(model_file, "%lf ", local_A[i][j]);
							}
							printf("\n");
							fprintf(model_file, "\n");
						}
						fclose(model_file);
						if(digit==NULL && word!=NULL)
							sprintf_s(file_p,"./Models/%s/B_%s.txt",word,word);
						else
							sprintf_s(file_p,"./Models/%d/B_%d.txt",digit,digit);
						fopen_s(&model_file,file_p,"w");
						printf("\n");
						printf("B MATRIX\n");
						for(int i=1;i<N+1;i++)
						{
							for(int j=1;j<M+1;j++)
							{
								local_B[i][j]=avg_B[i][j]/count;
								printf("%Le ",local_B[i][j]);
								fprintf(model_file, "%Le ", local_B[i][j]);
							}
							printf("\n");
							fprintf(model_file, "\n");
						}
						fclose(model_file);
		     	       	if(digit==NULL && word!=NULL)
							sprintf_s(file_p,"./Models/%s/pi_%s.txt",word,word);
						else
							sprintf_s(file_p,"./Models/%d/pi_%d.txt",digit,digit);
						fopen_s(&model_file,file_p,"w");
						for(int i=1;i<N+1;i++)
						{
								printf("%lf ",pi[i]);
								fprintf(model_file, "%lf ", pi[i]);
						}
						fclose(model_file);
				    	}       
				
}
char day[50];
int hour,mins=0;
int alarm()
{
						
						 printf("Recording time:\n");
						 hour=digit_recog();
					     return hour;
				
}
/*
int _tmain(int argc, _TCHAR* argv[])
{
	read_codebook();//initializing codebook
	int choice=0;
	int hour,mins=0;
	char pre_word[50];
	char str[100],str2[100];
	int digit=0,option=0;
	while(choice<6)
	{
		printf("\n\nEnter choice:\n1. start recording a new word \n2. Start generation observation sequence\n3. Start training the model \n4. Testing\n");
		printf("5. predict digit\n");
		scanf("%d",&choice);
		switch(choice)
		{
		case 1:
			        option=0;
						printf("Enter 1. To generate word  2. To generate digit\n");
						scanf("%d",&option);
						if(option==1)
						{
							printf("Enter the word to start training:");
							scanf("%s",&word);
							for(int file_cnt=1;file_cnt<=15;file_cnt++)
							{
								sprintf(cmd, "cd training_files && Recording_Module.exe 3 wav\\%s_%d.wav words\\%s_%d.txt",word,file_cnt,word,file_cnt);
								system(cmd);
							}
						}
						else
						{
							printf("Enter the digit to start training:");
							scanf("%d",&digit);
							for(int file_cnt=1;file_cnt<=20;file_cnt++)
							{
								sprintf(cmd, "cd training_files && Recording_Module.exe 3 wav\\%d_%d.wav digit_%d\\%d.txt",digit,file_cnt,digit,file_cnt);
								system(cmd);
							}
						}
						break;
		case 2: 
			        option=0;
					printf("Enter 1. To generate word  2. To generate digit\n");
					scanf("%d",&option);
					if(option==1)
					{
						 printf("enter the word to generate observation sequence:");
						 scanf("%s",&word);
						 for(int file_cnt=1;file_cnt<=15;file_cnt++)
						{  
								generate_observation_sequence(word,NULL,file_cnt); 
						}
						  printf("oberserbation sequence generated for word %s",word);
					}
					else
					{
					//	printf("enter the digit to generate observation sequence:");
				 	//	scanf("%d",&digit);
						for(int digit=0;digit<=13;digit++)
						{
							 for(int file_cnt=1;file_cnt<=20;file_cnt++)
							 {  
									generate_observation_sequence(NULL,digit,file_cnt); 
							 }
							  printf("oberserbation sequence generated for digit %d\n",digit);
						}	
				  	}
							
						break;
		case 3:    //Trainning into a final lamda model  
			            digit=0;
			            printf("Enter 1. To train word  2. To train digit\n");
					    scanf("%d",&option);
					 	if(option==1)
					    {
							printf("Enter the word to train:");
							scanf("%s",&word);
							training(word,NULL);
						}
						else
						{
							//printf("Enter the digit to train:");
							//scanf("%d",&digit);
							for(int digit=0;digit<=13;digit++)
							{
								training(NULL,digit);
							}
						}
						break;
			
		case 4:
			   
			        sprintf_s(pre_word,"%s",testing(1));
					printf("%s",pre_word);
				    if (strcmp(pre_word, "set_alarm") == 0) 
					{
						printf("Alarm function begins:\n");
						 printf("\nHour  digit:");
						 hour=digit_recog();
						 printf("hour=%d",hour);
						 printf("\nmin digit:");
						 mins=digit_recog();
						 if(mins==13)
							 mins=30;
						 printf("mins=%d",mins);
						 printf("\nA.M or P.M?:");
						 sprintf_s(day,"%s",testing(2));
						 set_alarm(hour,mins,day);

					} 
					else if (strcmp(pre_word, "set") == 0)
					{
					  printf("Set the  alarm:\n");
					}
					else if (strcmp(pre_word, "snooze") == 0)
					{
							 printf("snooze for 5 mins:\n");
					}
					else	
						 printf("errort\n");
					
					break;
		
		case 5:
			//   printf("predicted_word=%s",testing());
			  predicted_digit=digit_recog();
			  printf("predicted digit= %d", predicted_digit);
			  break;
		default:
			 printf("Invalid input\n");
		}

	}
	
	system("pause");
	return 0;
} 


*/