#include<algorithm>
#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

class bitstream { //비트 단위로 출력하기 위한 클래스
	std::streambuf* sbuf;
	int             count = 0;
	unsigned char   byte = 0;
public:
	bitstream(std::ostream& out) : sbuf(out.rdbuf()) {}
	~bitstream() {
		if (this->count) {
			this->sbuf->sputc(this->byte);
		}
	}
	bitstream& operator<< (bool b) { //연산자 오버라이딩을 통해 버퍼에 비트 조작
		this->byte = (this->byte << 1) | b;
		if (++this->count == 8) {
			this->sbuf->sputc(this->byte);
			this->count = 0;
		}
		return *this;
	}
};


typedef struct data{
	int amount = 0;
	unsigned int data;
}Data;

typedef struct node{
	bool bit;
	Data data;
	struct node* parent;
	struct node* child[2];
}Node;

bool compare(Node* node1, Node* node2);							// 데이터의 양을 기준으로 정렬을 위한 비교함수
void encode(vector<string> table);								// 인코딩을 진행하는 함수
void saveTable(vector<string> table);							// 호프만 테이블을 이진 파일로 저장하는 함수 
void dataProcess(vector<Node*>& data);							// 입력 파일의 데이터를 처리하는 함수
void createTable(vector<Node*>& trace, vector<string>& table);	// 호프만 테이블을 생성하는 함수

int main(){
	Node Eod;
	vector<Node*> trace(128);			//데이터에 해당하는 코드를 쉽게 찾기위한 배열
	vector<Node*> dataSet(128);			//데이터 저장용 배열
	vector<string> table(128);			//호프만 테이블
	
	for (int i = 0; i < 128; i++) dataSet[i] = new Node;
	dataProcess(dataSet);
	dataSet[3]->data.data = 3, dataSet[3]->data.amount = 1;
	sort(dataSet.begin(), dataSet.end(), compare);

	vector<Node*>::iterator iter = dataSet.begin();
	dataSet.erase(iter, iter + 65), iter = dataSet.begin();
	
	for (int i = 0; i < 63; i++) { 
		trace[dataSet[i]->data.data] = dataSet[i]; 
	}
	Node* tmp;
	while(dataSet.size() != 1) { //호프만 트리 작성
		tmp = new Node;

		tmp->bit = 0;
		tmp->data.data = 0;
		tmp->parent = NULL;

		dataSet[0]->bit = 0;
		dataSet[1]->bit = 1;
		tmp->child[0] = dataSet[0];
		tmp->child[1] = dataSet[1];
		tmp->data.amount = dataSet[0]->data.amount + dataSet[1]->data.amount;

		dataSet[0]->parent = tmp;
		dataSet[1]->parent = tmp;

		dataSet.erase(iter, iter + 2), iter = dataSet.begin();
		dataSet.insert(iter, tmp), iter = dataSet.begin();
		sort(dataSet.begin(), dataSet.end(), compare);
	}
	createTable(trace, table);
	
	for (int i = 0; i < 128; i++) { // 테이블의 값 출력 
		cout << (char)i << '\t' << table[i] << endl;
	}
	saveTable(table);
	encode(table);
}

bool compare(Node* node1, Node* node2){
	return node1->data.amount < node2->data.amount;
}
void dataProcess(vector<Node*> &data) {
	ifstream txtFile("input_data.txt");
	string txtdata;
	if (txtFile.is_open()) {
		txtdata.resize(6433);
		txtFile.read(&txtdata[0], 6433);
	}
	for (int i = 0; i < 6433; i++) {
		data[txtdata[i]]->data.data = txtdata[i]; // 문자각각의 아스키 코드 값과 총량을 저장 
		data[txtdata[i]]->data.amount++;
	}
	txtFile.close();
	return;
}
void createTable(vector<Node*> &trace, vector<string> &table) {
	string code;
	Node* current = NULL;

	for (int i = 0; i < trace.size(); i++) {
		code = "";
		current = trace[i];
		while (current&&current->parent) {
			if (current->bit) {
				code = "1" + code;
			}
			else {
				code = "0" + code;
			}
			current = current->parent;
		}
		table[i] = code;
	}
}


void saveTable(vector<string> table) {
	int check = 0;
	ofstream tableFile("table.hbs",ios::out | ios::binary );
	bitstream bitFile(tableFile);

	for (int i = 0; i < 128; i++) {
		if (table[i].length() != 0) {
			for (int j = 7; j >= 0; --j) {// 8비트 단위로 입력(ascii 데이터)
				int word = i >> j & 1;
				bitFile << word;
				check++;
			}
			for (int j = 7; j >= 0; --j) {// 8비트 단위로 입력(코드 길이 데이터)
				int word = table[i].length() >> j & 1;
				bitFile << word;
				check++;
			}
			for (int j = 0; j < table[i].length(); j++) { // 코드를 비트로 입력
				if (table[i][j] == '1') bitFile << true;
				else bitFile << false;
				check++;
			}
		}
		
	}
	if (check % 8) {						//byte align 을 위한 stuffing bit 추가
		for (int i = 0; i < 8 - check % 8; i++) bitFile << false;
	}
	tableFile.close();
	return;
}
void encode(vector<string> table) {
	int check = 0;
	ifstream txtFile("input_data.txt");
	ofstream encodeFile("huffman_code.hbs",ios::out | ios::binary);
	bitstream encodeBit(encodeFile);
	string txtdata;
	if (txtFile.is_open()) {
		txtdata.resize(6433);
		txtFile.read(&txtdata[0], 6433);
	}
	for (int i = 0; i < 6434; i++) {				// 각 문자에 해당하는 코드를 비트로 입력
		for (int j = 0; j < table[txtdata[i]].length(); j++) {
			if (table[txtdata[i]][j] == '1')encodeBit << true;
			else encodeBit << false;
			check++;
		}
	}
	for (int i = 0; i < table[3].length(); i++) {	// EOD 코드 입력
		if (table[3][i] == '1')encodeBit << true;
		else encodeBit << false;
		check++;
	}
	if (check % 8) {								//byte align 을 위한 stuffing bit 추가
		for (int i = 0; i < 8 - check % 8; i++) encodeBit << false;
	}
	txtFile.close();
	return;
}