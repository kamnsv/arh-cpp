#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <bitset>

using namespace std;


void arh_file(string &fname);
void dearh_file(string &fname);
float get_mean(string &fname);

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

float get_mean(string &fname)
{
	FILE *f = fopen(fname.c_str(), "rb");
	char byte[1];
	int n;
	float sum;
	int j = 0;
	int last = 0;
	while(!feof(f))
    {
        if (fread(byte, 1, 1, f) != 1) continue;
		bitset<8> x(*byte);
		n = (int)(x.to_ulong());
		sum += n;
		//cout <<j<<"."<<x<<" - "<<n<<endl;
		j++;
		last = n;
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
	string data = "";
	
	//first data
	for (int i=0; i < k; i++)
		data += "1";
	data += "0";
	
	while(fread(byte, 1, 1, f))
    {
		bitset<8> x(*byte);
		n = (int)(x.to_ulong());
		//cout <<j<<"."<<x<<" n= "<<n<<endl;
		q = (n - 1) / b;
		r = (n - 1) % b;
		//bits += q + 1 + k;
		//cout <<j<<"."<<x<<" - "<<n<<" q: "<<q<<" r: "<<r<<endl;
		//next data
		
		// q
		for (int i=0; i < q; i++)
			data += "1";
		data += "0";
		
		// r
		if (5 == k)
			data += bitset<5>(r).to_string().substr(0, -k);
		else if (6 == k)
			data += bitset<6>(r).to_string().substr(0, -k);
		else if (7 == k)
			data += bitset<7>(r).to_string().substr(0, -k);
		
		//cout<<"q="<<q<<" r="<<r<<endl;
		//cout<<"bin="<<bitset<8>(r).to_string().substr(0, -k)<<endl;
		int i = 0;
		while (data.size() >= 8) 
		{
			x = bitset<8> (data.substr(0, 8));
			n = (int)(x.to_ulong());
			fwrite(reinterpret_cast<const char *>(&n), 1, 1, arh);
			data = data.substr(8, data.size() - 8);
			bits += 8;
		}	
		
		j++;
    }
	// end 
	if (data.size()) {
		while( data.size()<8 )
			data += "0";
		//cout <<" data part "<<data<<" s "<<data.size()<<endl;
		bitset<8> x = bitset<8> (data.substr(0, 8));
		n = (int)(x.to_ulong());
		fwrite(reinterpret_cast<const char *>(&n), 1, 1, arh);
		bits += 8;
	}
	
	int size = j*8;
	cout<<"result(bits): "<<size<<" -> "<<bits<<endl;
	cout<<"compression ratio: "<<(float)size/(float)bits<<endl;
    
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
	
	// first byte
	if (!fread(byte, 1, 1, f)) 
	{
		cout<<fname<<" invalid format"<<endl;
		return;
	}
	
	string data = "";
	bitset<8> x(*byte);
	data = x.to_string();
	//cout<<"first: "<<data<<endl;
	int k = 0;
	while (data.substr(k, 1) == "1") 
		k++;
	
	int b = pow(2, k);
	data = data.substr(k + 1, data.size() - k - 1);
	cout<<"k: "<<k<<", b: "<<b<<endl;
	
	bool wait_q = true;
	bool wait_r = false;
	int q, j, n, r;
	int bytes = 0;
	while(!feof(f) || data.size() >= k)
    {
		//cout<<j<<"-----------"<<endl;
		if (fread(byte, 1, 1, f))
		{
			bitset<8> x(*byte);
			//cout<<" add "<<x.to_string()<<endl;
			data += x.to_string();
		}
		//cout<<" data next "<<data<<" s "<<data.size()<<endl;
		if (wait_q) 
		{
			//cout <<"wait_q"<<endl;
			q = 0;
			while (data.substr(q, 1) == "1")
				q++;
			//cout<<" -> q = "<<q<<endl;
			try {
				data = data.substr(q + 1, data.size() - q - 1);
			} catch (const exception& e) {}
			
			//cout<<" part r "<<data<<" s "<<data.size()<<endl;
			wait_q = false;
			wait_r = true;
			
		}
		
		if (wait_r) 
		{
			//cout <<"wait_r"<<endl;
			if (data.size() >= k)
			{
				//cout <<"data.size >= k"<<endl;
				
				
				
				r = (int)bitset<8>(data.substr(0, k)).to_ulong();
				
				//cout <<" data r "<<data.substr(0, k)<<endl;
				
				//cout <<" r = "<<r<<endl;
				
				n = q * b + r + 1;
				//cout <<bytes<<"."<<" n = "<<n<<endl;
				fwrite(reinterpret_cast<const char *>(&n), 1, 1, dearh);
				bytes++;
				try {
					data = data.substr(k, data.size() - k);
				} catch (const exception& e) {}
				//cout <<" data part "<<data<<" s "<<data.size()<<endl;
				
				wait_r = false;
				wait_q = true;
			}
				
		}
		//cout <<wait_q<<wait_r<<" data part "<<data<<" s "<<data.size()<<endl;
		j++;
    }
	//cout <<wait_q<<wait_r<<" data part "<<data<<" s "<<data.size()<<endl;
    fclose(f);
	
	fclose(dearh);
}