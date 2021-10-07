#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// length
#define LENARRAY 50
#define LENNUMBER 38

typedef struct str
{
	char arr[LENNUMBER];
	int lastfilledIndex;
	bool empty;
	char opr;
	float num;
} NUMBER;

typedef struct arr
{
	NUMBER arr[LENARRAY];
	int lastfilledIndex;
} Array;

// constants
NUMBER emptyNumber = {{}, 0};
NUMBER MinNumber = {{'-'}, 1};
NUMBER PlusNumber = {{'+'}, 1};
NUMBER divNumber = {{'/'}, 1};
NUMBER MultNumber = {{'*'}, 1};
NUMBER PowerNumber = {{'^'}, 1};
NUMBER RootNumber = {{'r'}, 1};


int LastFilledIndexArray(Array arr)
{
	for (int i = 0; i < LENARRAY; i++)
	{
		if (arr.arr[i].empty == true)
		{
			return i;
		}
	}
	return LENARRAY - 1;
}

int LastFilledIndexNum(NUMBER num)
{
	for (int i = 0; i < LENNUMBER; i++)
	{
		if (num.arr[i] == '\0')
		{
			return i;
		}
	}
	return LENNUMBER - 1;
}

void append2Num(NUMBER *num, char c)
{
	int l = num->lastfilledIndex;
	num->arr[l] = c;
	num->lastfilledIndex += 1;
	num->empty = false;
}
// fillwithnull
void fillNull(NUMBER *num)
{
	int len = strlen(num->arr);
	for (int i = 0; i < len; i++)
	{
		num->arr[i] = '\0';
	}
}

void fillNullCharArray(char * arr  ){
	memset(arr , '\0' , sizeof(arr) ) ; 
}


void fillnullArray(Array *arr)
{
	for (int i = 0; i < LENARRAY; i++)
	{
		NUMBER emptynumber = {{}, 0, true};
		arr->arr[i] = emptynumber;
		arr->arr[i].empty = true;
	}
}

void append2charArray(char * arr , int len , char c){
	for (int i = 0; i < len ; i++)
	{
		if( arr[i] == '\0') {
			arr[i] = c ; 
			break ;
		}  
	}
	
}

void append2Array(Array *arr, NUMBER num)
{
	int l = arr->lastfilledIndex;
	arr->arr[l] = num;
	arr->lastfilledIndex++;
}

void push2Num(char c, NUMBER *num)
{
	NUMBER tempNum = *num;
	fillNull(&*num);
	num->arr[0] = c;
	num->lastfilledIndex = LastFilledIndexNum(*num);
	for (int i = 0; i < LENNUMBER; i++)
	{
		append2Num(&*num, tempNum.arr[i]);
	}
}

double Num2F(NUMBER num)
{
	return atof(num.arr);
}


int Arr2I(const char* arr)
{
	return (int) atof(arr);
}

float Arr2F(const char* arr)
{
	return  atof(arr);
}
// print array and number :
void PrintNum(NUMBER num)
{
	printf("{");
	printf("%c", num.arr[0]);
	for (int i = 1; i < LENNUMBER; i++)
	{
		if (num.arr[i] == '\0')
		{
			break;
		}
		printf("%c", num.arr[i]);
	}

	//printf(" : %d , empty = %d , char = %c } \n  ", num.lastfilledIndex, num.empty , num.opr);
	printf(" : %d } \n ", num.lastfilledIndex);
}

NUMBER float2number(double value)
{
	NUMBER num = {{}, 0};
	if (value < 0)
	{
		double f = value * -1;
		gcvt(f, LENNUMBER, num.arr);
		push2Num('-', &num);
	}
	else
	{
		gcvt(value, LENNUMBER, num.arr);
	}
	num.lastfilledIndex = LastFilledIndexNum(num);
	return num;
}

bool is_digit(char c){
	switch (c)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
		return true ; 
		break;
	
	default:
		return false ; 
		break;
	}
}
// append to array to string and array

