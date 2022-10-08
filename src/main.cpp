#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <bitset>

using namespace std;

float get_mean(string &fname)
{
	FILE *f = fopen(fname.c_str(), "rb");
	char byte[1];
	int n;
	float sum;
	int j = 0;
	while(!feof(f))
    {
        if (fread(byte, 1, 1, f) != 1) continue;
		bitset<8> x(*byte);
		n = (int)(x.to_ulong());
		sum += n;
		j++;
		//cout <<j<<"."<<x<<" - "<<n<<endl;
    }
	//cout<<endl;
	fclose(f);
	return sum/j;
}

void arh_file(string &fname)
{
    FILE *f = fopen(fname.c_str(), "rb");
    if(!f)
	{ 
		cout<<fname<<" not found!"<<endl;
		return;
	}
	
	cout<<"Archiving RiceEncoding: "<<fname<<endl;
	string out = fname + ".arh";
	FILE *arh=fopen(out.c_str(), "wb");
	
	float mean = get_mean(fname);
	int k = (int)log2f(mean);
	int b = pow(2, k);
	
	cout<<"mean: "<<mean<<endl;
	cout<<"k: "<<k<<", ";
	cout<<"b: "<<b<<endl;
	
    char byte[1];
	int n, q, r;
	int j = 0;
	int bits = 0;
	while(!feof(f))
    {
        if (fread(byte, 1, 1, f) != 1) continue;
		bitset<8> x(*byte);
		n = (int)(x.to_ulong());
		q = (n - 1)/b;
		r = (n - 1)%b;
		bits += q + 1 + k;
		
		
		/*
		if (n != 144 and n != 34 and n != 113 and n !=162) continue;
		cout <<j<<"."<<x<<" - "<<n<<" -> "<<q<<": ";
		for (int i=0; i < q; i++)
			cout <<"1";
		cout <<"0 ";
		cout <<bitset<8>(r).to_string()<<endl;
		*/
		
		//fwrite(byte, 1, 1, arh);
		j++;
    }
	int size = j*8;
	cout<<"reuslt(bits):"<<size<<" -> "<<bits<<endl;
	cout<<(float)bits/(float)size<<"% "<<endl;
    
	fclose(arh);
	fclose(f);
}




void dearh_file(string &fname)
{
    FILE *f = fopen(fname.c_str(), "rb");
    if(!f)
	{ 
		cout<<fname<<" not found!"<<endl;
		return;
	}
	
	cout<<"Dearchiving: "<<fname<<endl;
	string out = fname + ".dearh";
	FILE *dearh=fopen(out.c_str(), "wb");
    char byte[1];
	while(!feof(f))
    {
        if(fread(byte, 1, 1, f) == 1) 
			fwrite(byte, 1, 1, dearh);
    }
    fclose(f);
	
	fclose(dearh);
}



int main(int argc, char* argv[])
{
    
    setlocale(LC_ALL,"en");
	   
	if(argc < 3) 
	{
		cout<<"The filename and the -arh/-dearh operation are required!"<<endl;
		return 1;
	}

	// Действие с файлом
	string fname (argv[1]);
	string action(argv[2]);
	
	if ("-arh" == action)
		arh_file(fname);
	
	else if ("-dearh" == action)
		dearh_file(fname);
	
	else 
		cout<<"Command "<<action<<" is undefined"<<endl;

	
	return 0;

}