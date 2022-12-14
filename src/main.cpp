#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <bitset>
#include <ctime> 

using namespace std;

string arh_file(string fname, int k, bool debug);
string dearh_file(string fname, int k, bool debug);

int main(int argc, char* argv[])
{

	setlocale(LC_ALL, "en");

	if (argc < 3)
	{
		cout << "Archiving with Rice encoding." << endl << endl;
		cout << "Use [FILENAME] [ACTION] [OPTIONS]." << endl;

		cout << endl << "\tFILENAME: The path to the archived file." << endl;
		
		cout << endl << "\tACTIONS:" << endl << endl;
		cout << "\t--arh   \t Archive file, output archive with '.arh' extension." << endl;
		cout << "\t--dearh \t Dearchive the file, the output is the original file, but with the extension '.dearh'." << endl;

		cout << endl << "\tOPTIONS:" << endl << endl;
		cout << "\t--k     \t Parameter [1;8] for encoding Rice, default 6." << endl;
		cout << "\t--debug \t Optional flag to display information about bytes." << endl;
		
		return 0;
	}

	string fname(argv[1]);
	int k = 6;
	bool debug = false;
	bool arh = false;
	bool dearh = false;

	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);

		if ("--debug" == arg)
			debug = true;

		if ("--arh" == arg)
			arh = true;

		if ("--dearh" == arg)
			dearh = true;

		try {

			if ("--k" == arg)
				k = atoi(argv[i + 1]);

			if (k < 1 || k > 8) {
				cout << "Possible values of the encoding parameter in the range from 1 to 8 inclusive" << endl;
				return 1;
			}

		}
		catch (exception& e) {
			cout << "Please enter a natural number to encode after the '--k' option:" << e.what() << endl;
			return 1;
		}

	}

	string out;

	if (arh) {
		out = arh_file(fname, k, debug);

		if ("" == out)
			return 1;

		cout << "Output name of the archived file: " << out << endl;
		if (dearh)
		{
			out = dearh_file(out, k, debug);

			if ("" == out)
				return 1;

			cout << "Output name of the dearchived file: " << out << endl;
		}
	}
	else if (dearh)
	{
		out = dearh_file(fname, k, debug);

		if ("" == out)
			return 1;

		cout << "Output name of the dearchived file: " << out << endl;
	}
	
	return 0;
}


string arh_file(string fname, int k, bool debug)
{
	FILE* f = fopen(fname.c_str(), "rb");
	if (!f)
	{
		cout << fname << " not found!" << endl;
		return "";
	}

	cout << "Archiving RiceEncoding: " << fname << endl;
	string out = fname + ".arh";
	FILE* arh = fopen(out.c_str(), "wb");

	int b = (int)pow(2, k);

	if (debug)
		cout << "b:" << b << endl;


	char byte[1];
	int n, q, r;
	int j = 0;
	int bits = 0;
	string data = "";

	while (fread(byte, 1, 1, f))
	{
		bitset<8> x(*byte);

		n = (int)(x.to_ulong());



		q = (n - 0) / b;
		r = (n - 0) % b;

		// q
		for (int i = 0; i < q; i++)
			data += "1";
		data += "0";

		// r

		data += bitset<8>(r).to_string().substr(8 - k, -k);

		if (debug)
			cout << j << "." << x << " - " << n << " q = " << q << " r = " << r << endl;

		int i = 0;
		while (data.size() >= 8)
		{
			x = bitset<8>(data.substr(0, 8));
			n = (int)(x.to_ulong());
			fwrite(reinterpret_cast<const char*>(&n), 1, 1, arh);
			data = data.substr(8, data.size() - 8);
			bits += 8;
		}

		j++;
	}

	// end zerro

	if (data.size()) {

		while (data.size() < 8)
			data += "1";

		bitset<8> x = bitset<8>(data.substr(0, 8));
		n = (int)(x.to_ulong());
		fwrite(reinterpret_cast<const char*>(&n), 1, 1, arh);
		bits += 8;

	}

	int size = j * 8;
	cout << "result(bits): " << size << " -> " << bits << endl;
	cout << "compression ratio: " << (float)size / (float)bits << endl;

	fclose(arh);
	fclose(f);
	return out;
}

string dearh_file(string fname, int k, bool debug)
{
	FILE* f = fopen(fname.c_str(), "rb");
	if (!f)
	{
		cout << fname << " not found!" << endl;
		return "";
	}

	cout << "Dearchiving: " << fname << endl;
	string out = fname + ".dearh";
	FILE* dearh = fopen(out.c_str(), "wb");

	char byte[1];

	string data = "";
	
	int b = (int)pow(2, k);

	bool wait_q = true;
	bool wait_r = false;
	int q = 0, n, r;
	int j = 0;
	int bytes = 0;
	while (!feof(f) || data.size() >= k)
	{
		if (fread(byte, 1, 1, f) == 1)
		{
			bitset<8> x(*byte);
			if (debug)
				cout << " add " << x.to_string() << endl;
			data += x.to_string();
		}

		if (wait_q)
		{
			if (!data.size()) {
				wait_q = false;
			}

			while (data.size() > 0 && data[0] == '1') {
				q++;
				data = data.substr(1, data.size() - 1);
			}
			
			if (data.size())
			{
				data = data.substr(1, data.size() - 1);
				wait_q = false;
				wait_r = true;
			} 
		}

		if (wait_r)
		{
			if (data.size() >= k)
			{
				
				r = (int)bitset<8>(data.substr(0, k)).to_ulong();
				n = q * b + r + 0;
				if (debug) 
					cout << bytes << "." << " n = " << n << " q = " << q << " r = " << r << endl;

				fwrite(reinterpret_cast<const char*>(&n), 1, 1, dearh);
				bytes++;
				
				
				data = data.substr(k, data.size() - k);
				wait_r = false;
				wait_q = true;
				q = 0;
			}

		}
		j++;
	}
	fclose(f);
	fclose(dearh);

	return out;
}