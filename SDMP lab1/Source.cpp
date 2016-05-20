

#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <map>
#include <cmath>
using namespace std::rel_ops;
typedef std::string     string_t;
typedef std::ifstream   ifstream_t;
const string_t unsorted_txt = "D:\\unsorted.txt";
const string_t sorted_txt = "D:\\sorted.txt";
const string_t repeated_txt = "D:\\repeated.txt";
const int NMAX = 200;




// Родительский класс Элемент
class Element {
public:
	virtual void scanFile(std::istream&) = 0;

protected:
	void error(string_t mess) {
		std::cerr << mess << std::endl;
		std::cout << "Press any key to exit." << std::endl;
		getchar();
		exit(1);
	}
};

// Производный класс Студент
class Countries : public Element {
protected:
	int ID;
	string_t country;
	string_t symbol;
	string_t money;
	string_t city;

public:
	Countries()
		: ID(0), country(""), symbol(""), money(""), city("")
	{}

	Countries(int _ID, string_t _country, string_t _symbol, string_t _money, string_t _city)
		: ID(_ID), country(_country), symbol(_symbol), money(_money), city(_city)
	{}

	virtual void scanFile(std::istream& fin) {
		fin >> ID >> country >> symbol >> money >> city;
	}

	const int       getID() const       { return ID; }
	const string_t  getCountry() const  { return country; }
	const string_t  getSymbol() const   { return symbol; }
	const string_t  getMoney() const    { return money; }
	const string_t  getCity() const     { return city; }

	bool operator==(const Countries& s) const { return ID == s.ID; }
	bool operator <(const Countries& s) const { return ID < s.ID; }

	~Countries() {}
};

// Перегрузка оператора << для вывода Студента
std::ostream& operator<<(std::ostream& os, const Countries& s) {
	os << s.getID() << "\t" + s.getCountry() + "\t\t" +
		s.getSymbol() + "\t\t" + s.getMoney() + "\t" +
		s.getCity() + "\n";
	return os;
}

// Описание элемента из неупорядоченной древовидной таблицы
class TreeLike : public Countries {
protected:
	int less;
	int more;

public:
	TreeLike() : Countries(), less(-1), more(-1) {}

	TreeLike(int _ID, string_t _name, string_t _symbol, string_t _money, string_t _city)
		: Countries(_ID, _name, _symbol, _money, _city), less(-1), more(-1)
	{}

	const int getLess() const { return less; }
	const int getMore() const { return more; }
	int setLess(int _less) { return less = _less; }
	int setMore(int _more) { return more = _more; }

	virtual void treeShow() {
		std::cout << this->getID() << "["
			<< less << ","
			<< more << "]" <<
			std::endl;
	}

	virtual void scanFile(std::istream& fin) {
		less = more = -1;
		Countries::scanFile(fin);
	}

	~TreeLike() {}
};

// Описание элемента из упорядоченной хэш таблицы
class HashCountries : public Countries {
public:
	HashCountries() : Countries() {}

	HashCountries(int _ID, string_t _country, string_t _symbol, string_t _money, string_t _city)
		: Countries(_ID, _country, _symbol, _money, _city)
	{}

	int hashFunc(int n) { return ID % n; }
};




// Класс структуры данных
class DataStruct {
protected:
	ifstream_t fin;
	long ncomp;

public:
	DataStruct() {
		fin.open(unsorted_txt, std::ios::in);
		if (!fin.is_open())
			error("File " + unsorted_txt + " not found!\n");
		ncomp = 0;
	}

	DataStruct(string_t filename) {
		fin.open(filename, std::ios::in);
		if (!fin.is_open())
			error("File " + filename + " not found!\n");
		ncomp = 0;
	}

	const long getNcomp() const { return ncomp; }

	void resetNcomp() { ncomp = 0; }

	~DataStruct() {
		if (fin.is_open())
			fin.close();
		fin.clear();
	}

protected:
	void error(string_t mess) {
		std::cerr << mess << std::endl;
		std::cout << "Press any key to exit." << std::endl;
		getchar();
		exit(1);
	}
};

// Шаблонный класс неупорядоченной таблицы
template <class E>
class Table : public DataStruct {
protected:
	int n;          // Количество записей в таблице
	E row[NMAX];    // Вектор записей таблицы

public:
	Table() : n(0) {}

	Table(string_t filename) : DataStruct(filename) {
		n = 0;
		while (!fin.eof()) {
			row[n].scanFile(fin);
			++n;
		}
	}

