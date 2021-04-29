#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

unsigned char binaryToChar(string binary);											//2진수를 ascii코드로 변환
void readData(string& data, string& table);											//데이터를 읽어오는 함수
void decode(string& txtData, vector<string>& table);								//디코딩울 진행하는 함수 
void processData(string& txtBinary, string& txtData);								//이진 데이터를 처리해서 문자열로 바꾸는 함수
void processTable(string& tableBinary, string& tableData, vector<string>& table);	//이진 테이블 데이터를 가져와서 테이블을 생성 



int main() {

	string txtData, txtBinary;				//텍스트 이진 데이터, 이진수 데이터
	string tableData, tableBinary;			//테이블 이진 데이터, 이진수 데이터

	vector<string> table(128);				//호프만 테이블 

	readData(txtBinary, tableBinary);
	processTable(tableBinary, tableData, table);
	processData(txtBinary, txtData);
	decode(txtData, table);
}
void readData(string& text, string& table) {
	ifstream tableFile("table.hbs", ios::in | ios::binary);
	ifstream dataFile("huffman_code.hbs", ios::in | ios::binary);
	
	if (dataFile.is_open()) {
		text.resize(3631);
		dataFile.read(&text[0], 3631);
	}
	if (tableFile.is_open()) {
		table.resize(192);
		tableFile.read(&table[0], 192);
	}
	dataFile.close();
	tableFile.close();
	
	return;
}

void processData(string& txtBinary, string& txtData) {
	for (int i = 0; i < 3631; i++) {
		for (int j = 7; j >= 0; j--) {
			if ((txtBinary[i] >> j) & 1) txtData += '1';
			else txtData += '0';
		}
	}
}

void decode(string& txtData, vector<string>& table) {
	ofstream decodeFile("output.txt", ios::out);
	string originalText;
	string code;
	int idx = 0;
	int flag = 0;
	while (true){
		 code.resize(0);
		 for (int i = 0; i < 13; i++, idx++) {  //코드의 최대 길이인 13으로 설정(호프만 테이블에서 최대길이를 구해올 수도 있음)
			 code += txtData[idx];
			 if (code == table[3]) {
				 break;
			 }
			 else {
				 for (int j = 0; j < 128; j++) { // 테이블에 있는 값을 찾아서 원래 데이터로 복호화 
					 if (code == table[j]) {
						 originalText += (char)j;
						 flag = 1;
					 }
				 }
			 }
			 if (flag) {
				 flag = 0;
				 idx++;
				 break;
			 }
		 }
		 if (code == table[3]) { //만약 EOD 코드(1011001111110) 를 만날 경우 종료
			 break;
		 }
	 }
	decodeFile << originalText;
	decodeFile.close();
}

void processTable(string& tableBinary, string& tableData, vector<string>& table) {
	tableData;
	for (int i = 0; i < 192; i++) {
		for (int j = 7; j >= 0; j--) {
			if ((tableBinary[i] >> j) & 1) tableData += '1'; // 비트 단위로 읽어서 비트 문자열로 변환 
			else tableData += '0';
		}
	}
	for (int i = 0; i < tableData.length();) { 
		int len, Ascii;
		string ascii;
		string clen;
		string code;

		for (int j = 0; j < 8; i++, j++) {  // ascii 데이터 read
			if (i < tableData.length())
				ascii += tableData[i];
		}
		for (int j = 0; j < 8; i++, j++) {  // 코드 길이 데이터 read
			if (i < tableData.length())
				clen += tableData[i];
		}
		Ascii = binaryToChar(ascii);
		len = binaryToChar(clen);
		for (int j = 0; j < len; i++, j++) { // 코드 데이터 read
			if (i < tableData.length()) code += tableData[i];
		}
		
		if (i < tableData.length()) table[Ascii] = code;
		else break;
	}
}
unsigned char binaryToChar(string binary) { // 2진수를 문자열로 변환 
	unsigned char result = 0;
	for (int i = 0; i < binary.length(); i++) {
		if (binary[i] == '1') result += (int)pow(2, binary.length() - i - 1);
	}
	return result;
}