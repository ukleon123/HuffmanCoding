#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

unsigned char binaryToChar(string binary);											//2������ ascii�ڵ�� ��ȯ
void readData(string& data, string& table);											//�����͸� �о���� �Լ�
void decode(string& txtData, vector<string>& table);								//���ڵ��� �����ϴ� �Լ� 
void processData(string& txtBinary, string& txtData);								//���� �����͸� ó���ؼ� ���ڿ��� �ٲٴ� �Լ�
void processTable(string& tableBinary, string& tableData, vector<string>& table);	//���� ���̺� �����͸� �����ͼ� ���̺��� ���� 



int main() {

	string txtData, txtBinary;				//�ؽ�Ʈ ���� ������, ������ ������
	string tableData, tableBinary;			//���̺� ���� ������, ������ ������

	vector<string> table(128);				//ȣ���� ���̺� 

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
		 for (int i = 0; i < 13; i++, idx++) {  //�ڵ��� �ִ� ������ 13���� ����(ȣ���� ���̺��� �ִ���̸� ���ؿ� ���� ����)
			 code += txtData[idx];
			 if (code == table[3]) {
				 break;
			 }
			 else {
				 for (int j = 0; j < 128; j++) { // ���̺� �ִ� ���� ã�Ƽ� ���� �����ͷ� ��ȣȭ 
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
		 if (code == table[3]) { //���� EOD �ڵ�(1011001111110) �� ���� ��� ����
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
			if ((tableBinary[i] >> j) & 1) tableData += '1'; // ��Ʈ ������ �о ��Ʈ ���ڿ��� ��ȯ 
			else tableData += '0';
		}
	}
	for (int i = 0; i < tableData.length();) { 
		int len, Ascii;
		string ascii;
		string clen;
		string code;

		for (int j = 0; j < 8; i++, j++) {  // ascii ������ read
			if (i < tableData.length())
				ascii += tableData[i];
		}
		for (int j = 0; j < 8; i++, j++) {  // �ڵ� ���� ������ read
			if (i < tableData.length())
				clen += tableData[i];
		}
		Ascii = binaryToChar(ascii);
		len = binaryToChar(clen);
		for (int j = 0; j < len; i++, j++) { // �ڵ� ������ read
			if (i < tableData.length()) code += tableData[i];
		}
		
		if (i < tableData.length()) table[Ascii] = code;
		else break;
	}
}
unsigned char binaryToChar(string binary) { // 2������ ���ڿ��� ��ȯ 
	unsigned char result = 0;
	for (int i = 0; i < binary.length(); i++) {
		if (binary[i] == '1') result += (int)pow(2, binary.length() - i - 1);
	}
	return result;
}