	virtual void show() {
		for (int i = 0; i < n; ++i)
			std::cout << row[i];
	}

	int search(E e) {
		int position = -1;
		for (int i = 0; (position == -1) && i < n; ++i) {
			++ncomp;
			if (e == row[i]) position = i;
		}
		return position;
	}

	const int getN() const { return n; }

	~Table() {}
};

// Шаблонный класс неупорядоченной древовидной таблицы
template <class E>
class TreeTable : public Table<E> {
	using Table<E>::n;
	using Table<E>::row;
	using Table<E>::ncomp;
public:
	TreeTable() { n = 0; }

	TreeTable(string_t filename) : Table<E>(filename) {
		for (int i = 1; i < n; ++i) {
			int forward = 1;
			int j = 0;

			while (forward) {
				if (row[i] < row[j]) {
					if (row[j].getLess() == -1)
						row[j].setLess(row[i].getID()),
						forward = 0;
					else
						for (int k = 0; k < n; ++k)
							if (row[k].getID() == row[j].getLess()) {
								j = k;
								break;
							}
				}
				else {
					if (row[i] > row[j]) {
						if (row[j].getMore() == -1)
							row[j].setMore(row[i].getID()),
							forward = 0;
						else
							for (int k = 0; k < n; k++)
								if (row[k].getID() == row[j].getMore()) {
									j = k;
									break;
								}
					}
				}
			}
		}
	}

	virtual void treeShow() {
		for (int i = 0; i < n; ++i)
			row[i].treeShow();
	}

	int search(E e) {
		int position = -1,
			forward = 1,
			i = 0;

		while (forward) {
			if (e == row[i])
				position = i, forward = 0;
			else {
				if (e < row[i]) {
					for (int k = 0; k < n; ++k)
						if (row[k].getID() == row[i].getLess()) {
							i = k;
							break;
						}
				}
				else {
					for (int k = 0; k < n; ++k)
						if (row[k].getID() == row[i].getMore()) {
							i = k;
							break;
						}
				}
				if (i == -1)
					forward = 0;
			}
			++ncomp;
		}
		return position;
	}

	~TreeTable() {}

};

// Шаблонный класс упорядоченной таблицы для бинарного поиска
template <class E>
class SortedTable : public Table<E> {
	using Table<E>::n;
	using Table<E>::row;
	using Table<E>::ncomp;
public:
	SortedTable() {}

	SortedTable(string_t filename) : Table<E>(filename) {}

	int search(E e) {
		int a = 0, b = n - 1;

		while (a < b) {
			int i = (a + b) / 2;
			ncomp++;
			if (e > row[i])
				a = i + 1;
			else
				if (e < row[i])
					b = i;
				else
					a = b = i;
		}
		return (ncomp++, e == row[a]) ? a : -1;
	}
};

// Шаблонный класс упорядоченной таблицы для поиска Фибоначчи
template <class E>
class FiboTable : public Table<E> {
	using Table<E>::n;
	using Table<E>::row;
	using Table<E>::ncomp;
public:
	FiboTable() {}

	FiboTable(string_t filename) : Table<E>(filename) {}

	int min(int x, int y) { return (x <= y) ? x : y; }

	int search(E e) {
		const int fibo[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144 };
		int pos, index = 0, k = 0;
		while (n > fibo[k]) k++;

		while (k > 0) {
			++ncomp;
			pos = index + fibo[--k];
			if (pos >= n || e < row[pos]);
			else if (e == row[pos]) return pos;
			else {
				index = pos;
				k--;
			}
		}
		return -1;
	}
};

// Шаблонный класс поиска в хэш-таблице
template <class E>
class HashTable : public Table<E> {
	using Table<E>::n;
	using Table<E>::row;
	using Table<E>::ncomp;
protected:
	int m;
	std::map<int, std::pair<int, E>> hash_map;

public:
	HashTable(string_t filename) : Table<E>(filename) {
		for (int i = 0; i < n; ++i)
			hash_map.insert(std::make_pair(row[i].getID(),
			std::make_pair(i, row[i])));

		m = hash_map.size();
	}

	int search(E e) {
		int position = -1;
		int i = e.hashFunc(n);
		while (position == -1) {
			++ncomp;
			if (e == hash_map[i].second)
				position = i;
			else
				i = (i + 1) % n;
		}
		return hash_map[position].first;
	}
	const int getM() const { return m; }
};




