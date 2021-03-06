#include "pch.h"
#include <iostream>
#include <ctime>

using namespace std;

void generate(short* array, int size)  //Заполнение массива псевдослучайными числами от 0 до 999
{
	for (int i = 0; i < size; i++)
	{
		array[i] = rand() % 1000;
		if (!(rand() % 3)) // Случайное умножение(1к2) этих чисел на -1
			array[i] *= -1;
	}
}

int main()
{
	srand(time(NULL));
    const int size = 20;
	short a1[size], a2[size], result[size];
	generate(a1, size);
	generate(a2, size); 

	cout << "Array 1: ";
	for (int i = 0; i < size; i++)
		cout << a1[i] << " ";
	cout << "\nArray 2: ";
	for (int i = 0; i < size; i++)
		cout << a2[i] << " ";

	//C
	int delay = 1000000; //Выставление задержки, без нее будет сложно оценить быстродействие
	time_t start = clock();
	for (int j = 0; j < delay; j++)
	{
		for (int i = 0; i < size; ++i)
			result[i] = a1[i] >= a2[i] ? a1[i] : a2[i];
	}
		time_t end = clock();

		cout << "\nResult C (" << (end - start) << " ms):	";
		for (int i = 0; i < size; i++)
			cout << result[i] << " ";
	
	//ASM
	int buffer, a, b;
	start = clock();
	for (int j = 0; j < delay; j++)
	{
		for (int i = 0; i < size; ++i)
		{
			a = a1[i];
			b = a2[i];
			_asm 
			{
				pusha
				xor eax, eax          //Установка регистров в ноль
					xor ebx, ebx
					mov eax, a
					mov ebx, b

					cmp eax, ebx
					jl a1_is_lower         //jump к a1_is_lower с пропуском строчек, если eax < ebx(a1<a2)
					mov buffer, eax
					jmp finish

					a1_is_lower :
					mov buffer, ebx

						finish :
					popa
			}
				result[i] = buffer;
		}
	}
		end = clock();

		cout << "\nResult ASM (" << (end - start) << " ms):	";
		for (int i = 0; i < size; i++)
			cout << result[i] << " ";
		cout << std::endl;

		//MMX
		const int N = size / (8 / sizeof(*a1)); //Количество итераций цикла 

		start = clock();
		for (int i = 0; i < delay; ++i)
		{
			_asm
			{
				xor esi, esi  //Установка регистров в ноль
				xor ecx, ecx
				pxor MM0, MM0
				pxor MM1, MM1

			    mov ecx, N   
				Loop1 :
				movq MM0, a1[esi]
					movq MM1, a2[esi]

					pmaxsw MM0, MM1

					movq result[esi], MM0

					add esi, 8
					loop Loop1

					emms  // Вернуть режим сопроцессора
			}
		}
		end = clock();

		cout << "Result MMX (" << (end - start) << " ms):	";
		for (int i = 0; i < size; i++)
			cout << result[i] << " ";
		cout << endl;

		system("pause");
		return 0;
}
