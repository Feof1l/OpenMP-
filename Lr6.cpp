// Lr6.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//Разрабоnать программу вычисления произведения значения минимальных элементов строк 
//матрицы большой размерности в 3 модификациях
// a) Без использования reduction
// b) С использованием reduction
// c) С использованием конструкции section
// Для каждого пункта разное кол-во ядер, разные размерности матриц
#include <iostream>
#include <ctime>
#include <fstream>
#include <functional>
#include <omp.h>
#include <random>
#include <windows.h>
using namespace std;


class Rand_double
{
public:
    Rand_double(double low, double high)
        :r(std::bind(std::uniform_real_distribution<>(low, high), std::default_random_engine())) {}

    double operator()() { return r(); }

private:
    std::function<double()> r;
};
double FindMinimums(double** matrix, int n, int m) {
    auto start = omp_get_wtime();
    int i, j;
    double minn = 1;
    double otvet = 1;
    double* masMin;
    masMin = new double[n];
    for (i = 0; i < n; i++) {
        minn = matrix[i][0];
        for (j = 0; j < m; j++) {
            if (matrix[i][j] < minn) {
                minn = matrix[i][j];

            }
        }
        masMin[i] = minn;
        //otvet *= minn;
    }
    for (i = 1; i < n; i++) {
       // cout << masMin[i] << " ";
        otvet *= masMin[i];
    }
    auto end = omp_get_wtime();
    double time = end - start;
    cout << "Закончил последовательный режим.Прошедшее время " << time;
    cout << endl;
    return otvet;

}
double FindMinimumsSection(double** matrix, int n, int m,int count) {
    auto start = omp_get_wtime();
    int i, j;
    double minn = 1;
    double otvet = 1;
    double* masMin;
    masMin = new double[n];
    cout << endl;
    #pragma omp parallel shared(matrix, n, m) private(i, j) num_threads(count)
    {
#pragma omp sections
        {
        for (i = 0; i < n; i++) {
            minn = matrix[i][0];
            for (j = 0; j < m; j++) {
                if (matrix[i][j] < minn) {
                    minn = matrix[i][j];

                }
            }
            masMin[i] = minn;
            //otvet *= minn;
        }}
    #pragma omp sections
        {
            for (i = 1; i < n; i++) {
                // cout << masMin[i] << " ";
                otvet *= masMin[i];
            }
        }
  
    }
    auto end = omp_get_wtime();
    double time = end - start;
    cout << "Закончил Section.Прошедшее время " << time;
    cout << endl;
    return otvet;

}
double FindMinimumsParal(double** matrix,int n,int m,int count) {
    auto start = omp_get_wtime();
    int i, j;
    double minn = 1;
    double otvet = 1;
    double* masMin;
    masMin = new double [n];
    cout << endl;
    #pragma omp parallel for shared(matrix, n, m) private(i, j) num_threads(count)
    for (i = 0; i < n; i++){
        minn = matrix[i][0];
        for (j = 0; j < m; j++) {
            if (matrix[i][j] < minn) {
                minn = matrix[i][j];
                
            }
        }
        masMin[i] = minn;
        //otvet *= minn;
    }
#pragma omp parallel for num_threads(count) reduction(*:otvet)
    for (i = 1; i < n; i++) {
        //cout << masMin[i]<<" ";
        otvet *= masMin[i];
    }
    auto end = omp_get_wtime();
    double time = end - start;
    cout <<"Закончил Paral.Прошедшее время "<< time;
    cout << endl;
    return otvet;
            
}
double** VvodMat(string filename,int n,int m) { // считываем  матрицы с файла
    ifstream in(filename);
    double** x;
    x = new double* [n];
    for (int i = 0; i < n; i++) x[i] = new double[m];

    //Считаем матрицу из файла
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            //in >> x[i][j]; ввод с файла
            x[i][j] = 1.0001;


    in.close();//под конец закроем файла
    return x;
}
void Generator(int n,int m,string filename) { // генерируем матрицы большой размерности и записываем в файл
    srand(time(NULL)); // Инициализируем генератор случайных чисел.
    double** a = new double* [n]; // Создаем массив указателей
    for (int i = 0; i < n; i++)
    {
        a[i] = new double[m]; // Создаем элементы
    }
    Rand_double rd{ 1,1.5 };
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {

            a[i][j] = rd();
        }
    }
    ofstream text(filename);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
        {
            text << a[i][j];
            text << " ";
        }
        text << endl;
    }
    text.close();
   // cout <<"konec";
}

int main()
{
	setlocale(LC_ALL, "Rus");
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    printf("максимальное количество потоков  = %d\n", sysinfo.dwNumberOfProcessors);  
   
    int n100 = 100;
    int m100 = 100;
    int n10 = 10;
    int m10 = 10;
    int m10000 = 10000;
    int n10000 = 10000;
    int m1000 = 1000;
    int n1000 = 1000;

    //Generator(n10000, m10000, "fin_10000.txt");

    double** matrix10;
    matrix10 = VvodMat("fin_10.txt", n10, m10);
    cout << "\nРазмерность матрицы 10\n";
    for (int i = 1; i < 5; i++) {
        cout << "\nколичество ядер = " << i << endl;
        cout << FindMinimumsParal(matrix10, n10, m10,i);
        cout << FindMinimumsSection(matrix10, n10, m10,i);
    }
    cout << endl;
    cout << FindMinimums(matrix10, n10, m10);
    for (int i = 0; i < n10; i++) delete[] matrix10[i];
    delete[] matrix10;

    double** matrix100;
    matrix100 = VvodMat("fin_100.txt", n100, m100);
    cout << "\nРазмерность матрицы 100 \n";
    for (int i = 1; i < 5; i++) {
        cout << "\nколичество ядер = " << i << endl;
        cout << FindMinimumsParal(matrix100, n100, m100, i);
        cout << FindMinimumsSection(matrix100, n100, m100, i);
    }
    cout << endl;
    cout << FindMinimums(matrix100, n100, m100);
    for (int i = 0; i < n100; i++) delete[] matrix100[i];
    delete[] matrix100;

    double** matrix1000;
    matrix1000 = VvodMat("fin_1000.txt", n1000, m1000);
    cout << "\nРазмерность матрицы 1000\n";
    for (int i = 1; i < 5; i++) {
        cout << "\nколичество ядер = " << i << endl;
        cout << FindMinimumsParal(matrix1000, n1000, m1000, i);
        cout << FindMinimumsSection(matrix1000, n1000, m1000, i);
    }
    cout << endl;
    cout << FindMinimums(matrix1000, n1000, m1000);
    for (int i = 0; i < n1000; i++) delete[] matrix1000[i];
    delete[] matrix1000;

    double** matrix10000;
    matrix10000 = VvodMat("fin_10000.txt", n10000, m10000);
    cout << "\nРазмерность матрицы 10000\n";
    for (int i = 1; i < 5; i++) {
        cout << "\nколичество ядер = " << i << endl;
        cout << FindMinimumsParal(matrix10000, n10000, m10000, i);
        cout << FindMinimumsSection(matrix10000, n10000, m10000, i);
    }
    cout << endl;
    cout << FindMinimums(matrix10000, n10000, m10000);

    for (int i = 0; i < n10000; i++) delete[] matrix10000[i];
    delete[] matrix10000;
}