void Menu();
void linearSearch(string_t);
void unsortedTreeSearch(string_t);
void binarySearch(string_t);
void fiboSearch(string_t);
void hashSearch(string_t);




int main() {
	Menu();
	return 0;
}




void Menu() {
	int choise = -1;

	std::cout << "1 - Linear search." << std::endl;
	std::cout << "2 - Unsorted tree table search." << std::endl;
	std::cout << "3 - Binary search." << std::endl;
	std::cout << "4 - Fibbonacci search." << std::endl;
	std::cout << "5 - Hashing Table search." << std::endl;
	std::cout << "0 - Exit." << std::endl;

	while (choise != 0) {
		std::cout << std::endl << "> ";
		std::cin >> choise;

		switch (choise) {
		case 1:
			linearSearch(unsorted_txt);
			break;

		case 2:
			unsortedTreeSearch(unsorted_txt);
			break;

		case 3:
			binarySearch(sorted_txt);
			break;

		case 4:
			fiboSearch(sorted_txt);
			break;

		case 5:
			hashSearch(repeated_txt);

		case 0:
			break;

		default:
			std::cout << "Enter the number between 0-2!" << std::endl;
			break;
		}
	}
	std::cout << "Good bye!" << std::endl;
}

void linearSearch(string_t filename) {
	// Реализация линейного поиска
	std::cout << "Loading data..." << std::endl;
	Table<Countries> S(filename);
	std::cout << "Complete!" << std::endl;
	S.show();

	char ch = 'n';
	while (ch != 'y') {
		int step = 0, id = 0;

		std::cout << "Enter id to search: ";
		std::cin >> id;
		if (id > 0) {
			Countries e(id, "", "", "", ""); S.resetNcomp();

			step = S.search(e);
			if (step < 0)
				std::cout << "Not in table! " << "The number of comparisons: "
				<< S.getNcomp() << std::endl;
			else
				std::cout << "Found on the position " << step + 1
				<< " in " << S.getNcomp() << " steps!" << std::endl;
		}
		std::cout << "Done ? (y/n) ";
		std::cin >> ch;
		std::cout << std::endl;
	}

	// Подсчёт средней длины поиска
	Countries sample;
	long NCOMP = 0;
	ifstream_t fin;
	fin.open(filename, std::ios::in);

	while (!fin.eof()) {
		sample.scanFile(fin);
		S.resetNcomp();
		if (S.search(sample) >= 0)
			NCOMP += S.getNcomp();
	}
	fin.close();
	fin.clear();

	std::cout << "N = " << S.getN() << ", NCOMP = " << NCOMP
		<< ", ALS = " << ((double)NCOMP / S.getN());
}

void unsortedTreeSearch(string_t filename) {
	// Реализация поиска в неупорядоченной древовидной таблице
	std::cout << "Loading data..." << std::endl;
	TreeTable<TreeLike> S(filename);
	std::cout << "Complete!" << std::endl;
	S.treeShow();

	char ch = 'n';
	while (ch != 'y') {
		int step, id = 0;

		std::cout << "Enter id to search: ";
		std::cin >> id;
		TreeLike t(id, "", "", "", ""); S.resetNcomp();

		step = S.search(t);
		if (step < 0)
			std::cout << "Not in table! " << "The number of comparisons: "
			<< S.getNcomp() << std::endl;
		else
			std::cout << "Found on the position " << step + 1
			<< " in " << S.getNcomp() << " steps!" << std::endl;
		std::cout << "Done ? (y/n) ";
		std::cin >> ch;
		std::cout << std::endl;
	}

	// Подсчёт средней длины поиска
	TreeLike sample;
	long NCOMP = 0;
	ifstream_t fin;
	fin.open(filename, std::ios::in);

	while (!fin.eof()) {
		sample.scanFile(fin);
		S.resetNcomp();
		if (S.search(sample) >= 0)
			NCOMP += S.getNcomp();
	}
	fin.close();
	fin.clear();

	std::cout << "N = " << S.getN() << ", NCOMP = " << NCOMP;
	std::cout << ", ALS = " << ((double)NCOMP / S.getN());
	std::cout << ", MAX = " << (S.getN() / 2.0);
	std::cout << ", MIN = " << (log((double)S.getN()) / log(2.0) + 2.0);
}