// equal funtion
bool is_array(char *arr1, char *arr2)
{
	if (strlen(arr1) != strlen(arr2))
	{
		return false;
	}
	for (int i = 0; i < LENNUMBER; i++)
	{
		if (arr1[i] == arr2[i])
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}
bool is_str(char *arr1, char *arr2)
{
	int len = strlen(arr1);
	for (int i = 0; i < len; i++)
	{
		if (arr1[i] == arr2[i])
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}
bool is_arrayDifferSize(char *arr1, char *arr2 , int tosize )
{
	// if (strlen(arr1) != strlen(arr2))
	// {
	// 	return false;
	// }
	int len = strlen(arr1);
	for (int i = 0; i < tosize; i++)
	{
		if (arr1[i] == arr2[i])
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void removeChar(char *str, char c) {
    int ofMv = 0, cnt = 0;
    
    for(; str[cnt] != 0; ofMv++) {
    
        if(str[ofMv] == c && str[ofMv])
            continue;
    
        str[cnt] = str[ofMv];
        cnt++;
    
        if(!str[ofMv])
            return;
    }
}

bool is_equalNum(NUMBER num1, NUMBER num2)
{

	if (is_array(num1.arr, num2.arr))
	{
		if (num1.lastfilledIndex == num2.lastfilledIndex)
		{
			return true;
		}
	}

	return false;
}

// in funtion

bool charInStr(char *arr, char c)
{
	for (int i = 0; i < strlen(arr); i++)
	{
		if (arr[i] == c)
		{
			printf("char In str() : arr[%d] == %c \n" , i  , c); 
			return true;
		}
	}
	return false;
}

bool charInArray(char c, Array arr)
{
	for (int i = 0; i < LENARRAY; i++)
	{
		if (charInStr(arr.arr[i].arr, c))
		{
			return true;
		}
	}
	return false;
}

bool NumInArray(NUMBER num, Array arr)
{
	NUMBER emptynum = {{}, 0, true};

	if (is_equalNum(num, emptynum))
	{ // if number is empty
		return false;
	}

	for (int i = 0; i < LENARRAY; i++)
	{
		if (is_equalNum(num, arr.arr[i]))
		{
			printf("Num in Array (): FOUND A NUMBER IN ARRAY \n "); 
			PrintNum(arr.arr[i]) ; 
			return true;
		}
	}
	return false;
}

bool intInIntArray(int j, int arr[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (j == arr[i])
		{
			printf("i = %d and arr have %d \n", j, arr[i]);
			return true;
		}
	}
	return false;
}

int sizeOfArray(Array arr){
	int size = 0 ; 
	for(int i = 0 ; i < LENARRAY ; i++){
		size++ ; 
		if (is_equalNum(arr.arr[i] , emptyNumber)) return size ;
		 
	}

}

// calculate
double calc(NUMBER num1, NUMBER num2, NUMBER opr)
{
	double result = 0;
	double float1 = Num2F(num1); // using varable name as float instead of double because am lazy
	double float2 = Num2F(num2);

	switch (opr.arr[0])
	{
	case '-':
		result = float1 - float2;
		break;
	case '+':
		result = float1 + float2;
		break;
	case '/':
		result = float1 / float2;
		break;
	case '*':
		result = float1 * float2;
		break;
	case 'r':
		result = (double)pow(float2, 1.0 / float1);
		break;
	case '^':
		result = (double)pow(float1, float2);
		break;
	default:
		break;
	}
	printf("result of %c is %f \n", opr.arr[0], result);
	return result;
}
void printIntArr(int * arr){
	int size =  sizeof(arr) / sizeof(int) ; 
	printf("int Array (%d): { \n" , size);
	for(int i = 0 ; i < size ; i ++ )
	{
		printf("%d , " , arr[i]) ; 
	}
	printf(" \n } ");
	
}
void PrintArray(Array arr)
{
	printf("Array (%d): [ \n ", LENARRAY);
	for (int i = 0; i < LENARRAY; i++)
	{
		if (arr.arr[i].empty == true)
		{
			break;
		}
		PrintNum(arr.arr[i]);
	}
	printf("] : %d \n", arr.lastfilledIndex);
}
