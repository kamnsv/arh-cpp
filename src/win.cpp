#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 1024
#define MD5LEN  16

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <bitset>
#include <ctime> 
#include <windows.h>
#include <Wincrypt.h>

using namespace std;

string arh_file(string fname, int k, bool debug);
string dearh_file(string fname, int k, bool debug);
bool create_rndbin(string fname, int bytes, bool debug);
DWORD print_md5file(string fname);

int main(int argc, char* argv[])
{

	setlocale(LC_ALL, "en");

	if (argc < 3)
	{
		cout << "Archiving with Rice encoding." << endl << endl;
		cout << "Use [FILENAME] [ACTION] [OPTIONS]." << endl;

		cout << endl << "\tFILENAME: The path to the archived file or the name for generating a new one," << endl;
		cout << "\t\t  if the '--n' option is specified with the number of bytes" << endl;
		
		cout << endl << "\tACTIONS:" << endl << endl;
		cout << "\t--arh   \t Archive file, output archive with '.arh' extension." << endl;
		cout << "\t--dearh \t Dearchive the file, the output is the original file, but with the extension '.dearh'." << endl;

		cout << endl << "\tOPTIONS:" << endl << endl;
		cout << "\t--n     \t Creates a random binary file of the given length [n] wtis name [FILENAME]." << endl;
		cout << "\t--k     \t Parameter [1;8] for encoding Rice, default 6." << endl;
		cout << "\t--debug \t Optional flag to display information about bytes." << endl;
		
		return 0;
	}

	string fname(argv[1]);
	int n = 0;
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

			if ("--n" == arg)
				n = atoi(argv[i + 1]);
		}
		catch (exception& e) {
			cout << "Please enter the number of bytes following the '--n' option:" << e.what() << endl;
			return 1;
		}

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

	if (n > 0)
	{

		if (!create_rndbin(fname, n, debug))
			return 1;
		else 
			cout << "Random binary generated: " << fname << endl;
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


DWORD print_md5file(string fname) {
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = NULL;
	BYTE rgbFile[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5LEN];
	DWORD cbHash = 0;
	CHAR rgbDigits[] = "0123456789abcdef";
	std::wstring stemp = std::wstring(fname.begin(), fname.end());
	LPCWSTR filename = stemp.c_str();
	// Logic to check usage goes here.

	hFile = CreateFile(filename,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		dwStatus = GetLastError();
		printf("Error opening file %s\nError: %d\n", filename,
			dwStatus);
		return dwStatus;
	}

	// Get handle to the crypto provider
	if (!CryptAcquireContext(&hProv,
		NULL,
		NULL,
		PROV_RSA_FULL,
		CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		CloseHandle(hFile);
		return dwStatus;
	}

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		CloseHandle(hFile);
		CryptReleaseContext(hProv, 0);
		return dwStatus;
	}

	while (bResult = ReadFile(hFile, rgbFile, BUFSIZE,
		&cbRead, NULL))
	{
		if (0 == cbRead)
		{
			break;
		}

		if (!CryptHashData(hHash, rgbFile, cbRead, 0))
		{
			dwStatus = GetLastError();
			printf("CryptHashData failed: %d\n", dwStatus);
			CryptReleaseContext(hProv, 0);
			CryptDestroyHash(hHash);
			CloseHandle(hFile);
			return dwStatus;
		}
	}

	if (!bResult)
	{
		dwStatus = GetLastError();
		printf("ReadFile failed: %d\n", dwStatus);
		CryptReleaseContext(hProv, 0);
		CryptDestroyHash(hHash);
		CloseHandle(hFile);
		return dwStatus;
	}

	cbHash = MD5LEN;
	if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
	{
		printf("MD5 hash of file is: ");
		for (DWORD i = 0; i < cbHash; i++)
		{
			printf("%c%c", rgbDigits[rgbHash[i] >> 4],
				rgbDigits[rgbHash[i] & 0xf]);
		}
		printf("\n");
	}
	else
	{
		dwStatus = GetLastError();
		printf("CryptGetHashParam failed: %d\n", dwStatus);
	}

	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	CloseHandle(hFile);

	return dwStatus;
}

bool create_rndbin(string fname, int bytes, bool debug)
{

	FILE* f = fopen(fname.c_str(), "w+");
	if (!f)
	{
		cout << fname << " not open!" << endl;
		return false;
	}
	int n;
	srand(time(0));
	for (int i = 0; i < bytes; i++) {
		
		n = rand() % 100;
		if (n < 50) n = rand() % 64;
		else if (n < 80) n = rand() % 128;
		else if (n < 95) n = rand() % 192;
		else n = rand() % 256;

		if (debug)
			cout << i << ". " << n << endl;

		fwrite(reinterpret_cast<const char*>(&n), 1, 1, f);
	}
	fclose(f);
	return true;
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
	print_md5file(fname.c_str());
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
	print_md5file(out.c_str());
	return out;
}