void binarySearch(string_t filename) {
	// Реализация бинарного поиска
	std::cout << "Loading data..." << std::endl;
	SortedTable<Countries> S(filename);
	std::cout << "Complete!" << std::endl;
	S.show();

	char ch = 'n';
	while (ch != 'y') {
		int step, id = 0;

		std::cout << "Enter id to search: ";
		std::cin >> id;
		Countries e(id, "", "", "", ""); S.resetNcomp();

		step = S.search(e);
		if (step < 0)
			std::cout << "No table!" << "The number of comparisons: "
			<< S.getNcomp() << std::endl;
		else
			std::cout << "Found on the position " << step + 1
			<< " in " << S.getNcomp() << " steps!" << std::endl;

		std::cout << "Done ? (y/n) ";
		std::cin >> ch;
		std::cout << std::endl;
	}

	// Подсчёт средней длины поиска
	Countries sample;
	double NCOMP = 0;
	ifstream_t fin;
	fin.open(filename, std::ios::in);

	while (!fin.eof()) {
		sample.scanFile(fin);
		S.resetNcomp();
		if (S.search(sample) >= 0)
			NCOMP += S.getNcomp();
	}
	fin.close();
	fin.clear();

	std::cout << "N = " << S.getN() << ", NCOMP = " << NCOMP;
	std::cout << ", ALS = " << (double)NCOMP / S.getN();
	std::cout << ", ALS_TEOR = " << (log((double)S.getN()) / log(2.0) + 2.0);
}

void fiboSearch(string_t filename) {
	// Реализация поиска Фибоначчи
	std::cout << "Loading data..." << std::endl;
	FiboTable<Countries> S(filename);
	std::cout << "Complete!" << std::endl;
	S.show();

	char ch = 'n';
	while (ch != 'y') {
		int step, id = 0;

		std::cout << "Enter id to search: ";
		std::cin >> id;
		Countries e(id, "", "", "", ""); S.resetNcomp();

		step = S.search(e);
		if (step < 0)
			std::cout << "No table!" << "The number of comparisons: "
			<< S.getNcomp() << std::endl;
		else
			std::cout << "Found on the position " << step + 1
			<< " in " << S.getNcomp() << " steps!" << std::endl;

		std::cout << "Done ? (y/n) ";
		std::cin >> ch;
		std::cout << std::endl;
	}

	// Подсчёт средней длины поиска
	Countries sample;
	double NCOMP = 0;
	ifstream_t fin;
	fin.open(filename, std::ios::in);

	while (!fin.eof()) {
		sample.scanFile(fin);
		S.resetNcomp();
		if (S.search(sample) >= 0)
			NCOMP += S.getNcomp();
	}
	fin.close();
	fin.clear();

	std::cout << "N = " << S.getN() << ", NCOMP = " << NCOMP;
	std::cout << ", ALS = " << (double)NCOMP / S.getN();
	std::cout << ", ALS_TEOR = " << log10(S.getN());
}

void hashSearch(string_t filename) {
	// Реализация поиска в хэш-таблице
	std::cout << "Loading data..." << std::endl;
	HashTable<HashCountries> S(filename);
	std::cout << "Complete!" << std::endl;
	S.show();

	char ch = 'n';
	while (ch != 'y') {
		int step, id = 0;

		std::cout << "Enter id to search: ";
		std::cin >> id;
		HashCountries e(id, "", "", "", ""); S.resetNcomp();

		step = S.search(e);
		if (step < 0)
			std::cout << "No table!" << "The number of comparisons: "
			<< S.getNcomp() << std::endl;
		else
			std::cout << "Found on the position " << step + 1
			<< " in " << S.getNcomp() << " steps!" << std::endl;

		std::cout << "Done ? (y/n) ";
		std::cin >> ch;
		std::cout << std::endl;
	}

	// Подсчёт средней длины поиска
	HashCountries sample;
	double NCOMP = 0;
	ifstream_t fin;
	fin.open(filename, std::ios::in);

	while (!fin.eof()) {
		sample.scanFile(fin);
		S.resetNcomp();
		if (S.search(sample) >= 0)
			NCOMP += S.getNcomp();
	}
	fin.close();
	fin.clear();

	std::cout << "M = " << S.getM() << ", NCOMP = " << NCOMP;
	std::cout << ", ALS = " << ((double)NCOMP / S.getM());
	double sigma = (double)S.getM() / (double)S.getN();
	std::cout << ", m/n = " << sigma;
	std::cout << ", D(m/n) = " << ((2.0 - sigma) / (2.0 - 2.0*sigma));
}